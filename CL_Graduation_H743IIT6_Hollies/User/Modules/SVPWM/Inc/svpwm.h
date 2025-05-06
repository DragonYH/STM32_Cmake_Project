#ifndef __SVPWM_H
#define __SVPWM_H

#include "three_phase_rectifier.h"
#include "user_global.h"

#define TIM_PERIOD 5999

void svpwm_Control(float Ualpha, float Ubeta, float Ts);

#endif
