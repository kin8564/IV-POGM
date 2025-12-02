/*
 * lcd_driver.c
 *
 *  Created on: Nov 15, 2025
 *      Author: kings
 */

#include "lcd_driver.h"
#include "GFX_STM32.h"    // text, cursor, font rendering
#include <stdlib.h>

void LCD_Init(void)
{
    ILI9341_Init();
    GFX_Init();      // sets rotation, default font, etc.
}

void LCD_FillScreen(uint16_t color)
{
    ILI9341_FillScreen(color);
}

void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    ILI9341_FillRect(x, y, w, h, color);
}

void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    ILI9341_DrawPixel(x, y, color);
}

void LCD_DrawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color)
{
    ILI9341_DrawFastVLine(x, y, h, color);
}

void LCD_DrawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{
    ILI9341_DrawFastHLine(x, y, w, color);
}

/* Text layer */
void LCD_SetCursor(uint16_t x, uint16_t y)
{
    GFX_SetCursor(x, y);
}

void LCD_SetTextSize(uint8_t size)
{
    GFX_SetTextSize(size);
}

void LCD_SetTextColor(uint16_t color)
{
    GFX_SetTextColor(color);
}

void LCD_Print(const char *str)
{
    GFX_PrintString(str);
}

void LCD_DrawLine(uint16_t x0, uint16_t y0,
                  uint16_t x1, uint16_t y1,
                  uint16_t color)
{
    // Bresenham line (fast)
    int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy, e2;

    while (1) {
        ILI9341_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}


