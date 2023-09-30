/*
 * Copyright (c) 2018 Charlie Waters
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vortex.h"

const uint16_t keymaps_default[][MATRIX_ROWS][MATRIX_COLS] = {
#if defined(KEYMAP_60_ANSI)
    [0] = LAYOUT_60_ansi(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        KC_LCTL, KC_LGUI, KC_LALT, KC_SPC,  KC_RALT, MO(1),   KC_RGUI, KC_RCTL
    ),
    [1] = LAYOUT_60_ansi(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,
        _______, _______, _______, _______, _______, _______, _______, KC_PGUP, KC_UP,   KC_PGDN, KC_PSCR, KC_SLCK, KC_PAUS, _______,
        _______, _______, _______, _______, _______, _______, KC_HOME, KC_LEFT, KC_DOWN, KC_RIGHT,KC_INS,  _______, _______,
        _______, KC_MPRV, KC_MPLY, KC_MNXT, KC_VOLD, KC_VOLU, KC_END,  KC_MUTE, BL_DEC,  BL_TOGG, BL_INC,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______
    ),
#elif defined(KEYMAP_VORTEX_CORE_0)
    [0] = LAYOUT_core(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,  KC_DEL,
        CTL_T(KC_ESC),KC_A,KC_S,   KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_ENT,
        KC_LSPO, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_RSPC, KC_NO,
        KC_LCTL, KC_LALT, KC_LGUI, LT(2,KC_CAPS),LT(1,KC_SPC),KC_SPC,  MO(1),   KC_RALT, KC_APP,  KC_RCTL
    ),
    [1] = LAYOUT_core(
        KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LBRC, KC_RBRC, KC_MINS, KC_EQL,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_QUOT,
        KC_LSFT,LCMD(KC_Z),LCMD(KC_X),LCMD(KC_C),LCMD(KC_V),KC_NO,KC_NO, KC_NO, KC_SLSH, KC_BSLS, KC_RSFT, KC_NO,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
    [2] = LAYOUT_core(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F10,  KC_F11,  KC_F12,
        KC_NO, LCMD(KC_LALT), LCMD(KC_LCTL), LALT(KC_LCTL), KC_F, KC_NO, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_NO, KC_NO,
        KC_BTN3, KC_ACL0, KC_ACL1, KC_ACL2, KC_BTN1, KC_BTN2, KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R, KC_BTN4, KC_BTN5,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
#elif defined(KEYMAP_VORTEX_CORE)
    [0] = LAYOUT_core(
        KC_TAB,         KC_Q,    KC_W,          KC_E,         KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, KC_BSPC, KC_QUOT,
        CTL_T(KC_ESC),  KC_A,    KC_S,          KC_D,         KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN,  KC_ENT,
        KC_LSFT,        KC_Z,    KC_X,          KC_C,         KC_V,    KC_B,    KC_N,    KC_M, KC_COMM,  KC_DOT, KC_RSFT, KC_SLSH,
        KC_LCTL,     KC_LALT, KC_LGUI, LT(2,KC_CAPS), LT(1,KC_SPC),  KC_SPC,   MO(3), KC_RALT,   KC_NO, KC_RCTL
    ),
    [1] = LAYOUT_core(
        KC_TILD, KC_EXLM,   KC_AT, KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_MINS,  KC_EQL,
        KC_GRV,     KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_DQT,
        KC_LSFT,   KC_NO, KC_PIPE, KC_PLUS, KC_UNDS, KC_QUES,   KC_NO, KC_LBRC, KC_RBRC, KC_LCBR, KC_RCBR, KC_BSLS,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
    [2] = LAYOUT_core(
        KC_KANA,       KC_F1,         KC_F2,         KC_F3,      KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,
        KC_NO, LCMD(KC_LALT), LCMD(KC_LCTL), LALT(KC_LCTL),       KC_F,   KC_NO, KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT,   KC_NO, KC_NO,
        KC_NO,    LCMD(KC_Z),    LCMD(KC_X),    LCMD(KC_C), LCMD(KC_V),   KC_NO, KC_HOME, KC_PGDN, KC_PGUP,  KC_END,   KC_NO, KC_NO,
        _______,     _______,       _______,       _______,    _______, _______, _______, _______, _______, _______
    ),
    [3] = LAYOUT_core(
        KC_BTN3, KC_BTN1, KC_MS_U, KC_BTN2, KC_WH_U, _______, _______, _______, _______, _______, _______, _______, _______,
        KC_BTN4, KC_MS_L, KC_MS_D, KC_MS_R, KC_WH_D, _______, _______, _______, _______, _______, _______, _______,
        KC_BTN5, _______, _______, KC_WH_L, KC_WH_R, _______, _______, _______, KC_ACL0, KC_ACL1, KC_ACL2,   KC_NO,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
#else
    #error "No Keymap!"
#endif
};
const uint16_t keymaps_default_size = sizeof(keymaps_default);
uint16_t keymaps[MAX_LAYERS][MATRIX_ROWS][MATRIX_COLS];

const uint16_t PROGMEM fn_actions[] = {

};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt) {
    return MACRO_NONE;
};


void matrix_init_user(void) {

}

void matrix_scan_user(void) {

}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return true;
}

void led_set_user(uint8_t usb_led) {

}
