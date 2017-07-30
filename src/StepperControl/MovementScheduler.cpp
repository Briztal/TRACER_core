/*
  MotionScheduler.cpp - Part of TRACER

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


#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#include "MovementScheduler.h"
#include "StepperController.h"
#include "MovementExecuter.h"

#include "../interface.h"
#include "../Actions/ContinuousActions.h"
#include "../Core/EEPROMStorage.h"
#include "TrajectoryExecuter.h"


void MovementScheduler::begin() {

#define CARTESIAN_GROUP(i, s0, s1, s2, ms)\
    theorical_regulation_speeds[i]= ms;\
    speed_groups_signatures[i] = ((s0!=-1) ? 1<<s0 : 0)|((s1!=-1) ? 1<<s1: 0)|((s2!=-1) ? 1<<s2 : 0);


#include "../config.h"

#undef CARTESIAN_GROUP

    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        axis_signatures[axis] = (sig_t) (1 << axis);
    }


}


void MovementScheduler::send_position() {

#ifdef position_log
    StepperController::send_position();

#else
    CI::echo("POSITION");
    float t[NB_STEPPERS];
    for (int axis = 0; axis < NB_STEPPERS; axis++) {
        t[axis] = (MovementScheduler::positions[axis] / EEPROMStorage::steps[axis]);
    }
    CI::send_position(t);
#endif

}

void MovementScheduler::set_speed_group(uint8_t speed_group) {
    MovementScheduler::speed_group = speed_group;
}


//TODO CHECK MAX_SPEED
void MovementScheduler::set_speed_for_group(uint8_t group_id, float speed) {
    theorical_regulation_speeds[group_id] = speed;
}


/*
 * get_adjusted_regulation_speed_linear : this function calculates the real regulation speed, from the theoretical one.
 *
 *  It verifies that the speed on each axis does not overpasses the maximum speed on this axis.
 *
 *  If it does, it determines the ratio to reduce the speed, for each axis where the speed is too high.
 *
 *  It applies the maximum ratio.
 */

//TODO PROCESS INV_SQUARE_DIST_SUM HERE, WHERE SPEED_GROUPS SIGNATURES ARE KNOWN
float MovementScheduler::get_adjusted_regulation_speed_linear(float *const distsmm, const float sqrt_square_dist_sum) {

    float regulation_speed = get_regulation_speed(distsmm, sqrt_square_dist_sum);


    regulation_speed = verifiy_speed_limits(distsmm, sqrt_square_dist_sum, regulation_speed);
    CI::echo("real : " + String(regulation_speed));

    regulation_speed = verify_acceleration_limits(regulation_speed);//distsmm, sqrt_square_dist_sum, regulation_speed);

    return regulation_speed;

}

float MovementScheduler::get_regulation_speed(float *const distsmm, const float sqrt_square_dist_sum) {
    //Determination of the regulation speed
    sig_t group_signature = speed_groups_signatures[speed_group];
    float group_coeff = 0;
    float group_speed = theorical_regulation_speeds[speed_group];

#define STEPPER(i, sig, ...)\
    if (group_signature&sig) {\
        group_coeff+=distsmm[i]*distsmm[i];\
    }

#include "../config.h"

#undef STEPPER

    float theorical_regulation_speed;

    if (group_coeff == 0) {//If no axis in the speed group has to move : find the first speed group with a moving axis.
        CI::echo("ERROR : THE SELECTED SPEED_GROUP DOESN'T COVER YOUR MOVEMENT : SELECTING ANOTHER");
        for (uint8_t group = 0; group < NB_CARTESIAN_GROUPS; group++) {
            if (group == speed_group) continue;
            group_signature = speed_groups_signatures[group];
            group_coeff = 0;

#define STEPPER(i, sig, ...)\
            if (group_signature&sig) {\
                group_coeff+=distsmm[i]*distsmm[i];\
            }

#include "../config.h"

#undef STEPPER

            if (group_coeff != 0) {
                group_speed = theorical_regulation_speeds[group];
                CI::echo("SELECTED GROUP" + String(group));
                break;
            }
        }
    }

    if (group_coeff == 0) {
        CI::echo("ERROR : THERE IS NO SPEED GROUP CONTAINING ANY AXIS MOVING DURING YOUR MOVEMENT");
        //TODO ERROR
        return 0;
    }

    return (float) (group_speed * sqrt_square_dist_sum / sqrt(group_coeff));
}


