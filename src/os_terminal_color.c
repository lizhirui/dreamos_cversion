/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-18     lizhirui     the first version
 */

#include <dreamos.h>

void terminal_color_set_fg(terminal_color_t color)
{
    os_printf("\033[%dm",color);
}

void terminal_color_set_bg(terminal_color_t color)
{
    os_printf("\033[%d;",color + 10);
}

void terminal_color_set(terminal_color_t fg,terminal_color_t bg)
{
    os_printf("\033[%d;%dm",bg + 10,fg);
}