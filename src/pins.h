/* Copyright 2025 teamprof.net@gmail.com
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
/* Copyright 2025 teamprof.net@gmail.com
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
#pragma once

#define YD_RP2040_V1_0_0

///////////////////////////////////////////////////////////////////////////////
// YD_RP2040 on PCB pico-alarm-lamp v1.0.0
#ifdef YD_RP2040_V1_0_0

#define PIN_DEBUG_USB PIN_GPIO6 // comment out to disable toggle IO during USB callback
#define PIN_DEBUG_DMA PIN_GPIO7 // comment out to disable toggle IO during DMA callback
// #define PIN_DEBUG_AUDIO_TASK PIN_GPIO8 // comment out to disable toggle IO in audio_task

#define PIN_TX0 0
#define PIN_RX0 1
#define PIN_I2C1_SDA 2
#define PIN_I2C1_SCL 3
#define PIN_TX1 4
#define PIN_RX1 5
#define PIN_GPIO6 6
#define PIN_GPIO7 7
#define PIN_GPIO8 8
#define PIN_I2S_DI 9
#define PIN_I2S_BCLK 10
#define PIN_I2S_WS 11

#define PIN_WS2812 23 // on-board WS2812
#define PIN_LED_OP 25 // PICO_DEFAULT_LED_PIN : on - board LED(Blue)

#endif // YD_RP2040_V1_0_0