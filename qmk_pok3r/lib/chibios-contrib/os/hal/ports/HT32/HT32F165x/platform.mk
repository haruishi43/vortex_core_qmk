# List of all the template platform files.
PLATFORMSRC = $(CHIBIOS)/os/hal/ports/common/ARMCMx/nvic.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/LLD/hal_lld.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/LLD/hal_gpt_lld.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/LLD/hal_pal_lld.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/LLD/hal_pwm_lld.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/LLD/hal_serial_lld.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/LLD/hal_spi_lld.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/LLD/hal_st_lld.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/LLD/hal_uart_lld.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/LLD/hal_usb_lld.c

# Required include directories
PLATFORMINC = $(CHIBIOS)/os/hal/ports/common/ARMCMx \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/LLD \
              $(CHIBIOS_CONTRIB)/os/hal/ports/HT32/HT32F165x
