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

#include "SpeedPlanner.h"
#include "StepperController.h"
#include "MovementExecuter.h"

#include "../interface.h"
#include "../Actions/ContinuousActions.h"
#include "../Core/EEPROMStorage.h"
#include "TrajectoryExecuter.h"
#include "SpeedManager.h"
#include "motion_data.h"


void SpeedPlanner::begin() {

#define CARTESIAN_GROUP(i, s0, s1, s2, ms)\
    theorical_regulation_speeds[i]= ms;\
    speed_groups_signatures[i] = ((s0!=-1) ? 1<<s0 : 0)|((s1!=-1) ? 1<<s1: 0)|((s2!=-1) ? 1<<s2 : 0);


#include "../config.h"

#undef CARTESIAN_GROUP

    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        axis_signatures[axis] = (sig_t) (1 << axis);
    }

}

void SpeedPlanner::send_position() {

#ifdef position_log
    StepperController::send_position();
#else
    CI::echo("POSITION");
    float t[NB_STEPPERS];
    for (int axis = 0; axis < NB_STEPPERS; axis++) {
        t[axis] = (SpeedPlanner::positions[axis] / EEPROMStorage::steps[axis]);
    }
    CI::send_position(t);
#endif

}

void SpeedPlanner::set_speed_group(uint8_t speed_group) {
    SpeedPlanner::speed_group = speed_group;
}


