#include "PS2CodeManager.hpp"
#include "DebugPrint.h"
#include "USBHIDKeys.h"
#include <Arduino.h>
#include <cstring>

uint8_t PS2CodeManager::_ps2_to_usb_basic[PS2CODE_LEN];
uint8_t PS2CodeManager::_ps2_to_usb_extended[PS2CODE_LEN];
int PS2CodeManager::_state;
uint8_t PS2CodeManager::_prev_code;

void setCode(uint8_t array[], uint8_t value, uint8_t index)
{
	array[index] = value;
}

void PS2CodeManager::initialize(void)
{
	setCode(_ps2_to_usb_basic, KEY_MOD_LCTRL, 0x14);
	setCode(_ps2_to_usb_basic, KEY_MOD_LSHIFT, 0x12);
	setCode(_ps2_to_usb_basic, KEY_MOD_LALT, 0x11);
	setCode(_ps2_to_usb_basic, KEY_A, 0x1C);
	setCode(_ps2_to_usb_basic, KEY_B, 0x32);
	setCode(_ps2_to_usb_basic, KEY_C, 0x21);
	setCode(_ps2_to_usb_basic, KEY_D, 0x23);
	setCode(_ps2_to_usb_basic, KEY_E, 0x24);
	setCode(_ps2_to_usb_basic, KEY_F, 0x2B);
	setCode(_ps2_to_usb_basic, KEY_G, 0x34);
	setCode(_ps2_to_usb_basic, KEY_H, 0x33);
	setCode(_ps2_to_usb_basic, KEY_I, 0x43);
	setCode(_ps2_to_usb_basic, KEY_J, 0x3B);
	setCode(_ps2_to_usb_basic, KEY_K, 0x42);
	setCode(_ps2_to_usb_basic, KEY_L, 0x4B);
	setCode(_ps2_to_usb_basic, KEY_M, 0x3A);
	setCode(_ps2_to_usb_basic, KEY_N, 0x31);
	setCode(_ps2_to_usb_basic, KEY_O, 0x44);
	setCode(_ps2_to_usb_basic, KEY_P, 0x4D);
	setCode(_ps2_to_usb_basic, KEY_Q, 0x15);
	setCode(_ps2_to_usb_basic, KEY_R, 0x2D);
	setCode(_ps2_to_usb_basic, KEY_S, 0x1B);
	setCode(_ps2_to_usb_basic, KEY_T, 0x2C);
	setCode(_ps2_to_usb_basic, KEY_U, 0x3C);
	setCode(_ps2_to_usb_basic, KEY_V, 0x2A);
	setCode(_ps2_to_usb_basic, KEY_W, 0x1D);
	setCode(_ps2_to_usb_basic, KEY_X, 0x22);
	setCode(_ps2_to_usb_basic, KEY_Y, 0x35);
	setCode(_ps2_to_usb_basic, KEY_Z, 0x1A);
	setCode(_ps2_to_usb_basic, KEY_1, 0x16);
	setCode(_ps2_to_usb_basic, KEY_2, 0x1E);
	setCode(_ps2_to_usb_basic, KEY_3, 0x26);
	setCode(_ps2_to_usb_basic, KEY_MOD_RSHIFT, 0x59);
	setCode(_ps2_to_usb_basic, KEY_4, 0x25);
	setCode(_ps2_to_usb_basic, KEY_5, 0x2E);
	setCode(_ps2_to_usb_basic, KEY_6, 0x36);
	setCode(_ps2_to_usb_basic, KEY_7, 0x3D);
	setCode(_ps2_to_usb_basic, KEY_8, 0x3E);
	setCode(_ps2_to_usb_basic, KEY_9, 0x46);
	setCode(_ps2_to_usb_basic, KEY_0, 0x45);
	setCode(_ps2_to_usb_basic, KEY_ENTER, 0x5A);
	setCode(_ps2_to_usb_basic, KEY_ESC, 0x76);
	setCode(_ps2_to_usb_basic, KEY_BACKSPACE, 0x66);
	setCode(_ps2_to_usb_basic, KEY_TAB, 0x0D);
	setCode(_ps2_to_usb_basic, KEY_SPACE, 0x29);
	setCode(_ps2_to_usb_basic, KEY_MINUS, 0x4E);
	setCode(_ps2_to_usb_basic, KEY_EQUAL, 0x55);
	setCode(_ps2_to_usb_basic, KEY_LEFTBRACE, 0x54);
	setCode(_ps2_to_usb_basic, KEY_RIGHTBRACE, 0x5B);
	setCode(_ps2_to_usb_basic, KEY_BACKSLASH, 0x5D);
	setCode(_ps2_to_usb_basic, KEY_SEMICOLON, 0x4C);
	setCode(_ps2_to_usb_basic, KEY_APOSTROPHE, 0x52);
	setCode(_ps2_to_usb_basic, KEY_GRAVE, 0x0E);
	setCode(_ps2_to_usb_basic, KEY_COMMA, 0x41);
	setCode(_ps2_to_usb_basic, KEY_DOT, 0x49);
	setCode(_ps2_to_usb_basic, KEY_SLASH, 0x4A);
	setCode(_ps2_to_usb_basic, KEY_CAPSLOCK, 0x58);
	setCode(_ps2_to_usb_basic, KEY_F1, 0x05);
	setCode(_ps2_to_usb_basic, KEY_F2, 0x06);
	setCode(_ps2_to_usb_basic, KEY_F3, 0x04);
	setCode(_ps2_to_usb_basic, KEY_F4, 0x0C);
	setCode(_ps2_to_usb_basic, KEY_F5, 0x03);
	setCode(_ps2_to_usb_basic, KEY_F6, 0x0B);
	setCode(_ps2_to_usb_basic, KEY_F7, 0x83);
	setCode(_ps2_to_usb_basic, KEY_F8, 0x0A);
	setCode(_ps2_to_usb_basic, KEY_F9, 0x01);
	setCode(_ps2_to_usb_basic, KEY_F10, 0x09);
	setCode(_ps2_to_usb_basic, KEY_F11, 0x78);
	setCode(_ps2_to_usb_basic, KEY_F12, 0x07);
	setCode(_ps2_to_usb_basic, KEY_SCROLLLOCK, 0x7E);

	setCode(_ps2_to_usb_extended, KEY_INSERT, 0x70);
	setCode(_ps2_to_usb_extended, KEY_HOME, 0x6C);
	setCode(_ps2_to_usb_extended, KEY_PAGEUP, 0x7D);
	setCode(_ps2_to_usb_extended, KEY_DELETE, 0x71);
	setCode(_ps2_to_usb_extended, KEY_END, 0x69);
	setCode(_ps2_to_usb_extended, KEY_PAGEDOWN, 0x7A);
	setCode(_ps2_to_usb_extended, KEY_RIGHT, 0x74);
	setCode(_ps2_to_usb_extended, KEY_LEFT, 0x6B);
	setCode(_ps2_to_usb_extended, KEY_DOWN, 0x72);
	setCode(_ps2_to_usb_extended, KEY_UP, 0x75);
	setCode(_ps2_to_usb_extended, KEY_F13, 0x3D);
	setCode(_ps2_to_usb_extended, KEY_HANGEUL, 0x11);
	setCode(_ps2_to_usb_extended, KEY_HANJA, 0x14);
	setCode(_ps2_to_usb_extended, KEY_MEDIA_VOLUMEUP, 0x32);
	setCode(_ps2_to_usb_extended, KEY_MEDIA_VOLUMEDOWN, 0x21);
	setCode(_ps2_to_usb_extended, KEY_MEDIA_MUTE, 0x23);
}

