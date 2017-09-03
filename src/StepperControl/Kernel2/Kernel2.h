/*
  Kernel2.h - Part of TRACER

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

/*
 * The Kernel2 class. This class's methods are called by TrajectoryExecuter, is this version of the kernel have been
 *      chosen in your configuration file.
 *
 *
 *  This version is the most advanced, and offers the following features :
 *      - tracing of trajectories expressed in the high level coordinates. Any kind of trajectory is supported;
 *      - exact acceleration regulation in beginning and ending points, for each stepper;
 *      - real-time acceleration / deceleration regulation during movements;
 *      - exact jerk regulation for each stepper;
 *      - regulation_speed regulation by cartesian groups of the high level coordinates;
 *      - constant update of tools power, based on the current regulation_speed and their power density;
 *
 *
 *  The kernel class is in charge of :
 *      - the sub_movements pre-processing;
 *      - the sub_movements queue management
 *      - the regulation_speed and jerk regulation;
 *      - the position log;
 *
 */

#include <config.h>
#if defined(ENABLE_STEPPER_CONTROL) && (KERNEL == 2)


#ifndef TRACER_KERNEL2_H
#define TRACER_KERNEL2_H

#include <stdint.h>
#include <StepperControl/_kernels_data.h>


class Kernel2 {



public :

    static void initialise_tracing_procedure();

    //-------------------------------------------Current high level position--------------------------------------------

private :

    //The current position in the high level system.
    static float *const current_hl_position;


    //---------------------------------------------Current movement update----------------------------------------------

public:

    static void update_pre_process_speed_data(k2_movement_data *movement_data);

    static void initialise_movement_data(k2_movement_data *movement_data);

    static void compute_jerk_data(const k2_movement_data *current_movement, k2_movement_data *previous_movement);

    //------------------------------------------------environment update------------------------------------------------

public :

    static void update_real_time_jerk_environment(k2_movement_data *movement_data);

    //static void update_real_time_jerk_environment(k2_movement_data *movement_data);


    //--------------------------------------------sub_movements preparation---------------------------------------------

public:

    //The function to determine a new sub_movement
    static void initialise_sub_movement(k2_sub_movement_data *sub_movement_data);

    //The function to update the current variables according to the sub_movement data computed in the function above
    static void accept_sub_movement(k2_sub_movement_data *sub_movement);

    //The first sub_movement preparation, called at the beginning of the movement routine.
    static float compute_time_for_first_sub_movement(k2_sub_movement_data *sub_movement_data);

    //The sub_movement preparation function, called on interrupts.
    static float compute_time_for_sub_movement(k2_sub_movement_data *sub_movement_data);


private:

    //The regulation_speed group for the current movement
    static uint8_t movement_speed_group;

    //the regulation regulation_speed for the next movement
    static float next_regulation_speed;



    //----------------------------------------------------positon log---------------------------------------------------


public:

    //The function to send the current position through the interface.
    static void send_position();



};

#endif

#endif //TRACER_KERNEL2_H