float MovementScheduler::verifiy_speed_limits(float *const distsmm, const float sqrt_square_dist_sum, float regulation_speed) {

    /*
     * Formula :
     *
     * The speed projected on a particular axis, which will move of [di] mm, is given by
     *      Vi = theorical_regulation_speed*di/sqrt_square_dist_sum;
     */

    float scoeff = regulation_speed / sqrt_square_dist_sum;

    float r = 0;
    bool init = true;
    float speed, tms;

    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        //Maximum speed checking
        if ((speed = (*(distsmm + axis)) * scoeff) > (tms = EEPROMStorage::maximum_speeds[axis])) {//TODO RESTORE AXIS_t
            r = (init) ? tms / speed : min(r, tms / speed);
            init = false;
        }
    }


    if (r != 0) {
        return regulation_speed * r;
    } else {
        return regulation_speed;
    }

}

float MovementScheduler::verify_acceleration_limits(float regulation_speed) {//TODO


    /*
    float *accelerations = EEPROMStorage::accelerations;
    float maxValue = 0;
    uint8_t m_axis = 0;
    float temp_time;
    for (uint8_t axis = 0; axis < dimension; axis++) {
        //Maximum acceleration step
        //Dists are in correct order, not accelerations so axis_t must be used
        if ((temp_time = distsmm[axis] / accelerations[axis_t[axis]]) > maxValue) {
            m_axis = axis;
            maxValue = temp_time;
        }
    }

    if (m_axis != 0) {
        acceleration0 = *distsmm / maxValue;
    } else {
        acceleration0 = accelerations[axis_t[0]];
    }
    */

    return regulation_speed;
}


/*
 * pre_set_speed_axis : this function is called during the scheduling of a movement.
 *
 *  new_axis is the axis that will be used for delay regulation.
 *
 *  distsmm contains distances each axis will move.
 *
 *  sqrt_square_dist_sum is the square root of the sum of square distances.
 *
 *  It prepares the speed change, before the currently planned movement is really executed.
 *
 *
 *  It first starts by checking the jerk limits on each axis. If the jerk of one axis exceeds its limit,
 *      then it plans a deceleration point.
 *
 *  It sets three parameters :
 *      - ratio : the distance ratio, used to adapt the speed management to one axis, in linear moves (1 for others);
 *      - tmp_delay_numerator : the numerator used during the delay computing : given d the speed distance square
 *              root, the current delay is given by tmp_delay_numerator / d;
 *      - tmp_speed_numerator : the speed factor : given  d the speed distance square root, the current speed in mm/s
 *              is given by d * tmp_speed_numerator
 *      - tmp_regulation_delay : the regulation delay that will be used during the movement we now plan.
 */
void MovementScheduler::pre_set_speed_axis(uint8_t new_axis, float *distsmm, float sqrt_square_dist_sum, float regulation_speed, uint8_t processing_steps) {

    float distance_coefficient = distsmm[new_axis]/sqrt_square_dist_sum;

    //the acceleration considered is the new axis acceleration
    float acceleration = EEPROMStorage::accelerations[new_axis];
    float steps = EEPROMStorage::steps[new_axis];

    float ratio = steps / distance_coefficient;

    float tmp_speed_factor = (float) (sqrt(2.0 * acceleration / steps) / distance_coefficient);

    delay_t tmp_delay_numerator = (delay_t) (1000000 / sqrt(2 * steps * acceleration));//TODO DELAY
    delay_t tmp_regulation_delay = (delay_t) (1000000 / (steps * distance_coefficient * regulation_speed));

    CI::echo("REGULATION_DELAY : " + String(tmp_regulation_delay));

    TrajectoryExecuter::fill_speed_data(tmp_delay_numerator, tmp_regulation_delay, tmp_speed_factor, ratio, processing_steps);

}