bool PS2CodeManager::parseInit(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown)
{
	switch (ps2_code)
	{
	case 0xF0:
		_state = EXPECT_BASIC_BREAK;
		_prev_code = ps2_code;
		return (false);
	case 0xE0:
		_state = EXPECT_EXTENDED;
		_prev_code = ps2_code;
		return (false);
	case 0xE1:
		_state = EXPECT_PAUSEBREAK_MAKE;
		_prev_code = ps2_code;
		return (false);
	default:
		_state = INIT;
		*ret_code = _ps2_to_usb_basic[ps2_code];
		*ret_updown = MAKE;
		return (true);
	}
}

bool PS2CodeManager::parseExtended(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown)
{
	switch (ps2_code)
	{
	case 0xF0:
		_state = EXPECT_EXTENDED_BREAK;
		_prev_code = ps2_code;
		return (false);
	case 0x12:
		_state = EXPECT_PRTSC_MAKE;
		_prev_code = ps2_code;
		return (false);
	default:
		_state = INIT;
		*ret_code = _ps2_to_usb_extended[ps2_code];
		*ret_updown = MAKE;
		return (true);
	}
}

bool PS2CodeManager::parsePauseBreakMake(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown)
{
	// E1 14 77 E1 F0 14 E0 77
	static const uint8_t reference[8] = {0xE1, 0x14, 0x77, 0xE1, 0xF0, 0x14, 0xE0, 0x77};
	{
		DEBUG_PRINTF("Unimplemented stub of %s\n", __func__);
		_state = INIT;
		*ret_code = 0x00;
		*ret_updown = MAKE;
	}
	return (true);
}

