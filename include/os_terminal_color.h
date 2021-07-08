/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-18     lizhirui     the first version
 */

// @formatter:off
#ifndef __OS_TERMINAL_COLOR_H__
#define __OS_TERMINAL_COLOR_H__

    typedef enum
    {
        TERMINAL_COLOR_BLACK = 30,
        TERMINAL_COLOR_RED,
        TERMINAL_COLOR_GREEN,
        TERMINAL_COLOR_YELLOW,
        TERMINAL_COLOR_CYAN,
        TERMINAL_COLOR_PURPLE,
        TERMINAL_COLOR_BLUE,
        TERMINAL_COLOR_WHITE
    }terminal_color_t;

    void terminal_color_set_fg(terminal_color_t color);
    void terminal_color_set_bg(terminal_color_t color);
    void terminal_color_set(terminal_color_t fg,terminal_color_t bg);

#endif