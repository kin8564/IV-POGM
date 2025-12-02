/*
 * lcd_ui.h
 *
 *  Created on: Nov 13, 2025
 *      Author: kings
 */

#ifndef LCD_UI_H
#define LCD_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Initialize the LCD UI layout.
 *
 * - Clears screen
 * - Draws top area for current value
 * - Draws bottom area for graph
 * - Draws separator line and labels
 */
void LCD_UI_Init(void);

/**
 * @brief Set the label text displayed next to the current value.
 *        For example: "Glucose (mg/dL)" or "ADC Value".
 *
 * @param label  Null-terminated string (stored internally).
 */
void LCD_UI_SetLabel(const char *label);

/**
 * @brief Update the current value displayed in the top region.
 *
 * @param raw_value  Latest ADC or processed glucose value.
 */
void LCD_UI_UpdateCurrentValue(uint16_t raw_value);

/**
 * @brief Add a new sample to the graph and update the graph area.
 *
 * Call this once per new measurement (e.g., after each ADC conversion).
 *
 * @param raw_value  Latest ADC value (0–4095 assumed).
 */
void LCD_UI_AddSample(uint16_t raw_value);

/**
 * @brief Clear the graph area and reset the internal graph buffer.
 */
void LCD_UI_ClearGraph(void);

#ifdef __cplusplus
}
#endif

#endif // LCD_UI_H