bool PS2CodeManager::parseExtendedBreak(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown)
{
	switch (ps2_code)
	{
	case 0x7C:
		_state = EXPECT_PRTSC_BREAK;
		_prev_code = ps2_code;
		return (false);
	default:
		_state = INIT;
		*ret_code = _ps2_to_usb_extended[ps2_code];
		*ret_updown = BREAK;
		return (true);
	}
}

bool PS2CodeManager::parsePrtScMake(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown)
{
	static const uint8_t reference[4] = {0xE0, 0x12, 0xE0, 0x7C};
	{
		DEBUG_PRINTF("Unimplemented stub of %s\n", __func__);
		_state = INIT;
		*ret_code = 0x00;
		*ret_updown = MAKE;
	}
	return (true);
}

bool PS2CodeManager::parsePrtScBreak(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown)
{
	static const uint8_t reference[6] = {0xE0, 0xF0, 0x7C, 0xE0, 0xF0, 0x12};
	{
		DEBUG_PRINTF("Unimplemented stub of %s\n", __func__);
		_state = INIT;
		*ret_code = 0x00;
		*ret_updown = MAKE;
	}
	return (true);
}

bool PS2CodeManager::parse(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown)
{
	static unsigned long prev_call_time;
	static const unsigned long reset_time = 10;

	if (millis() - prev_call_time > reset_time)
	{
		DEBUG_PRINTF("PS/2 parser state reset (%d ms passed)\n", reset_time);
		_state = INIT;
	}

	prev_call_time = millis();

	switch (_state)
	{
	case INIT:
		return (parseInit(ps2_code, ret_code, ret_updown));
	case EXPECT_BASIC_BREAK:
		_state = INIT;
		*ret_code = _ps2_to_usb_basic[ps2_code];
		*ret_updown = BREAK;
		return (true);
	case EXPECT_EXTENDED:
		return (parseExtended(ps2_code, ret_code, ret_updown));
	case EXPECT_PAUSEBREAK_MAKE:
		return (parsePauseBreakMake(ps2_code, ret_code, ret_updown));
	case EXPECT_EXTENDED_BREAK:
		return (parseExtendedBreak(ps2_code, ret_code, ret_updown));
	case EXPECT_PRTSC_MAKE:
		return (parsePrtScMake(ps2_code, ret_code, ret_updown));
	case EXPECT_PRTSC_BREAK:
		return (parsePrtScBreak(ps2_code, ret_code, ret_updown));
	default:
		DEBUG_PRINTF("Error: undefined PS/2 parser state\n");
		return (false);
	}
}
