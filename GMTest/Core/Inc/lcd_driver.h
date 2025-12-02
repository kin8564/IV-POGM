/*
 * lcd_driver.h
 *
 *  Created on: Nov 15, 2025
 *      Author: kings
 */

#ifndef INC_LCD_DRIVER_H_
#define INC_LCD_DRIVER_H_

#include <stdint.h>
#include "ILI9341_STM32.h"

// Generic names expected by lcd_ui
#define LCD_BLACK     ILI9341_BLACK
#define LCD_WHITE     ILI9341_WHITE
#define LCD_RED       ILI9341_RED
#define LCD_GREEN     ILI9341_GREEN
#define LCD_BLUE      ILI9341_BLUE
#define LCD_YELLOW    ILI9341_YELLOW
#define LCD_CYAN      ILI9341_CYAN
#define LCD_MAGENTA   ILI9341_MAGENTA
#define LCD_DARKGREY  ILI9341_DARKGREY
#define LCD_LIGHTGREY ILI9341_LIGHTGREY

void LCD_Init(void);
void LCD_FillScreen(uint16_t color);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
void LCD_DrawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);

/* Text functions (via GFX or custom text renderer) */
void LCD_SetCursor(uint16_t x, uint16_t y);
void LCD_SetTextSize(uint8_t size);
void LCD_SetTextColor(uint16_t color);
void LCD_Print(const char *str);
void LCD_DrawLine(uint16_t x0, uint16_t y0,
                  uint16_t x1, uint16_t y1,
                  uint16_t color);


#endif /* INC_LCD_DRIVER_H_ */
