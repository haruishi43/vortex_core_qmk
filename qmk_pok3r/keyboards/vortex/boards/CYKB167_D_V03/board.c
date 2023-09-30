/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio
                        (C) 2018 Charlie Waters

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"

#define PBIT(PORT, LINE) ((PAL_PORT(LINE) == PORT) ? (1 << PAL_PAD(LINE)) : 0)
#define PAFIO_L(PORT, LINE, AF) (((PAL_PORT(LINE) == PORT) && (PAL_PAD(LINE) < 8)) ? (AF << (PAL_PAD(LINE) << 2)) : 0)
#define PAFIO_H(PORT, LINE, AF) (((PAL_PORT(LINE) == PORT) && (PAL_PAD(LINE) >= 8)) ? (AF << ((PAL_PAD(LINE) - 8) << 2)) : 0)
#define PAFIO(PORT, N, LINE, AF) ((N) ? PAFIO_H(PORT, LINE, AF) : PAFIO_L(PORT, LINE, AF))

#define OUT_BITS(PORT) (\
    PBIT(PORT, LINE_ROW1) | \
    PBIT(PORT, LINE_ROW2) | \
    PBIT(PORT, LINE_ROW3) | \
    PBIT(PORT, LINE_ROW4) | \
    PBIT(PORT, LINE_ROW5) | \
    PBIT(PORT, LINE_ROW6) | \
    PBIT(PORT, LINE_ROW7) | \
    PBIT(PORT, LINE_ROW8) | \
    PBIT(PORT, LINE_ROW9) | \
    PBIT(PORT, LINE_ROW10) | \
    PBIT(PORT, LINE_SPI_CS) | \
0)

#define IN_BITS(PORT) (\
    PBIT(PORT, LINE_COL1) | \
    PBIT(PORT, LINE_COL2) | \
    PBIT(PORT, LINE_COL3) | \
    PBIT(PORT, LINE_COL4) | \
    PBIT(PORT, LINE_COL5) | \
    PBIT(PORT, LINE_COL6) | \
    PBIT(PORT, LINE_COL7) | \
    PBIT(PORT, LINE_COL8) | \
0)

#define AF_BITS(PORT, N) (\
    PAFIO(PORT, N, LINE_ROW1,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW2,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW3,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW4,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW5,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW6,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW7,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW8,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW9,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW10,    AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL1,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL2,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL3,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL4,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL5,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL6,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL7,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL8,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_SPI_SCK,  AFIO_SPI)  | \
    PAFIO(PORT, N, LINE_SPI_MOSI, AFIO_SPI)  | \
    PAFIO(PORT, N, LINE_SPI_MISO, AFIO_SPI)  | \
    PAFIO(PORT, N, LINE_SPI_CS,   AFIO_GPIO) | \
0)

/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
const PALConfig pal_default_config = {
    // GPIO A
    .setup[0] = {
        .DIR = OUT_BITS(IOPORTA),
        .INE = IN_BITS(IOPORTA),
        .PU = IN_BITS(IOPORTA),
        .PD = 0x0000,
        .OD = 0x0000,
        .DRV = 0x0000,
        .LOCK = 0x0000,
        .OUT = 0x0000,
        .CFG[0] = AF_BITS(IOPORTA, 0),
        .CFG[1] = AF_BITS(IOPORTA, 1),
    },
    // GPIO B
    .setup[1] = {
        .DIR = OUT_BITS(IOPORTB),
        .INE = IN_BITS(IOPORTB),
        .PU = IN_BITS(IOPORTB),
        .PD = 0x0000,
        .OD = 0x0000,
        .DRV = 0x0000,
        .LOCK = 0x0000,
        .OUT = 0x0000,
        .CFG[0] = AF_BITS(IOPORTB, 0),
        .CFG[1] = AF_BITS(IOPORTB, 1),
    },
    // GPIO C
    .setup[2] = {
        .DIR = OUT_BITS(IOPORTC),
        .INE = IN_BITS(IOPORTC),
        .PU = IN_BITS(IOPORTC),
        .PD = 0x0000,
        .OD = 0x0000,
        .DRV = 0x0000,
        .LOCK = 0x0000,
        .OUT = 0x0000,
        .CFG[0] = AF_BITS(IOPORTC, 0),
        .CFG[1] = AF_BITS(IOPORTC, 1),
    },
    // GPIO D
    .setup[3] = {
        .DIR = OUT_BITS(IOPORTD),
        .INE = IN_BITS(IOPORTD),
        .PU = IN_BITS(IOPORTD),
        .PD = 0x0000,
        .OD = 0x0000,
        .DRV = 0x0000,
        .LOCK = 0x0000,
        .OUT = 0x0000,
        .CFG[0] = AF_BITS(IOPORTD, 0),
        .CFG[1] = AF_BITS(IOPORTD, 1),
    },
    .ESSR[0] = 0x00000000,
    .ESSR[1] = 0x00000000,
};

const ioline_t row_list[MATRIX_ROWS] = {
    LINE_ROW1,
    LINE_ROW2,
    LINE_ROW3,
    LINE_ROW4,
    LINE_ROW5,
    LINE_ROW6,
    LINE_ROW7,
    LINE_ROW8,
    LINE_ROW9,
    LINE_ROW10,
};

const ioline_t col_list[MATRIX_COLS] = {
    LINE_COL1,
    LINE_COL2,
    LINE_COL3,
    LINE_COL4,
    LINE_COL5,
    LINE_COL6,
    LINE_COL7,
    LINE_COL8,
};

void __early_init(void) {
    ht32_clock_init();
}

#if HAL_USE_GPT == TRUE
// GPT Initialization

static const GPTConfig bftm0_config = {
    .frequency = 1000000,
    .callback = NULL,
};

void gpt_init(void) {
    gptStart(&GPTD_BFTM0, &bftm0_config);
}
#endif

// SPI Initialization

static const SPIConfig spi1_config = {
    .end_cb = NULL,
    .cr0 = SPI_CR0_SELOEN,
    .cr1 = 8 | SPI_CR1_FORMAT_MODE0 | SPI_CR1_MODE,
    .cpr = 1,
    .fcr = 0,
};

void spi_init(void) {
    spiStart(&SPID1, &spi1_config);
    palSetLine(LINE_SPI_CS);
}

/**
 * @brief   Board-specific initialization code.
 * @todo    Add your board-specific code, if any.
 */
void boardInit(void) {
#if HAL_USE_GPT == TRUE
    gpt_init();
#endif
    spi_init();
}

const uint8_t bootloader_magic[] __attribute__ ((section (".magic.bootloader"))) = {
    0x00, 0x00, 0x04, 0x03, // builtin bootloader CRC will fail if this word is not correct
    0x01, 0x00, 0x00, 0x34,
    0x04, 0x00, 0x02, 0x00, // probably not necessary
};