/*
 * verify_jerk_limits : this function check if the jerk caused by the new move caracterised by the distances array
 *      does not exceeds limits on each axis.
 *
 * If the jerk on one axis exceeds the limit, it calculates the maximum speed that does not cause jerk excess,
 *      and plans a deceleration before the planned move.
 *
 */

float * dist_ratios;
void MovementScheduler::verify_jerk_limits(float *distsmm, float sqrt_square_dist_sum, - const float regulation_speed) {

    /*
     * Formula :
     *
     * The maximum (global) speed, to have the jerk on the axis i lesser than J is given by
     *
     *      V = J / abs(d1[i]/D1 - d2[i]/D2)
     *
     *      we will name the quantity d[i]/D a distance ratio.
     *
     * Where :
     *
     *      d1[i] is the distance traveled by the axis i during the last move
     *      d2[i] is the distance that will be traveled by the axis i during the future move
     *
     *      D1 = sqrt(sum_k(d1[k]^2)), the euclidian norm of d1
     *      D2 = sqrt(sum_k(d2[k]^2)), the euclidian norm of d2
     *
     */

    float min_speed = regulation_speed;
    bool corrected = false;

    for (unsigned char axis = 0; axis<NB_STEPPERS; axis++) {
        //Calculate the jerk ratio
        float jerk_ratio = distsmm[axis]/sqrt_square_dist_sum;

        //Calculate the maximum speed
        float v = EEPROMStorage::jerks[axis]/(jerk_ratio-dist_ratios[axis]);
        v = abs(v);

        //Minimise the speed
        if (v<min_speed) {
            min_speed = v;
            corrected = true;
        }

        //Update dist_ratios
        dist_ratios[axis] = jerk_ratio;
    }

    CI::echo("JERK_MIN_SPEED : "+String(min_speed));

    //If the regulation speed causes jerk excess, then plan a deceleration.
    if (corrected) {
        SpeedManager::plan_deceleration(min_speed);
    }

}

//TODO ACCELERATION CHECKING ON OTHER AXIS
//TODO CALCULER (ts *ta ) AU PREALABLE MATRCIELLEMENT

#undef LOCAL_STEP_AND_WAIT


#define m MovementScheduler


//POWER AND SPEED

uint8_t m::speed_group = 0;

float ttrs[NB_CARTESIAN_GROUPS];
float *const m::theorical_regulation_speeds = ttrs;

sig_t ttgs[NB_CARTESIAN_GROUPS];
sig_t *const m::speed_groups_signatures = ttgs;

int32_t pos[NB_STEPPERS];
int32_t *const m::positions = pos;


#define STEPPER(i, j, si, st, sp, ac, dp, ps, pd, pinPower, pmi, vi, pma, va) 1<<i,

sig_t tmp_signature[NB_STEPPERS];
sig_t *const m::axis_signatures = tmp_signature;

//Fields for theorical_regulation_speed processing :

uint8_t m::linear_tools_nb;

void (*function_array[NB_CONTINUOUS])(float);

void (**m::linear_set_functions)(float) = function_array;




/*

int32_t d0[NB_STEPPERS], d1[NB_STEPPERS];
int32_t *m::d_0 = d0, *m::d_1 = d1;
int32_t *m::end_distances = d0, *m::next_end_distances = d1;

uint8_t do0[NB_STEPPERS], do1[NB_STEPPERS];
uint8_t *m::d_o_0 = do0, *m::d_o_1 = do1;
uint8_t *m::distances_order = do0, *m::next_distances_order = do1;

bool m::is_d_0 = true;
 */



/*
 //Acceleration checking

float *accelerations = EEPROMStorage::accelerations;
float maxValue = 0;
uint8_t m_axis = 0;
float temp_time;
for (uint8_t axis = 0; axis < dimension; axis++) {
    //Maximum acceleration step
    //Dists are in correct order, not accelerations so axis_t must be used
    if ((temp_time = distsmm[axis] / accelerations[axis_t[axis]]) > maxValue) {
        m_axis = axis;
        maxValue = temp_time;
    }
}

if (m_axis != 0) {
    acceleration0 = *distsmm / maxValue;
} else {
    acceleration0 = accelerations[axis_t[0]];
}
 */

#endif