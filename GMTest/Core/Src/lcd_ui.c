/*
 * lcd_ui.c
 *
 *  Created on: Nov 13, 2025
 *      Author: kings
 */
#include "lcd_ui.h"

#include <string.h>
#include <stdio.h>

//#include "main.h"
//#include "spi.h"

#include "lcd_driver.h"

// -----------------------------------------------------------------------------
//  Configuration
// -----------------------------------------------------------------------------

// Physical screen size (portrait orientation).
#define SCREEN_W   320
#define SCREEN_H   240

// Layout: top 1/4 for current value, bottom 3/4 for graph.
#define TOP_H      (SCREEN_H / 4)            // 60 px
#define GRAPH_Y    (TOP_H)
#define GRAPH_H    (SCREEN_H - TOP_H)        // 180 px

// Graph resolution: one data point per horizontal pixel.
#define GRAPH_POINTS   (SCREEN_W)

// ADC range for mapping (adjust if you change resolution).
#define ADC_MAX        4095U

// Colors (change to match your color macros)
#ifndef LCD_COLOR_BLACK
#define LCD_COLOR_BLACK		ILI9341_BLACK
#endif

#ifndef LCD_COLOR_WHITE
#define LCD_COLOR_WHITE		ILI9341_WHITE
#endif

#ifndef LCD_COLOR_GREEN
#define LCD_COLOR_GREEN		ILI9341_GREEN
#endif

#ifndef LCD_COLOR_RED
#define LCD_COLOR_RED		ILI9341_RED
#endif

#ifndef LCD_COLOR_DARKGREY
#define LCD_COLOR_DARKGREY	ILI9341_DARKGREY
#endif

#define TOP_BG_COLOR       LCD_COLOR_DARKGREY
#define TOP_TEXT_COLOR     LCD_COLOR_WHITE

#define GRAPH_BG_COLOR     LCD_COLOR_BLACK
#define GRAPH_AXIS_COLOR   LCD_COLOR_WHITE
#define GRAPH_TRACE_COLOR  LCD_COLOR_GREEN

// -----------------------------------------------------------------------------
//  Internal state
// -----------------------------------------------------------------------------

static uint16_t graph_y[GRAPH_POINTS];
static uint16_t graph_index = 0;
static uint8_t  graph_initialized = 0;

static char current_label[32] = "ADC Value";

// -----------------------------------------------------------------------------
//  Internal helper functions
// -----------------------------------------------------------------------------

static uint16_t clamp_u16(uint16_t v, uint16_t max)
{
    return (v > max) ? max : v;
}

/**
 * @brief Map a raw ADC value to a Y coordinate in the graph region.
 *
 * Graph origin is at (x, GRAPH_Y) at top, but we want low ADC at bottom,
 * high ADC at top, so we invert.
 */
static uint16_t map_to_graph_y(uint16_t adc_val)
{
    adc_val = clamp_u16(adc_val, ADC_MAX);

    // Scale to 0 .. (GRAPH_H - 1)
    uint32_t scaled = ((uint32_t)adc_val * (GRAPH_H - 1U)) / ADC_MAX;

    // Invert so higher values are towards the top of the graph area.
    uint16_t y_local = (uint16_t)((GRAPH_H - 1U) - scaled);

    // Convert to absolute screen coordinate.
    return (uint16_t)(GRAPH_Y + y_local);
}

/**
 * @brief Draw static labels and separators after screen clear.
 */
static void draw_static_layout(void)
{
    // Draw top background.
    LCD_FillRect(0, 0, SCREEN_W, TOP_H, TOP_BG_COLOR);

    // Draw graph background.
    LCD_FillRect(0, GRAPH_Y, SCREEN_W, GRAPH_H, GRAPH_BG_COLOR);

    // Divider line between top and graph areas.
    LCD_DrawLine(0, TOP_H, SCREEN_W, TOP_H, LCD_COLOR_WHITE);

    // Label for current reading.
    LCD_SetCursor(8, 8);
    LCD_SetTextSize(2);
    LCD_SetTextColor(TOP_TEXT_COLOR);
    LCD_Print(current_label);

    // Label for graph.
    LCD_SetCursor(8, GRAPH_Y + 4);
    LCD_SetTextSize(2);
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_Print("Trend");

    // Optional: Draw a simple vertical "Y-axis" at left side of graph area.
    LCD_DrawFastVLine(0, GRAPH_Y, GRAPH_H, GRAPH_AXIS_COLOR);
}

