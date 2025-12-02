/*
 * ILI9341_STM32.c
 *
 *  Created on: Nov 13, 2025
 *      Author: kings
 */

#include "ILI9341_STM32.h"
#include "spi.h"        // for ILI9341_SPI_HANDLE

/* Internal state: current width/height after rotation */
uint16_t ILI9341_Width  = ILI9341_TFTWIDTH;
uint16_t ILI9341_Height = ILI9341_TFTHEIGHT;

/* MADCTL bits */
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

/* ======== Low-level helpers ======== */

static void ILI9341_Select(void)
{
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_RESET);
}

static void ILI9341_Unselect(void)
{
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_SET);
}

static void ILI9341_DC_Command(void)
{
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_RESET);
}

static void ILI9341_DC_Data(void)
{
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);
}

static void ILI9341_WriteCommand(uint8_t cmd)
{
    ILI9341_Select();
    ILI9341_DC_Command();
    HAL_SPI_Transmit(&ILI9341_SPI_HANDLE, &cmd, 1, HAL_MAX_DELAY);
    ILI9341_Unselect();
}

static void ILI9341_Reset(void)
{
//    HAL_GPIO_WritePin(ILI9341_RST_GPIO_Port, ILI9341_RST_Pin, GPIO_PIN_RESET);
//    HAL_Delay(5);
//    HAL_GPIO_WritePin(ILI9341_RST_GPIO_Port, ILI9341_RST_Pin, GPIO_PIN_SET);
//    HAL_Delay(150);
	ILI9341_WriteCommand(ILI9341_SWRESET);
	HAL_Delay(150);
}

static void ILI9341_WriteData(uint8_t data)
{
    ILI9341_Select();
    ILI9341_DC_Data();
    HAL_SPI_Transmit(&ILI9341_SPI_HANDLE, &data, 1, HAL_MAX_DELAY);
    ILI9341_Unselect();
}

static void ILI9341_WriteDataBuffer(uint8_t *buff, uint16_t len)
{
    ILI9341_Select();
    ILI9341_DC_Data();
    HAL_SPI_Transmit(&ILI9341_SPI_HANDLE, buff, len, HAL_MAX_DELAY);
    ILI9341_Unselect();
}

static void ILI9341_WriteData16(uint16_t data)
{
    uint8_t buf[2] = { data >> 8, data & 0xFF };
    ILI9341_WriteDataBuffer(buf, 2);
}

/* ======== Initialization sequence (from Adafruit initcmd[]) ======== */

static const uint8_t ili9341_init_cmds[] = {
    0xEF, 3, 0x03, 0x80, 0x02,
    0xCF, 3, 0x00, 0xC1, 0x30,
    0xED, 4, 0x64, 0x03, 0x12, 0x81,
    0xE8, 3, 0x85, 0x00, 0x78,
    0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    0xF7, 1, 0x20,
    0xEA, 2, 0x00, 0x00,
    ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
    ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
    ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
    ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
    ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
    ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
    ILI9341_PIXFMT  , 1, 0x55,             // 16-bit pixel
    ILI9341_FRMCTR1 , 2, 0x00, 0x18,
    ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
    0xF2          , 1, 0x00,               // 3Gamma Function Disable
    ILI9341_GAMMASET , 1, 0x01,            // Gamma curve selected
    ILI9341_GMCTRP1 , 15,                   // Positive Gamma
        0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
        0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03,
        0x0E, 0x09, 0x00,
    ILI9341_GMCTRN1 , 15,                   // Negative Gamma
        0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
        0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C,
        0x31, 0x36, 0x0F,
    ILI9341_SLPOUT  , 0x80,                // Exit sleep, delay flag
    ILI9341_DISPON  , 0x80,                // Display on, delay flag
    0x00                                   // End of list
};

static void ILI9341_RunInitSequence(void)
{
    const uint8_t *addr = ili9341_init_cmds;

    while (1) {
        uint8_t cmd = *addr++;
        if (cmd == 0x00) {
            break;
        }

        uint8_t x = *addr++;
        uint8_t numArgs = x & 0x7F;

        ILI9341_WriteCommand(cmd);
        if (numArgs) {
            ILI9341_Select();
            ILI9341_DC_Data();
            HAL_SPI_Transmit(&ILI9341_SPI_HANDLE, (uint8_t *)addr, numArgs,
                             HAL_MAX_DELAY);
            ILI9341_Unselect();
            addr += numArgs;
        }

        if (x & 0x80) {
            HAL_Delay(150);
        }
    }
}

/* ======== Address window & drawing primitives ======== */

