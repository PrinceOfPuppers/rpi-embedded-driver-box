#ifndef NUNCHUK_DEMOS_NUNCHUK_JOYSTICK_MOUSE_H_
#define NUNCHUK_DEMOS_NUNCHUK_JOYSTICK_MOUSE_H_


// absolute distance from zero which is considered no movement (nunchuk joystick range is -1 to 1)
#define MOUSE_JOYSTICK_DEADZONE_X 0.06
#define MOUSE_JOYSTICK_DEADZONE_Y 0.03


// 0 to 1, higher is more sensitive (its a multiplier)
#define MOUSE_JOYSTICK_SENSITIVITY_X 1.0
#define MOUSE_JOYSTICK_SENSITIVITY_Y 1.0


#define MOUSE_POLLING_INTERVAL_MICROSECONDS 8333 // approx 8333 microsec = 120 HZ (typical polling rate)


void stop_nunchuk_joystick_mouse();

int start_nunchuk_joystick_mouse(char *device_file);

#endif