//TODO CHECK MAX_SPEED
void SpeedPlanner::set_speed_for_group(uint8_t group_id, float speed) {
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
 *
 */

//TODO PROCESS INV_SQUARE_DIST_SUM HERE, WHERE SPEED_GROUPS SIGNATURES ARE KNOWN
float SpeedPlanner::get_adjusted_regulation_speed_linear(float *const relative_distsmm, const float sqrt_square_dist_sum) {

    float regulation_speed = get_regulation_speed(relative_distsmm, sqrt_square_dist_sum);

    regulation_speed = verifiy_speed_limits(relative_distsmm, sqrt_square_dist_sum, regulation_speed);

    regulation_speed = verify_acceleration_limits(regulation_speed);//relative_distsmm, sqrt_square_dist_sum, regulation_speed);

    return regulation_speed;

}

float SpeedPlanner::get_regulation_speed(float *const relative_distsmm, const float sqrt_square_dist_sum) {
    //Determination of the regulation speed
    sig_t group_signature = speed_groups_signatures[speed_group];
    float group_coeff = 0;
    float group_speed = theorical_regulation_speeds[speed_group];

#define STEPPER(i, sig, ...)\
    if (group_signature&sig) {\
        group_coeff+=relative_distsmm[i]*relative_distsmm[i];\
    }

#include "../config.h"

#undef STEPPER

    if (group_coeff == 0) {//If no axis in the speed group has to enqueue_movement : find the first speed group with a moving axis.
        CI::echo("ERROR : THE SELECTED SPEED_GROUP DOESN'T COVER YOUR MOVEMENT : SELECTING ANOTHER");
        for (uint8_t group = 0; group < NB_CARTESIAN_GROUPS; group++) {
            if (group == speed_group) continue;
            group_signature = speed_groups_signatures[group];
            group_coeff = 0;

#define STEPPER(i, sig, ...)\
            if (group_signature&sig) {\
                group_coeff+=relative_distsmm[i]*relative_distsmm[i];\
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


float
SpeedPlanner::verifiy_speed_limits(float *const relative_distsmm, const float sqrt_square_dist_sum, float regulation_speed) {

    /*
     * Formula :
     *
     * The speed projected on a particular axis, which will enqueue_movement of [di] mm, is given by
     *      Vi = theorical_regulation_speed*di/sqrt_square_dist_sum;
     */

    float scoeff = regulation_speed / sqrt_square_dist_sum;

    float r = 0;
    bool init = true;
    float speed, tms;

    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        //Maximum speed checking
        float dist = *(relative_distsmm + axis);
        if ((speed = (abs(dist)) * scoeff) > (tms = EEPROMStorage::maximum_speeds[axis])) {//TODO RESTORE AXIS_t
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

float SpeedPlanner::verify_acceleration_limits(float regulation_speed) {//TODO


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
 *  distsmm contains distances each axis will enqueue_movement.
 *
 *  sqrt_square_dist_sum is the square root of the sum of square distances.
 *
 *  It prepares the speed change, before the currently planned movement is really executed.
 *
 *  -----------------------
 *
 *  It first starts by checking the jerk limits on each axis. If the jerk of one axis exceeds its limit,
 *      then it plans a deceleration point.
 *
 *
 *  After this, it adds the current motion distances to the future jerk point distances
 *
 *
 *  It then sets three parameters :
 *      - ratio : the distance ratio, used to adapt the speed management to one axis, in linear moves (1 for others);
 *      - tmp_delay_numerator : the numerator used during the delay computing : given d the speed distance square
 *              root, the current delay is given by tmp_delay_numerator / d;
 *      - tmp_speed_numerator : the speed factor : given  d the speed distance square root, the current speed in mm/s
 *              is given by d * tmp_speed_numerator
 *      - tmp_regulation_delay : the regulation delay that will be used during the movement we now plan.
 *
 */
void SpeedPlanner::pre_set_speed_axis(uint8_t new_axis, float *relative_distsmm, float sqrt_square_dist_sum,
                                      int32_t *movement_distances, float regulation_speed, uint8_t processing_steps) {

    float distance_coefficient = abs(relative_distsmm[new_axis]) / sqrt_square_dist_sum;

    //the acceleration considered is the new axis acceleration
    float acceleration = EEPROMStorage::accelerations[new_axis];
    float steps = EEPROMStorage::steps[new_axis];

    //Verify the jerk limits for the current movement and add a jerk point before the current movement if necessary.
    verify_jerk_limits(relative_distsmm, sqrt_square_dist_sum, regulation_speed);

    //Add the future movement distances to the current jerk distances
    add_jerk_distances(movement_distances);

    last_regulation_speed = regulation_speed;

    float ratio = steps / distance_coefficient;

    float tmp_speed_factor = (float) (sqrt(2.0 * acceleration / steps) / distance_coefficient);

    delay_t tmp_delay_numerator = (delay_t) (1000000 / sqrt(2 * steps * acceleration));//TODO DELAY
    delay_t tmp_regulation_delay = (delay_t) (1000000 / (steps * distance_coefficient * regulation_speed));

    //Determine the distance offset for the last move, so with the last acceleration and steps
    uint32_t tmp_jerk_distance_offset = 0;

    last_steps = steps, last_acceleration = acceleration;

    TrajectoryExecuter::fill_speed_data(tmp_delay_numerator, tmp_regulation_delay, tmp_speed_factor, ratio, processing_steps);

}


void SpeedPlanner::add_jerk_distances(int32_t *movement_distances) {
    //push all distances, used to update the jerk point in real time
    for (unsigned char axis = 0; axis < NB_STEPPERS; axis++) {
        next_jerk_distances[axis] += movement_distances[axis];


    }
}


/*
 * verify_jerk_limits : this function check if the jerk caused by the new move caracterised by the distances array
 *      does not exceeds limits on each axis.
 *
 * If the jerk on one axis exceeds the limit, it calculates the maximum speed that does not cause jerk excess,
 *      and plans a deceleration before the planned enqueue_movement.
 *
 */


bool SpeedPlanner::verify_jerk_limits(float *relative_distsmm, float sqrt_square_dist_sum, const float regulation_speed) {

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
     *      d2[i] is the distance that will be traveled by the axis i during the future enqueue_movement
     *
     *      D1 = sqrt(sum_k(d1[k]^2)), the euclidean norm of d1
     *      D2 = sqrt(sum_k(d2[k]^2)), the euclidean norm of d2
     *
     */


    if (last_regulation_speed != 0) {


        float jerk_speed = regulation_speed;

        for (unsigned char axis = 0; axis < NB_STEPPERS; axis++) {

            //Calculate the jerk ratio
            float jerk_ratio = relative_distsmm[axis] / sqrt_square_dist_sum;

            //Calculate the maximum speed
            float v = EEPROMStorage::jerks[axis] / (jerk_ratio - dist_ratios[axis]);
            v = abs(v);

            //Minimise the speed
            if (v < jerk_speed) {
                jerk_speed = v;
            }

            //Update dist_ratios
            dist_ratios[axis] = jerk_ratio;
        }

        //If a jerk bound is reached, add a jerk point.
        if (jerk_speed != regulation_speed) {

            uint32_t tmp_jerk_distance_offset = (uint32_t) (last_steps * jerk_speed * jerk_speed / (2 * last_acceleration));

            //Enqueue the jerk point data
            push_jerk_point();


            disable_stepper_interrupt();



            if ((TrajectoryExecuter::in_motion)&&(!SpeedManager::watch_for_jerk_point)) {

                //If the current move is made without jerk point : notify SpeedManager to watch for jerk points
                pull_jerk_point();
                SpeedManager::speed_offset = tmp_jerk_distance_offset;

            } else {

                //If not, simply mark that the previously planned movement will end with a jerk point
                motion_data * ptr = TrajectoryExecuter::peak_last_motion_data();
                ptr->jerk_distance_offset = tmp_jerk_distance_offset;
                ptr->jerk_point = true;

            }


            enable_stepper_interrupt();

            return true;

        }

        return false;

    } else {

        //return false;


        for (unsigned char axis = 0; axis < NB_STEPPERS; axis++) {
            //Calculate the jerk ratio
            dist_ratios[axis] = relative_distsmm[axis] / sqrt_square_dist_sum;
        }

        return false;
    }

}


/*
 * push_jerk_point : pushes a jerk point.
 *
 *      It adds the movement distance coordinates
 */
void SpeedPlanner::push_jerk_point() {

    //Push the distance  (the queue is only here to manage pull and push indices on the array)
    unsigned char push_id = speed_distances_offsets.push_indice();
    speed_distances_offsets.push(0);

    //push all distances, used to update the jerk point in real time
    for (unsigned char axis = 0; axis < NB_STEPPERS; axis++) {
        enqueued_end_distances[push_id * NB_STEPPERS + axis] = next_jerk_distances[axis];
        next_jerk_distances[axis] = 0;
    }

}

/*
 * pull_jerk_point : this function updates SpeedManager's jerk_distance and speed_offset,
 *
 *      according to previously enqueued data.
 *
 *      It is called by SpeedManager, before beginning a movement, when
 */

void SpeedPlanner::pull_jerk_point() {

    unsigned char pull_indice = speed_distances_offsets.pull_indice();

    speed_distances_offsets.pull();

    disable_stepper_interrupt();

    for (unsigned char axis = 0; axis < NB_STEPPERS; axis++) {
        SpeedManager::jerk_distances[axis] += enqueued_end_distances[pull_indice * NB_STEPPERS + axis];
    }
    SpeedManager::watch_for_jerk_point = true;

    enable_stepper_interrupt();


}

bool SpeedPlanner::must_watch_for_jerk_points() {
    return speed_distances_offsets.available_elements();
}

#undef LOCAL_STEP_AND_WAIT

#define m SpeedPlanner

//POWER AND SPEED

float m::last_steps = 0, m::last_acceleration = 0;

float drt[NB_STEPPERS];
float *m::dist_ratios = drt;

Queue<uint8_t> m::speed_distances_offsets(MOTION_DATA_QUEUE_SIZE);

float m::last_regulation_speed = 0;

int32_t teed[MOTION_DATA_QUEUE_SIZE * NB_STEPPERS];
int32_t *const m::enqueued_end_distances = teed;

uint8_t m::speed_group = 0;

float ttrs[NB_CARTESIAN_GROUPS];
float *const m::theorical_regulation_speeds = ttrs;

sig_t ttgs[NB_CARTESIAN_GROUPS];
sig_t *const m::speed_groups_signatures = ttgs;

int32_t pos[NB_STEPPERS];
int32_t *const m::positions = pos;

int32_t tnjd[NB_STEPPERS]{0};
int32_t *const m::next_jerk_distances = tnjd;


#define STEPPER(i, j, si, st, sp, ac, dp, ps, pd, pinPower, pmi, vi, pma, va) 1<<i,

sig_t tmp_signature[NB_STEPPERS];
sig_t *const m::axis_signatures = tmp_signature;

//Fields for theorical_regulation_speed processing :

uint8_t m::linear_tools_nb;

void (*function_array[NB_CONTINUOUS])(float);

void (**m::linear_set_functions)(float) = function_array;


#endif