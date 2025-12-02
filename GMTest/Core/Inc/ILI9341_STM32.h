/*
 * ILI9341_STM32.h
 *
 *  Created on: Nov 13, 2025
 *      Author: kings
 */

#ifndef ILI9341_STM32_H
#define ILI9341_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include <stdint.h>

/* ======== USER CONFIG: PINS + SPI HANDLE ======== */

// Changed to match your ioc config (GPIO port/pins for CS, DC, !RST).
#define ILI9341_SPI_HANDLE      hspi1    // extern SPI_HandleTypeDef hspi1;

#define ILI9341_CS_GPIO_Port    GPIOC
#define ILI9341_CS_Pin          GPIO_PIN_8

#define ILI9341_DC_GPIO_Port    GPIOC
#define ILI9341_DC_Pin          GPIO_PIN_9

#define ILI9341_RST_GPIO_Port   NULL
#define ILI9341_RST_Pin         0

/* Screen size */
#define ILI9341_TFTWIDTH   240
#define ILI9341_TFTHEIGHT  320

/* ILI9341 command codes */
#define ILI9341_NOP        0x00
#define ILI9341_SWRESET    0x01
#define ILI9341_RDDID      0x04
#define ILI9341_RDDST      0x09

#define ILI9341_SLPIN      0x10
#define ILI9341_SLPOUT     0x11
#define ILI9341_PTLON      0x12
#define ILI9341_NORON      0x13

#define ILI9341_RDMODE     0x0A
#define ILI9341_RDMADCTL   0x0B
#define ILI9341_RDPIXFMT   0x0C
#define ILI9341_RDIMGFMT   0x0D
#define ILI9341_RDSELFDIAG 0x0F

#define ILI9341_INVOFF     0x20
#define ILI9341_INVON      0x21
#define ILI9341_GAMMASET   0x26
#define ILI9341_DISPOFF    0x28
#define ILI9341_DISPON     0x29

#define ILI9341_CASET      0x2A
#define ILI9341_PASET      0x2B
#define ILI9341_RAMWR      0x2C
#define ILI9341_RAMRD      0x2E

#define ILI9341_PTLAR      0x30
#define ILI9341_VSCRDEF    0x33
#define ILI9341_MADCTL     0x36
#define ILI9341_VSCRSADD   0x37
#define ILI9341_PIXFMT     0x3A

#define ILI9341_FRMCTR1    0xB1
#define ILI9341_FRMCTR2    0xB2
#define ILI9341_FRMCTR3    0xB3
#define ILI9341_INVCTR     0xB4
#define ILI9341_DFUNCTR    0xB6

#define ILI9341_PWCTR1     0xC0
#define ILI9341_PWCTR2     0xC1
#define ILI9341_PWCTR3     0xC2
#define ILI9341_PWCTR4     0xC3
#define ILI9341_PWCTR5     0xC4
#define ILI9341_VMCTR1     0xC5
#define ILI9341_VMCTR2     0xC7

#define ILI9341_RDID1      0xDA
#define ILI9341_RDID2      0xDB
#define ILI9341_RDID3      0xDC
#define ILI9341_RDID4      0xDD

#define ILI9341_GMCTRP1    0xE0
#define ILI9341_GMCTRN1    0xE1

/* Color definitions (16-bit 565) */
#define ILI9341_BLACK       0x0000
#define ILI9341_NAVY        0x000F
#define ILI9341_DARKGREEN   0x03E0
#define ILI9341_DARKCYAN    0x03EF
#define ILI9341_MAROON      0x7800
#define ILI9341_PURPLE      0x780F
#define ILI9341_OLIVE       0x7BE0
#define ILI9341_LIGHTGREY   0xC618
#define ILI9341_DARKGREY    0x7BEF
#define ILI9341_BLUE        0x001F
#define ILI9341_GREEN       0x07E0
#define ILI9341_CYAN        0x07FF
#define ILI9341_RED         0xF800
#define ILI9341_MAGENTA     0xF81F
#define ILI9341_YELLOW      0xFFE0
#define ILI9341_WHITE       0xFFFF
#define ILI9341_ORANGE      0xFD20
#define ILI9341_GREENYELLOW 0xAFE5
#define ILI9341_PINK        0xFC18

/* Public API */

extern uint16_t ILI9341_Width;
extern uint16_t ILI9341_Height;

void ILI9341_Init(void);
void ILI9341_SetRotation(uint8_t r);

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_FillRect(uint16_t x, uint16_t y,
                      uint16_t w, uint16_t h, uint16_t color);
void ILI9341_DrawFastHLine(uint16_t x, uint16_t y,
                           uint16_t w, uint16_t color);
void ILI9341_DrawFastVLine(uint16_t x, uint16_t y,
                           uint16_t h, uint16_t color);

/* If you want to build a GFX-like layer or use lcd_ui on top, you’ll call
 * these primitives from that layer.
 */

#ifdef __cplusplus
}
#endif

#endif /* ILI9341_STM32_H */