static void ILI9341_SetAddrWindow(uint16_t x0, uint16_t y0,
                                  uint16_t x1, uint16_t y1)
{
    /* Column address set */
    ILI9341_WriteCommand(ILI9341_CASET);
    uint8_t data_col[4] = {
        x0 >> 8, x0 & 0xFF,
        x1 >> 8, x1 & 0xFF
    };
    ILI9341_WriteDataBuffer(data_col, 4);

    /* Page address set */
    ILI9341_WriteCommand(ILI9341_PASET);
    uint8_t data_page[4] = {
        y0 >> 8, y0 & 0xFF,
        y1 >> 8, y1 & 0xFF
    };
    ILI9341_WriteDataBuffer(data_page, 4);

    /* RAM write */
    ILI9341_WriteCommand(ILI9341_RAMWR);
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x >= ILI9341_Width) || (y >= ILI9341_Height)) return;

    ILI9341_SetAddrWindow(x, y, x, y);
    ILI9341_WriteData16(color);
}

void ILI9341_DrawFastHLine(uint16_t x, uint16_t y,
                           uint16_t w, uint16_t color)
{
    if ((y >= ILI9341_Height) || (x >= ILI9341_Width)) return;

    if (x + w - 1 >= ILI9341_Width) {
        w = ILI9341_Width - x;
    }

    ILI9341_SetAddrWindow(x, y, x + w - 1, y);

    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint32_t total = w;
    ILI9341_Select();
    ILI9341_DC_Data();
    while (total--) {
        uint8_t buf[2] = {hi, lo};
        HAL_SPI_Transmit(&ILI9341_SPI_HANDLE, buf, 2, HAL_MAX_DELAY);
    }
    ILI9341_Unselect();
}

void ILI9341_DrawFastVLine(uint16_t x, uint16_t y,
                           uint16_t h, uint16_t color)
{
    if ((x >= ILI9341_Width) || (y >= ILI9341_Height)) return;

    if (y + h - 1 >= ILI9341_Height) {
        h = ILI9341_Height - y;
    }

    ILI9341_SetAddrWindow(x, y, x, y + h - 1);

    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint32_t total = h;
    ILI9341_Select();
    ILI9341_DC_Data();
    while (total--) {
        uint8_t buf[2] = {hi, lo};
        HAL_SPI_Transmit(&ILI9341_SPI_HANDLE, buf, 2, HAL_MAX_DELAY);
    }
    ILI9341_Unselect();
}

void ILI9341_FillRect(uint16_t x, uint16_t y,
                      uint16_t w, uint16_t h, uint16_t color)
{
    if ((x >= ILI9341_Width) || (y >= ILI9341_Height)) return;

    if (x + w - 1 >= ILI9341_Width)  w = ILI9341_Width  - x;
    if (y + h - 1 >= ILI9341_Height) h = ILI9341_Height - y;

    ILI9341_SetAddrWindow(x, y, x + w - 1, y + h - 1);

    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint32_t total = (uint32_t)w * h;

    ILI9341_Select();
    ILI9341_DC_Data();
    while (total--) {
        uint8_t buf[2] = {hi, lo};
        HAL_SPI_Transmit(&ILI9341_SPI_HANDLE, buf, 2, HAL_MAX_DELAY);
    }
    ILI9341_Unselect();
}

void ILI9341_FillScreen(uint16_t color)
{
    ILI9341_FillRect(0, 0, ILI9341_Width, ILI9341_Height, color);
}

/* ======== Rotation ======== */

void ILI9341_SetRotation(uint8_t m)
{
    m %= 4;
    uint8_t madctl = 0;

    switch (m) {
    case 0: // portrait
        madctl = (MADCTL_MX | MADCTL_BGR);
        ILI9341_Width  = ILI9341_TFTWIDTH;
        ILI9341_Height = ILI9341_TFTHEIGHT;
        break;
    case 1: // landscape
        madctl = (MADCTL_MV | MADCTL_BGR);
        ILI9341_Width  = ILI9341_TFTHEIGHT;
        ILI9341_Height = ILI9341_TFTWIDTH;
        break;
    case 2: // portrait flipped
        madctl = (MADCTL_MY | MADCTL_BGR);
        ILI9341_Width  = ILI9341_TFTWIDTH;
        ILI9341_Height = ILI9341_TFTHEIGHT;
        break;
    case 3: // landscape flipped
        madctl = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        ILI9341_Width  = ILI9341_TFTHEIGHT;
        ILI9341_Height = ILI9341_TFTWIDTH;
        break;
    }

    ILI9341_WriteCommand(ILI9341_MADCTL);
    ILI9341_WriteData(madctl);
}

/* ======== Public init ======== */

void ILI9341_Init(void)
{
    /* Make sure GPIO and SPI clocks & pins are already configured in CubeMX */
    ILI9341_Unselect();
    ILI9341_Reset();
    ILI9341_RunInitSequence();

    /* Default rotation */
    ILI9341_SetRotation(1);   // landscape for your UI
}

