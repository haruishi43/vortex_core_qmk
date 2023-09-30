#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

reasm="$1"

reas () {
    dir="$1"
    in="$dir/$2"
    out="$dir/$3"
    adr="$4"
    sym="$dir/$5"
    echo "$in"

    $reasm -EOA -a $adr -s "$sym" "$in" "$out" > /dev/null
    echo "  out: $out"
}

pushd $DIR > /dev/null

#    Directory                  Firmware Binary             Output Source               VMA     Symbol File

reas pok3r/builtin              firmware_builtin.bin        firmware_builtin_ref.s      0x0     symbols_builtin.sym
reas pok3r/v117                 firmware_v117.bin           pok3r_v117_ref.s            0x2c00  symbols_v117.sym

reas pok3r_rgb/builtin_rgb      firmware_builtin_rgb.bin    firmware_builtin_rgb_ref.s  0x0     symbols_builtin_rgb.sym
reas pok3r_rgb/v140             pok3r_rgb_v140.bin          pok3r_rgb_v140_ref.s        0x3400  pok3r_rgb_v140.sym

reas pok3r_rgb2/bootloader      rgb2_bootloader.bin         rgb2_bootloader_ref.s       0x0     rgb2_bootloader.sym
reas pok3r_rgb2/v105            rgb2_v105.bin               rgb2_v105_ref.s             0x3400  rgb2_v105.sym

reas core/builtin_core          firmware_builtin_core.bin   vortex_core_builtin_ref.s   0x0     symbols_builtin_core.sym
reas core/v145                  core_v145.bin               core_v145_ref.s             0x3400  core_v145.sym

reas race/bootloader            race_bootloader.bin         race_bootloader_ref.s       0x0     race_bootloader.sym
reas race/v124                  race_v124.bin               race_v124_ref.s             0x3400  race_v124.sym

reas vibe/v113                  vibe_v113.bin               vibe_v113_ref.s             0x3400  vibe_v113.sym

reas cypher/v136                cypher_v136.bin             cypher_v136_ref.s           0x3200  cypher_v136.sym

reas md200/bootloader           md200_bootloader.bin        md200_bootloader_ref.s      0x0     md200_bootloader.sym
reas md200/v112                 md200_v112.bin              md200_v112_ref.s            0x3400  md200_v112.sym

reas md600/v148                 md600_v148.bin              md600_v148_ref.s            0x3400  md600_v148.sym

popd > /dev/null
