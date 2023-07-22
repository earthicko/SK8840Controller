#include "USBHIDMsgManager.hpp"
#include "DebugPrint.h"
#include "USBHIDKeys.h"
#include "remapKeyboard.h"

bool USBHIDMsgManager::_ispressed[USBCODE_LEN];
hidmsg_t USBHIDMsgManager::msg;

void USBHIDMsgManager::update(uint8_t keycode, bool pressed, bool is_modkey)
{
	keycode = remapKeycode(keycode, is_modkey);

	if ((_ispressed[keycode] && pressed) || (!_ispressed[keycode] && !pressed))
	{
#ifdef _DEBUG
		DEBUG_PRINTF("Already ");
		if (pressed)
			DEBUG_PRINTF("pressed ");
		else
			DEBUG_PRINTF("released ");
		DEBUG_PRINTF("%X\n", keycode);
#endif
		return;
	}

	if (pressed)
		_ispressed[keycode] = true;
	else
		_ispressed[keycode] = false;

	msg.ep = 1;
	msg.len = 8;

	if (is_modkey)
	{
		if (pressed)
			msg.buf[0] |= keycode;
		else
			msg.buf[0] &= ~keycode;
	}
	else
	{
		if (pressed)
		{
			bool inserted = false;
			for (uint8_t i = 2; i < 8; i++)
			{
				if (msg.buf[i] == 0)
				{
					msg.buf[i] = keycode;
					inserted = true;
					break;
				}
			}
			if (!inserted)
			{
				// TODO: handle too many keys
				DEBUG_PRINTF("Too many keys pressed\n");
			}
		}
		else
		{
			for (uint8_t i = 2; i < 8; i++)
			{
				if (msg.buf[i] == keycode)
				{
					for (uint8_t j = i; j < 7; j++)
						msg.buf[j] = msg.buf[j + 1];
					msg.buf[7] = 0;
					break;
				}
			}
		}
	}

	DEBUG_PRINTF("Composed ");
	for (uint8_t i = 0; i < 8; i++)
		DEBUG_PRINTF("%X ", msg.buf[i]);
	DEBUG_PRINTF("\n");
}
