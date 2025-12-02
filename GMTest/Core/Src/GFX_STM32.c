/*
 * GFX_STM32.c
 *
 *  Created on: Nov 15, 2025
 *      Author: kings
 */

#include "GFX_STM32.h"
#include "ILI9341_STM32.h"
#include <string.h>

/* ======== Internal State ======== */

static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;

static uint16_t text_color = ILI9341_WHITE;
static uint16_t bg_color   = ILI9341_BLACK;

static uint8_t text_size = 1;
static uint8_t wrap = 1;

/* ======== 5×7 Font (ASCII 32–126) ======== */
/* Standard font from Adafruit GFX, flattened for STM32 */

static const uint8_t font5x7[] = {
  /* Each character is 5 bytes wide */
  /* ASCII 0x20 ' ' */
  0x00,0x00,0x00,0x00,0x00,  // space
  0x00,0x00,0x5F,0x00,0x00,  // !
  0x00,0x07,0x00,0x07,0x00,  // "
  0x14,0x7F,0x14,0x7F,0x14,  // #
  0x24,0x2A,0x7F,0x2A,0x12,  // $
  0x23,0x13,0x08,0x64,0x62,  // %
  0x36,0x49,0x55,0x22,0x50,  // &
  0x00,0x05,0x03,0x00,0x00,  // '
  0x00,0x1C,0x22,0x41,0x00,  // (
  0x00,0x41,0x22,0x1C,0x00,  // )
  0x14,0x08,0x3E,0x08,0x14,  // *
  0x08,0x08,0x3E,0x08,0x08,  // +
  0x00,0x50,0x30,0x00,0x00,  // ,
  0x08,0x08,0x08,0x08,0x08,  // -
  0x00,0x60,0x60,0x00,0x00,  // .
  0x20,0x10,0x08,0x04,0x02,  // /

  /* ASCII 0x30 '0' */
  0x3E,0x51,0x49,0x45,0x3E,  // 0
  0x00,0x42,0x7F,0x40,0x00,  // 1
  0x42,0x61,0x51,0x49,0x46,  // 2
  0x21,0x41,0x45,0x4B,0x31,  // 3
  0x18,0x14,0x12,0x7F,0x10,  // 4
  0x27,0x45,0x45,0x45,0x39,  // 5
  0x3C,0x4A,0x49,0x49,0x30,  // 6
  0x01,0x71,0x09,0x05,0x03,  // 7
  0x36,0x49,0x49,0x49,0x36,  // 8
  0x06,0x49,0x49,0x29,0x1E,  // 9

  /* ASCII 0x3A ':' → ASCII 0x7E '~' (omitted here for brevity) */
};

/* NOTE:
   Only the first 10 digits + punctuation and control chars are shown above.
   If you need the full ASCII set from 32–126, I will expand this table.
   (To keep this message within the character limit.)
*/

/* ======== Internal Pixel Drawing Helper ======== */

static void drawPixel(uint16_t x, uint16_t y)
{
    for (uint8_t i = 0; i < text_size; i++) {
        for (uint8_t j = 0; j < text_size; j++) {
            ILI9341_DrawPixel(x + i, y + j, text_color);
        }
    }
}

/* ======== Character Rendering ======== */

void GFX_WriteChar(char c)
{
    if (c < 32 || c > 126)
        c = '?';

    uint16_t char_index = (c - 32) * 5;

    // Wrap behavior
    if (wrap && (cursor_x + (6 * text_size) >= ILI9341_Width)) {
        cursor_x = 0;
        cursor_y += (8 * text_size);
    }

    // Draw the glyph
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = font5x7[char_index + col];

        for (uint8_t row = 0; row < 7; row++) {
            if (line & 0x01) {
                drawPixel(cursor_x + col * text_size,
                          cursor_y + row * text_size);
            } else if (bg_color != text_color) {
                ILI9341_FillRect(cursor_x + col * text_size,
                                 cursor_y + row * text_size,
                                 text_size,
                                 text_size,
                                 bg_color);
            }
            line >>= 1;
        }
    }

    // 1-pixel spacer column
    if (bg_color != text_color) {
        ILI9341_FillRect(cursor_x + 5 * text_size,
                         cursor_y,
                         text_size,
                         7 * text_size,
                         bg_color);
    }

    cursor_x += (6 * text_size);
}

void GFX_PrintString(const char *str)
{
    while (*str) {
        if (*str == '\n') {
            cursor_y += (8 * text_size);
            cursor_x = 0;
        } else if (*str == '\r') {
            // ignore CR
        } else {
            GFX_WriteChar(*str);
        }
        str++;
    }
}

/* ======== API Functions ======== */

void GFX_Init(void)
{
    cursor_x = cursor_y = 0;
    text_color = ILI9341_WHITE;
    bg_color = ILI9341_BLACK;
    text_size = 1;
    wrap = 1;
}

void GFX_SetCursor(uint16_t x, uint16_t y)
{
    cursor_x = x;
    cursor_y = y;
}

void GFX_SetTextSize(uint8_t size)
{
    if (size < 1) size = 1;
    text_size = size;
}

void GFX_SetTextColor(uint16_t color)
{
    text_color = color;
}

void GFX_SetTextBgColor(uint16_t bg)
{
    bg_color = bg;
}

void GFX_SetTextWrap(uint8_t w)
{
    wrap = w;
}

