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
#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#include "ws2812.h"

#include "../pins.h"
#include "../AppLog.h"

#define IS_RGBW true
#define NUM_PIXELS 1

#define WS2812_PIN PIN_WS2812

namespace ws2812
{
    static PIO pio;
    static uint sm;
    static uint offset;

    void put_pixel(uint32_t color)
    {
        pio_sm_put_blocking(pio, sm, color);
        pio_sm_clear_fifos(pio, sm);
    }

    static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
    {
        return ((uint32_t)(r) << 8) |
               ((uint32_t)(g) << 16) |
               (uint32_t)(b);
    }

    const uint32_t RED = urgb_u32(0xff, 0, 0) << 8u;
    const uint32_t GREEN = urgb_u32(0, 0xff, 0) << 8u;
    const uint32_t BLUE = urgb_u32(0, 0, 0xff) << 8u;
    const uint32_t WHITE = urgb_u32(0xff, 0xff, 0xff) << 8u;
    const uint32_t BLACK = urgb_u32(0, 0, 0) << 8u;

    void init(void)
    {
        DBGLOG(Debug, "WS2812 pin=%d", WS2812_PIN);

        // PIO pio = ws2812_pio;
        // int sm = 0;
        // uint offset = pio_add_program(pio, &ws2812_program);

        // PIO pio;
        // uint sm;
        // uint offset;
        bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
        hard_assert(success);

        ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
    }

    void deinit(void)
    {
        pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);
    }
}
