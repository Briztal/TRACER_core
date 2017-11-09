/*
  PID.cpp - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "PID.h"

#define PID(i, name, kp, ki, kd)\
/*compute_ function : computes the output for a particular error*/\
float PID::compute_##name(float error) {\
    float s = (sums[i]+=error), l = lasts[i];\
    float ret = kp * error + ki * s + kp * (error - l);\
    lasts[i] = error;\
    return ret;\
}\
/*reset function : resets the sum and last to zero*/\
void PID::reset_##name() {\
    sums[i] = 0;\
    lasts[i] = 0;\
}

#include <config.h>

#undef PID

//Sums
float t_pid_sums[NB_PIDS];
float *const PID::sums = t_pid_sums;

//Lasts
float t_pid_lasts[NB_PIDS];
float *const PID::lasts = t_pid_lasts;