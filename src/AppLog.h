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
#include <stdio.h>
#include <string.h>


// #undef DEBUG_LOG_LEVEL // disable debug log
#define DEBUG_LOG_LEVEL Debug

enum LogLevel
{
    Error = 0,
    Info,
    Debug
};

#define PRINTLN(msg, ...)            \
    {                                \
        printf(msg, ##__VA_ARGS__); \
        printf("\n");               \
    }

#ifdef DEBUG_LOG_LEVEL

#define __FILENAME__ (__FILE__ + strlen(__FILE__) - strlen(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__))

#define DBGLOG(logLevel, msg, ...)                                      \
    if (logLevel <= DEBUG_LOG_LEVEL)                                    \
    {                                                                   \
        printf("[%s line %d] %s: ", __FILENAME__, __LINE__, __func__); \
        printf(msg, ##__VA_ARGS__);                                    \
        printf("\n");                                                  \
    }
#define DBGLOG_ARRAY(logLevel, title, fmt, array, length)                        \
    if (logLevel <= DEBUG_LOG_LEVEL)                                             \
    {                                                                            \
        printf("[%s line %d] %s: %s", __FILENAME__, __LINE__, __func__, title); \
        for (size_t i = 0; i < length; i++)                                      \
        {                                                                        \
            xprintf(fmt, array[i]);                                              \
        }                                                                        \
        printf("\n");                                                           \
    }
#else
#define DBGLOG(logLevel, msg, ...)
#endif


