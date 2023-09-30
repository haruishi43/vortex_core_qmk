# vortex_core_qmk

My vortex core qmk setup

# TLDR: Command

```Bash
# make
export PATH=~/Library/xPacks/@xpack-dev-tools/arm-none-eabi-gcc/8.3.1-1.4.1/.content/bin:$PATH
cd qmk_pok3r

# edit rules
vim keyboards/vortex/core/rules.mk

# edit keymap
vim keyboards/vortex/keymaps/default/keymap.c

make vortex/core:default

# apply
cd pok3rtool/pok3rtool-build
sudo ./pok3rtool -t vortex-core flash QMK_CORE ../../qmk_pok3r/vortex_core_default.bin

# when it fails repeat!
sudo ./pok3rtool -t vortex-core bootloader
sudo ./pok3rtool -t vortex-core flash QMK_CORE ../../qmk_pok3r/vortex_core_default.bin
```

# Steps to unlock

## Compile OpenOCD-ht32

OpenOCD is a standard tool / interface that allows you to use many different types of hardware debuggers interchangeably. It is a very useful project for tasks like this, where we will need to connect directly into an embedded chip via its debugging ports.

```Bash
# dependencies
brew install automake

# git clone
git clone --recursive https://github.com/ChaoticEnigma/openocd-ht32.git

# configure the build system
./bootstrap

# These arguments may change depending on the hardware debugger you are using. I use the ST-LinkV2
# Check the official OpenOCD documentation for more information on this
./configure --host=x86_64-w64-mingw32 --disable-werror

# Build
make
sudo make install

```

## pok3rtool

```bash

git clone --recursive https://github.com/pok3r-custom/pok3rtool.git

cd pok3rtool

# Pull the git submodules
git submodule update --init --recursive

cd ../pok3rtool-build

# Build the tool
cmake ../pok3rtool
make
```


## Firmware and edit keymap

I'm a bit confused on how this works...


```bash

# just clone it for now
git clone --recursive https://github.com/pok3r-custom/pok3r_re_firmware.git
git clone --recursive https://github.com/pok3r-custom/qmk_pok3r.git

# dependencies gcc-8.3.0
# brew installs the latest version which doesn't compile correctly, so don't do this:
# brew install armmbed/formulae/arm-none-eabi-gcc
# reference: https://www.reddit.com/r/olkb/comments/bnf0l9/planck_chibios_compiling_error_even_on_default/
# instead, install gcc using xpm
npm install --location=global xpm@latest
xpm install --global @xpack-dev-tools/arm-none-eabi-gcc@8.3.1-1.4.1
# you also need to put this in your path
export PATH=~/Library/xPacks/@xpack-dev-tools/arm-none-eabi-gcc/8.3.1-1.4.1/.content/bin:$PATH
# reference: https://qiita.com/baggio/items/35f682e9effbca36cd97
# reference: https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack


# edit keymap
cd qmk_pok3r
vim keyboards/vortex/keymaps/default/keymap.c
# Edit the part below #elif defined(KEYMAP_VORTEX_CORE)

# WARN: `make` command will fail on macOS unless tmk_core/rules.mk line 381 is changed from `wc --bytes ...` to `wc -c ...`!!!!

# make firmware
make vortex/core:default
```


## Unlocking vortex core

Connect ST-link to keyboard (also make sure to connect the usb to the keyboard for power).

```bash
# dependencies
brew install telnet

# go to openocd directory
cd openocd-ht32/tcl

# start connection
openocd -c 'set HT32_SRAM_SIZE 0x4000' -c 'set HT32_FLASH_SIZE 0x10000' -f ./interface/stlink-v2.cfg -f ./target/ht32f165x.cfg

# on a different terminal, run:
telnet 127.0.0.1 4444
```

```
> halt


> ht32f165x mass_erase 0

# make sure not to bump!! you will wait for a couple minutes
> flash write_image /Users/haruyaishikawa/vortex/pok3r_re_firmware/disassemble/core/builtin_core/firmware_builtin_core.bin 0


> exit
```

You can remove the usb and debugger.

You might want to keep the soldered debugger cables since the firmware might not be installed correctly sometimes (it happens frequently than you think).


## Using QMK

Now for the fun part.
See if the system recognizes the keyboard and you can type correctly (the firmware is pretty much the initial configuration of Core). If so, run the following commands:

```Bash
cd /path/to/pok3rtool/pok3rtool-build
# list devices
sudo ./pok3rtool list -v
```

If the command doesn't recognize the device, the culprit could be Karabiner or some software that creates virtual keyboard.

```Bash
# flash qmk
sudo ./pok3rtool -t vortex-core flash QMK_CORE ../../qmk_pok3r/vortex_core_default.bin
```


# My Keymap

References:
- [Detailed `make` instructions](https://github.com/qmk/qmk_firmware/blob/master/docs/getting_started_make_guide.md)
- [Keycodes](https://github.com/pok3r-custom/qmk_pok3r/blob/master/docs/keycodes.md)
	- [Basic Keycodes](https://github.com/pok3r-custom/qmk_pok3r/blob/master/docs/keycodes_basic.md)
	- [Quantum Keycodes](https://github.com/pok3r-custom/qmk_pok3r/blob/master/docs/quantum_keycodes.md)
	- [Advanced Keycodes](https://github.com/pok3r-custom/qmk_pok3r/blob/master/docs/feature_advanced_keycodes.md)
- [Keymap](https://github.com/pok3r-custom/qmk_pok3r/blob/e2d470113a0db497d866be9bda56af7378180602/docs/keymap.md)
- Advanced:
	- Layer-Tap: `LT(layer, kc)`: Turn on `layer` when held, `kc` when tapped
	- [Tap Dance](https://github.com/pok3r-custom/qmk_pok3r/blob/master/docs/feature_tap_dance.md)
	- [Leader Key](https://github.com/pok3r-custom/qmk_pok3r/blob/master/docs/feature_leader_key.md)

# License

- each subdirectories are copies of the original repositories, so they are licensed under the original licenses.
