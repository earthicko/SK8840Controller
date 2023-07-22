#ifndef USBHIDMSGMANAGER_HPP
#define USBHIDMSGMANAGER_HPP

#include "MsgPipe.hpp"
#include <stdint.h>

#define USBCODE_LEN 256

class USBHIDMsgManager
{
  private:
	static bool _ispressed[USBCODE_LEN];

  public:
	static hidmsg_t msg;
	static void update(uint8_t keycode, bool pressed, bool is_modkey);
};

#endif