/**
 * @brief Draw the axes or baseline for graph (if you want more decoration).
 */
static void draw_graph_axes(void)
{
    // Horizontal baseline at bottom of graph.
    LCD_DrawLine(0, GRAPH_Y + GRAPH_H - 1, SCREEN_W, GRAPH_Y + GRAPH_H - 1, GRAPH_AXIS_COLOR);
}

// -----------------------------------------------------------------------------
//  Public API
// -----------------------------------------------------------------------------

void LCD_UI_Init(void)
{
    // Low-level LCD init (provided by your driver).
    LCD_Init();

    // Clear screen and setup layout.
    LCD_FillScreen(LCD_COLOR_BLACK);
    draw_static_layout();
    draw_graph_axes();

    // Initialize the graph buffer to midline.
    uint16_t mid_y = map_to_graph_y(ADC_MAX / 2U);
    for (uint16_t i = 0; i < GRAPH_POINTS; ++i) {
        graph_y[i] = mid_y;
    }
    graph_index = 0;
    graph_initialized = 1;
}

void LCD_UI_SetLabel(const char *label)
{
    if (label == NULL) {
        return;
    }

    // Store label (truncate if too long).
    strncpy(current_label, label, sizeof(current_label) - 1U);
    current_label[sizeof(current_label) - 1U] = '\0';

    // Redraw the top region label.
    LCD_FillRect(0, 0, SCREEN_W, TOP_H, TOP_BG_COLOR);

    LCD_SetCursor(8, 8);
    LCD_SetTextSize(2);
    LCD_SetTextColor(TOP_TEXT_COLOR);
    LCD_Print(current_label);
}

/**
 * @brief Update the numeric display at the top of the screen.
 *        This clears only the numeric line, not the whole top region.
 */
void LCD_UI_UpdateCurrentValue(uint16_t raw_value)
{
    char buf[32];

    // Clear numeric area only (below the label).
    // Adjust Y/height if you change font sizes.
    LCD_FillRect(0, 36, SCREEN_W, TOP_H - 36, TOP_BG_COLOR);

    // Big numeric value.
    LCD_SetCursor(8, 40);
    LCD_SetTextSize(4);
    LCD_SetTextColor(TOP_TEXT_COLOR);

    snprintf(buf, sizeof(buf), "%u", raw_value);
    LCD_Print(buf);
}

void LCD_UI_ClearGraph(void)
{
    // Clear the graph area visually.
    LCD_FillRect(0, GRAPH_Y, SCREEN_W, GRAPH_H, GRAPH_BG_COLOR);

    // Redraw axes/baseline.
    draw_graph_axes();

    // Reset buffer to midline.
    uint16_t mid_y = map_to_graph_y(ADC_MAX / 2U);
    for (uint16_t i = 0; i < GRAPH_POINTS; ++i) {
        graph_y[i] = mid_y;
    }
    graph_index = 0;
    graph_initialized = 1;
}

/**
 * @brief Add a new sample and update the graph visually.
 *
 * This uses a circular buffer and draws a small vertical "dot" column
 * at the current X position. It erases only that column to keep SPI
 * bandwidth low.
 */
void LCD_UI_AddSample(uint16_t raw_value)
{
    if (!graph_initialized) {
        // Should not happen if LCD_UI_Init() was called, but safe guard.
        LCD_UI_ClearGraph();
    }

    uint16_t x = graph_index;
    uint16_t y = map_to_graph_y(raw_value);

    graph_y[x] = y;

    // Erase this column in the graph area.
    LCD_DrawFastVLine(x, GRAPH_Y, GRAPH_H, GRAPH_BG_COLOR);

    // Draw a small 3-pixel vertical "dot" centered at y for readability.
    for (int8_t dy = -1; dy <= 1; ++dy) {
        int16_t yy = (int16_t)y + dy;
        if ((yy >= GRAPH_Y) && (yy < (GRAPH_Y + GRAPH_H))) {
            LCD_DrawPixel(x, yy, GRAPH_TRACE_COLOR);
        }
    }

    // Optionally connect to previous sample (uncomment if you want a line graph).
    /*
    uint16_t prev_index = (x == 0U) ? (GRAPH_POINTS - 1U) : (x - 1U);
    uint16_t prev_y = graph_y[prev_index];
    LCD_DrawLine(prev_index, prev_y, x, y, GRAPH_TRACE_COLOR);
    */

    // Advance circular index.
    graph_index++;
    if (graph_index >= GRAPH_POINTS) {
        graph_index = 0;
    }
}


