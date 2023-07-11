#ifndef PS2MOUSE_HPP
#define PS2MOUSE_HPP

#define REMOTE 1
#define STREAM 2

#include <stdint.h>

class PS2Mouse
{
  private:
	int _clock_pin;
	int _data_pin;
	int _mode;
	int _initialized;
	int _enabled;
	int _disabled;
	int8_t read_byte(void);
	int read_bit(void);
	int16_t read_movement_x(int);
	int16_t read_movement_y(int);
	void pull_high(int);
	void pull_low(int);
	void set_mode(int);

  public:
	PS2Mouse(int, int, int mode = REMOTE);
	void initialize(void);
	int clock_pin(void);
	int data_pin(void);
	int read(void);
	int16_t *report(int16_t data[]);
	void write(int);
	void enable_data_reporting(void);
	void disable_data_reporting(void);
	void set_remote_mode(void);
	void set_stream_mode(void);
	void set_resolution(int);
	void set_scaling_2_1(void);
	void set_scaling_1_1(void);
	void set_sample_rate(int);
};

#endif
