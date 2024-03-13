#ifndef __BEEP_H__
#define __BEEP_H__

#define BEEP_GPIO_INDEX "45"

int beep_init();

int beep_deinit();

int beep_on();

int beep_off();

#endif