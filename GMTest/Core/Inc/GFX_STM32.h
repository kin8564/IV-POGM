/*
 * GFX_STM32.h
 *
 *  Created on: Nov 15, 2025
 *      Author: kings
 */

#ifndef INC_GFX_STM32_H_
#define INC_GFX_STM32_H_

#include <stdint.h>
#include "ILI9341_STM32.h"

// A simple GFX-like text rendering system for STM32

void GFX_Init(void);

void GFX_SetCursor(uint16_t x, uint16_t y);
void GFX_SetTextSize(uint8_t size);
void GFX_SetTextColor(uint16_t color);
void GFX_SetTextBgColor(uint16_t bg);
void GFX_SetTextWrap(uint8_t w);

void GFX_WriteChar(char c);
void GFX_PrintString(const char *str);

#endif /* INC_GFX_STM32_H_ */
