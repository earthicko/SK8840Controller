#include "BLEComboParser.hpp"
#include "DebugPrint.h"
#include "MsgPipe.hpp"
#include "PS2CodeManager.hpp"
#include "PS2Mouse.hpp"
#include "USBHIDKeys.h"
#include "USBHIDMsgManager.hpp"
#include <Arduino.h>
#include <PS2KeyRaw.h>
#include <cstring>

const int MOUSE_MASK_LEFT = 0x01;
const int MOUSE_MASK_RIGHT = 0x02;
const int MOUSE_MASK_WHEEL = 0x04;
const int MOUSE_MASK_ALL = MOUSE_MASK_LEFT | MOUSE_MASK_RIGHT | MOUSE_MASK_WHEEL;

const uint8_t PIN_MSE_DAT = 32;
const uint8_t PIN_MSE_CLK = 33;
const uint8_t PIN_KBD_DAT = 25;
const uint8_t PIN_KBD_CLK = 26;

const TickType_t delay_tick = 10;

constexpr uint8_t CORE_TASK_BLE{0};

static PS2Mouse mouse(PIN_MSE_CLK, PIN_MSE_DAT, STREAM);
static PS2KeyRaw keyboard;
static TaskHandle_t task_handle_ble;
static MsgPipe<hidmsg_t> hid_msg_pipe;
static BLEComboParser ble("IBM UltraNav", "IBM / rokart", 100);

void BLETask(void *parameter)
{
	static bool connection;
	static const parser_t parsers[] = {
		(const parser_t)(NULL),
		(const parser_t)(&BLEComboParser::parseHIDDataKeyboard),
		(const parser_t)(&BLEComboParser::parseHIDDataMouse),
	};

	DEBUG_PRINTF("BLETask start.\n");
	ble.begin();
	while (true)
	{
		if (ble.isConnected())
		{
			if (connection == false)
			{
				DEBUG_PRINTF("BLE Connected\n");
				connection = true;
			}
		}
		else
		{
			if (connection == true)
			{
				DEBUG_PRINTF("BLE Disconnected\n");
				connection = false;
			}
		}
		hidmsg_t msg;
		if (hid_msg_pipe.pop(msg))
		{
			vTaskDelay(delay_tick);
			continue;
		}
		if (!ble.isConnected())
		{
			vTaskDelay(delay_tick);
			continue;
		}
		(ble.*parsers[msg.ep])((int8_t *)msg.buf);
	}
}

void setup(void)
{
	PS2CodeManager::initialize();
#ifdef _DEBUG
	Serial.begin(115200);
#endif
	DEBUG_PRINTF("Start\n");
	mouse.initialize();
	keyboard.begin(PIN_KBD_DAT, PIN_KBD_CLK);

	xTaskCreatePinnedToCore(BLETask,          // Function to implement the task
							"BLETask",        // Name of the task
							8192,             // Stack size in words
							NULL,             // Task input parameter
							2,                // Priority of the task
							&task_handle_ble, // Task handle.
							CORE_TASK_BLE);   // Core where the task should run
}

void handleMouse(void)
{
	static int16_t data_prev[3];
	int16_t data[3];

	mouse.report(data);
	data[0] &= MOUSE_MASK_ALL;

	if (!(data[0] == data_prev[0] && data[1] == data_prev[1] && data[2] == data_prev[2]))
	{
		if (data[0] == data_prev[0] && data[1] == 0 && data[2] == 0)
			return;

		hidmsg_t msg;

#ifdef _DEBUG
		char left_stat = data[0] & MOUSE_MASK_LEFT ? 'o' : '-';
		char right_stat = data[0] & MOUSE_MASK_RIGHT ? 'o' : '-';
		char wheel_stat = data[0] & MOUSE_MASK_WHEEL ? 'o' : '-';
		DEBUG_PRINTF("%c %c %c | (%d, %d)\n",
					 left_stat,
					 right_stat,
					 wheel_stat,
					 data[1],
					 data[2]); // Stat, X, Y
		DEBUG_PRINTF("data = {%d, %d, %d}\n", data[0], data[1], data[2]);
#endif

		msg.ep = 2;
		msg.len = 5;
		msg.buf[0] = 1;
		msg.buf[1] = (uint8_t)(data[0]);
		msg.buf[2] = (uint8_t)(data[1]);
		msg.buf[3] = -(uint8_t)(data[2]);
		msg.buf[4] = 0;

		hid_msg_pipe.push(msg);

		memcpy(data_prev, data, sizeof(data_prev));
	}
}

void handleKeyboard(void)
{
	if (keyboard.available() == 0)
		return;

	uint8_t c = keyboard.read();
	DEBUG_PRINTF("Received keycode %X\n", c);

	usb_hid_event_t usb_hid_event;

	if (PS2CodeManager::parse(c, usb_hid_event))
	{
		bool pressed;

		DEBUG_PRINTF("Parsed USB keycode %X, ", usb_hid_event.code);
		switch (usb_hid_event.updown)
		{
		case PS2CodeManager::BREAK:
			DEBUG_PRINTF("BREAK\n");
			pressed = false;
			break;
		case PS2CodeManager::MAKE:
			DEBUG_PRINTF("MAKE\n");
			pressed = true;
			break;
		}

		USBHIDMsgManager::update(usb_hid_event.code, pressed, usb_hid_event.is_modkey);

		hid_msg_pipe.push(USBHIDMsgManager::msg);
	}
}

void loop(void)
{
	handleMouse();
	handleKeyboard();
}
