/* Copyright 2026 teamprof.net@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <atomic>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hardware/dma.h"
#include "hardware/sync.h"

#include "tflite_model.h"

#include "dsp_pipeline.h"
#include "ml_model.h"
#include "pins.h"
#include "AppDef.h"
#include "AppLog.h"
#include "audio/audio_const.h"
#include "audio/i2s.h"
#include "ws2812/ws2812.h"

// system operating frequency
#define SYS_CLOCK_FREQ 102400
// #define SYS_CLOCK_FREQ 128000
// #define SYS_CLOCK_FREQ 132000

// select microphone digital gain
#define MIC_GAIN MIC_GAIN_X16

#define MIC_GAIN_X1 0
#define MIC_GAIN_X2 1
#define MIC_GAIN_X4 2
#define MIC_GAIN_X8 3
#define MIC_GAIN_X16 4
#define MIC_GAIN_X32 5

#define EVENT_I2S_DMA (1 << 1)
#define EVENT_PDM_DMA (1 << 2)

static std::atomic<int> app_events = ATOMIC_VAR_INIT(0);

static __ALIGNED(8) pioi2s::pio_i2s_t _i2s;
static_assert(alignof(_i2s) == 8, "Alignment of _i2s must be equal to 8 bytes");

static ML_DATA int16_t _audio_buffer[AUDIO_FRAME_LEN];
static q15_t input_q15[AUDIO_FRAME_LEN + (AUDIO_FFT_LEN / 2)];

static DSPPipeline dsp_pipeline(AUDIO_FFT_LEN);
static MLModel ml_model(tflite_model, 128 * 1024);

static int8_t *scaled_spectrum = nullptr;
static int32_t spectogram_divider;
static float spectrogram_zero_point;

static inline int16_t *get_buffer_ptr(void)
{
    return _audio_buffer;
}

static void dma_i2s_in_handler(void)
{
#ifdef PIN_DEBUG_DMA
    gpio_xor_mask(1u << PIN_DEBUG_DMA);
#endif

    int32_t *src = *(int32_t **)dma_hw->ch[_i2s.dma_ch_in_ctrl].read_addr;
    int16_t *dst = get_buffer_ptr();

    // convert MSB 24-bit to 16-bit audio data with digital gain
    for (int i = 0; i < DMA_BUFFER_SIZE; i += NUM_CHANNELS)
    {
        *dst++ = (int16_t)(*src >> (16 - MIC_GAIN));
        src += NUM_CHANNELS;
    }

    dma_hw->ints0 = 1u << _i2s.dma_ch_in_data; // clear the IRQ

    atomic_fetch_add(&app_events, EVENT_I2S_DMA);
}

static void init_io(void)
{
#ifdef PIN_DEBUG_USB
    gpio_init(PIN_DEBUG_USB);
    gpio_set_dir(PIN_DEBUG_USB, GPIO_OUT);
    gpio_put(PIN_DEBUG_USB, 0);
#endif

#ifdef PIN_DEBUG_DMA
    gpio_init(PIN_DEBUG_DMA);
    gpio_set_dir(PIN_DEBUG_DMA, GPIO_OUT);
    gpio_put(PIN_DEBUG_DMA, 0);
#endif

#ifdef PIN_DEBUG_AUDIO_TASK
    gpio_init(PIN_DEBUG_AUDIO_TASK);
    gpio_set_dir(PIN_DEBUG_AUDIO_TASK, GPIO_OUT);
    gpio_put(PIN_DEBUG_AUDIO_TASK, 0);
#endif
}

int main(void)
{
    // Set system clock to more evenly divide the audio frequencies
    set_sys_clock_khz(SYS_CLOCK_FREQ, true);

    stdio_init_all();

    DBGLOG(Debug, "pico-audio-ml-alarm-lamp");

    init_io();
    ws2812::init();
    ws2812::put_pixel(ws2812::BLUE);

    gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_PWM);

    uint pwm_slice_num = pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN);
    uint pwm_chan_num = pwm_gpio_to_channel(PICO_DEFAULT_LED_PIN);

    // Set period of 256 cycles (0 to 255 inclusive)
    pwm_set_wrap(pwm_slice_num, 256);

    // Set the PWM running
    pwm_set_enabled(pwm_slice_num, true);

    if (!ml_model.init())
    {
        DBGLOG(Debug, "Failed to initialize ML model!");
        panic("Failed to initialize ML model!");
    }

    if (!dsp_pipeline.init())
    {
        DBGLOG(Debug, "Failed to initialize DSP Pipeline!");
        panic("Failed to initialize DSP Pipeline!");
    }

    scaled_spectrum = (int8_t *)ml_model.input_data();
    spectogram_divider = 64 * ml_model.input_scale();
    spectrogram_zero_point = ml_model.input_zero_point();

    q15_t *capture_buffer_q15 = get_buffer_ptr();

    if (!pioi2s::master_in_mono_left_start(&pioi2s::i2s_config_default, dma_i2s_in_handler, &_i2s))
    {
        DBGLOG(Debug, "Failed to start pioi2s::master_in_mono_left_start()!");
        panic("Failed to start pioi2s::master_in_mono_left_start()!");
    }

    static bool color_red = false;

    while (true)
    {
        auto events = atomic_exchange_explicit(&app_events, 0, std::memory_order_release);
        if (events)
        {
            if (events & (EVENT_I2S_DMA | EVENT_PDM_DMA))
            {
#ifdef PIN_DEBUG_AUDIO_TASK
                gpio_xor_mask(1u << PIN_DEBUG_AUDIO_TASK);
#endif

                dsp_pipeline.shift_spectrogram(scaled_spectrum, SPECTROGRAM_SHIFT, 124);

                // move input buffer values over by INPUT_BUFFER_SIZE samples
                memmove(input_q15, &input_q15[AUDIO_FRAME_LEN], (AUDIO_FFT_LEN / 2));

                // copy new samples to end of the input buffer with a bit shift of INPUT_SHIFT
                arm_shift_q15(capture_buffer_q15, AUDIO_INPUT_SHIFT, input_q15 + (AUDIO_FFT_LEN / 2), AUDIO_FRAME_LEN);

                for (int i = 0; i < SPECTROGRAM_SHIFT; i++)
                {
                    dsp_pipeline.calculate_spectrum(
                        input_q15 + i * ((AUDIO_FFT_LEN / 2)),
                        scaled_spectrum + (129 * (124 - SPECTROGRAM_SHIFT + i)),
                        spectogram_divider, spectrogram_zero_point);
                }

                float prediction = ml_model.predict();
                bool detected = (prediction >= 0.5);
                if (detected != color_red)
                {
                    color_red = detected;
                    ws2812::put_pixel(color_red ? ws2812::RED : ws2812::BLUE);
                }

                pwm_set_chan_level(pwm_slice_num, pwm_chan_num, prediction * 255);

                continue;
            }
        }
        __WFE();
    }

    return 0;
}
