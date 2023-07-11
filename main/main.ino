#include "BLEComboParser.hpp"
#include "DebugPrint.h"
#include "MsgPipe.hpp"
#include "PS2Mouse.hpp"
#include <Arduino.h>
#include <PS2KeyRaw.h>

const TickType_t delay_tick = 10;

constexpr uint8_t CORE_TASK_BLE{0};

static PS2Mouse mouse(16, 15, STREAM);
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
		if (hid_msg_pipe.pop(&msg))
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
#ifdef _DEBUG
	Serial.begin(115200);
#endif
	DEBUG_PRINTF("Start\n");
	mouse.initialize();
	keyboard.begin(17, 18);

	xTaskCreatePinnedToCore(BLETask,          // Function to implement the task
							"BLETask",        // Name of the task
							8192,             // Stack size in words
							NULL,             // Task input parameter
							2,                // Priority of the task
							&task_handle_ble, // Task handle.
							CORE_TASK_BLE);   // Core where the task should run
}

void loop(void)
{
	static int16_t data_prev[3];
	int16_t data[3];

	mouse.report(data);

	if (!(data[0] == data_prev[0] && data[1] == data_prev[1] && data[2] == data_prev[2]))
	{
		const int MOUSE_MASK_LEFT = 0x01;
		const int MOUSE_MASK_RIGHT = 0x02;
		const int MOUSE_MASK_WHEEL = 0x04;
		hidmsg_t msg;

		char left_stat = data[0] & MOUSE_MASK_LEFT ? 'o' : '-';
		char right_stat = data[0] & MOUSE_MASK_RIGHT ? 'o' : '-';
		char wheel_stat = data[0] & MOUSE_MASK_WHEEL ? 'o' : '-';
		DEBUG_PRINTF("%c %c %c | (%d, %d)\n",
					 left_stat,
					 right_stat,
					 wheel_stat,
					 data[1],
					 data[2]); // Stat, X, Y
		data_prev[0] = data[0];
		data_prev[1] = data[1];
		data_prev[2] = data[2];
	}

	if (keyboard.available())
	{
		int c = keyboard.read();
		DEBUG_PRINTF("Key: %X\n", c);
	}
}
