#ifndef PS2CODEMANAGER_HPP
#define PS2CODEMANAGER_HPP

#include <stdint.h>

#define PS2CODE_LEN 256
#define PS2CODE_SIZE 8

class PS2CodeManager
{
  private:
	enum PS2CODE_PARSE_STATE
	{
		INIT = 0,
		EXPECT_BASIC_BREAK,
		EXPECT_EXTENDED,
		EXPECT_PAUSEBREAK_MAKE,
		EXPECT_EXTENDED_BREAK,
		EXPECT_PRTSC_MAKE,
		EXPECT_PRTSC_BREAK
	};

	static uint8_t _ps2_to_usb_basic[PS2CODE_LEN];
	static uint8_t _ps2_to_usb_extended[PS2CODE_LEN];
	static int _state;
	static uint8_t _prev_code;

	static bool parseInit(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown);
	static bool parseExtended(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown);
	static bool parsePauseBreakMake(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown);
	static bool parseExtendedBreak(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown);
	static bool parsePrtScMake(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown);
	static bool parsePrtScBreak(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown);

  public:
	enum KEYPRESS
	{
		MAKE = 0,
		BREAK
	};

	static void initialize(void);
	static bool parse(uint8_t ps2_code, uint8_t *ret_code, uint8_t *ret_updown);
};

#endif
