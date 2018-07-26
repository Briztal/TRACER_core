/*
  machine_controller.c Part of TRACER

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


#include "machine_controller.h"

#include <kernel/kernel.h>

//--------------------------------------------------- Private headers --------------------------------------------------

//Merge the kinematic constraint's duration interval with the current limit interval;
bool mcontroller_correct_duration_window(time_interval_t *final_interval, time_interval_t optional_interval);


//---------------------------------------------- Initialisation - deletion ---------------------------------------------

/*
 * machine_controller_create : allocates and initialises a machine controller, with all provided data;
 *
 * 	Arrays are duplicated, so they don't need to be allocated in the heap;
 */

machine_controller_t *machine_controller_create(
	uint8_t nb_distances_computers, const distances_computer_t *const distances_computer_c,
	uint8_t nb_builder_computations, const controller_builder_computation_t *const builder_computations_c,
	uint8_t nb_kinematic_constraints, const kinematic_constraint_t *const kinematic_constraints_c,
	uint8_t nb_state_computations, const controller_state_computation_t *const state_computations_c,
	void *(*const controller_builder_creator)(), void (*const controller_builder_deleter)(void *),
	void *(*const controller_state_creator)(), void (*const controller_state_deleter)(void *)
) {

	//Duplicate the distance computers array;
	const distances_computer_t *const distances_computer =
		kernel_malloc_copy((size_t) nb_distances_computers * sizeof(distances_computer_t),
						   distances_computer_c);


	//Duplicate the distance computers array;
	const controller_builder_computation_t *const builder_computations =
		kernel_malloc_copy((size_t) nb_builder_computations * sizeof(controller_builder_computation_t),
						   builder_computations_c);


	//Duplicate the distance computers array;
	const kinematic_constraint_t *const kinematic_constraints =
		kernel_malloc_copy((size_t) nb_kinematic_constraints * sizeof(kinematic_constraint_t),
						   kinematic_constraints_c);

	//Duplicate the distance computers array;
	const controller_state_computation_t *const state_computations =
		kernel_malloc_copy((size_t) nb_state_computations * sizeof(controller_state_computation_t),
						   state_computations_c);


	//Create an initializer for the machine controller;
	machine_controller_t init = {

		//Save the builder creation function;
		.controller_builder_creator = controller_builder_creator,

		//Save the builder deleter function;
		.controller_builder_deleter = controller_builder_deleter,

		//Save the state creation function;
		.controller_state_creator = controller_state_creator,

		//Save the builder deleter function;
		.controller_state_deleter = controller_state_deleter,


		//Save the number of distances computers;
		.nb_distances_computers = nb_distances_computers,

		//Set the first distance computer active;
		.current_distance_computer = 0,

		//Transfer the ownership of the duplicated distance computers array;
		.distances_computers = distances_computer,


		//Save the number of builder computations;
		.nb_builder_computations = nb_builder_computations,

		//Transfer the ownership of the duplicated builder computations array;
		.builder_computations = builder_computations,


		//Save the number of kinematic constraints;
		.nb_kinematic_constraints = nb_kinematic_constraints,

		//Transfer the ownership of the duplicated kinematic constraints array;
		.kinematic_constraints = kinematic_constraints,


		//Save the number of controller state computations;
		.nb_state_computations = nb_state_computations,

		//Transfer the ownership of the duplicated state computations array;
		.state_computations = state_computations,

	};

	//Allocate and initialise the machine controller;
	machine_controller_t *controller = kernel_malloc_copy(sizeof(machine_controller_t), &init);

	//Return the initialised controller;
	return controller;

}


/*
 * machine_controller_delete : deletes each distance computer, then frees the quadruplet of owned arrays,
 * and free the controller structure;
 */

void machine_controller_delete(machine_controller_t *const machine_controller) {

	//Cache the number of distances computer;
	const uint8_t nb_distances_computers = machine_controller->nb_distances_computers;

	//Cache the distances computers array;
	const distances_computer_t *const distances_computers = machine_controller->distances_computers;

	//For each distance computer :
	for (uint8_t distance_computer_id = 0; distance_computer_id < nb_distances_computers; distance_computer_id++) {

		//Cache the distance computer;
		const distances_computer_t *const distances_computer = distances_computers + distance_computer_id;

		//Delete the distance computer;
		(*(distances_computer->delete))(distances_computer->instance);

	}

	//Free the distances computers array;
	kernel_free((void *) machine_controller->distances_computers);

	//Free the builder computations array;
	kernel_free((void *) machine_controller->builder_computations);

	//Free the kinematic constraints array;
	kernel_free((void *) machine_controller->kinematic_constraints);

	//Free the state computations array;
	kernel_free((void *) machine_controller->state_computations);

	//Free the controller structure;
	kernel_free(machine_controller);

}


//-------------------------------------------------- Structs creation --------------------------------------------------

//Wrapper for calling the state creation function;
void *machine_controller_create_state(machine_controller_t *controller) {
	return (*(controller->controller_state_creator))();
}

//Wrapper for calling the state deletion function;
void machine_controller_delete_state(machine_controller_t *controller, void *state){
	(*(controller->controller_state_deleter))(state);
}


//Wrapper for calling the builder creation function;
void *machine_controller_create_builder(machine_controller_t *controller) {
	return (*(controller->controller_builder_creator))();
}

//Wrapper for calling the builder deletion function;
void machine_controller_delete_builder(machine_controller_t *controller, void *builder){
	(*(controller->controller_builder_deleter))(builder);
}



//--------------------------------------------------- Implementation ---------------------------------------------------

/*
 * mcontroller_update_builder : updates the builder, by executing each enabled builder computations;
 */

void mcontroller_update_builder(const machine_controller_t *const controller, const builder_computation_args *const args) {

	//Cache the computations array;
	const controller_builder_computation_t *const computations = controller->builder_computations;

	//Cache the number of computations;
	const uint8_t nb_computations = controller->nb_builder_computations;

	//For each computation to realise :
	for (uint8_t computation_index = 0; computation_index < nb_computations; computation_index++) {

		//Cache the computation pointer;
		const controller_builder_computation_t *const computation = computations + computation_index;

		//If the computation is enabled :
		if (computation->enabled) {

			//Execute the computation;
			(*(computation->computation_function))(args);

		}

	}

}


/*
 * mcontroller_determine_duration : determines the current movement's final duration window, evaluating all
 * constraints successively;
 */

void mcontroller_determine_duration(const machine_controller_t *const controller, const duration_computation_args *args) {

	//Create a cached copy of the provided time interval;
	time_interval_t final_interval = *(args->duration_window);

	//If the final interval is invalid :
	if (!final_interval.valid) {

		//Error, not supposed to happen;
		kernel_error("machine_controller.c : mcontroller_correct_duration_window : the final interval is invalid;");

	}

	//If the time interval is already a singleton :
	if ((final_interval.max_exists) && (final_interval.min >= final_interval.max)) {

		//Nothing to do;
		return;

	}

	//Create a time interval that we will give to kinematic constraints; They will initialise it;
	time_interval_t constraint_interval = {};

	//Create a new duration computation struct that we pass to kinematic constraints functions;
	const duration_computation_args functions_args = {

		//Cache provided state and builder;
		.current_state = args->current_state,
		.builder = args->builder,

		//Provide out temp time duration;
		.duration_window = &constraint_interval,

	};


	//Cache the constraints array;
	const kinematic_constraint_t *const constraints = controller->kinematic_constraints;

	//Cache the number of constraints;
	const uint8_t nb_constraints = controller->nb_kinematic_constraints;

	//For each computation to realise :
	for (uint8_t constraint_index = 0; constraint_index < nb_constraints; constraint_index++) {

		//Cache the constraint;
		const kinematic_constraint_t *const constraint = constraints + constraint_index;

		//If the constraint is enabled :
		if (constraint->enabled) {

			//Determine the constraint's required duration window; constraint_interval will be re-initialised;
			(*(constraint->constraint_function))(&functions_args);

			//The constraint updates the provided interval to a non empty valid one;

			//Correct the duration window according to the new restriction;
			const bool singleton = mcontroller_correct_duration_window(&final_interval, constraint_interval);

			//If the duration window is now a singleton :
			if (singleton) {

				//no more constraints to evaluate;
				break;

			}

		}

	}

	//Now that the duration window is reduced to its minimal width, save it;
	*(args->duration_window) = final_interval;

}


/*
 * This function will correct the final duration window, including information from the provided optional interval;
 *
 * 	The corrected final interval will be a portion of the provided one; If both interval do not intersect,
 * 	the appropriate bound is chosen, as the unique value of the final interval, that becomes a singleton;
 */

bool mcontroller_correct_duration_window(time_interval_t *const final_interval,
										 const time_interval_t optional_interval) {

	//If the constraint interval is invalid :
	if (!optional_interval.valid) {

		//Error, the constraint must provide a valid interval;
		kernel_error("machine_controller.c : mcontroller_correct_duration_window : the constraint "
						 "interval is invalid;");

	}

	//If the provided interval is empty :
	if (optional_interval.empty) {

		//Nothing to do; As this function is executed, the final interval is not a singleton;
		return false;

	}

	//Cache all required data. Min var is mutable, as is susceptible to be changed in the first step;
	float min = final_interval->min;
	const bool max_exists = final_interval->max_exists;
	const float max = final_interval->max;

	//We can correct the interval's minimum bound;

	//If the minimal bound must be corrected :
	if (min < optional_interval.min ) {

		//If the final interval's maximum bound exists, and is lower than the optional interval's minimum :
		if (max_exists && (max <= optional_interval.min)) {

			//The final interval becomes a singleton containing max;
			final_interval->min = max;
			return true;

		}

		//If not, just correct the minimal bound and its cached version;
		final_interval->min = min = optional_interval.min;

	}


	//If the optional maximum exists, we can correct the final interval's max bound:
	if (optional_interval.max_exists) {

		//If it is smaller that the minimum :
		if (optional_interval.max <= min) {

			//The final interval becomes a singleton containing min;
			final_interval->max_exists = true;
			final_interval->max = min;
			return true;

		}

		//If the final interval's max doesn't exist, or if it is greater that the optional interval's one :
		if ((!max_exists) || (optional_interval.max < max)) {

			//Update the maximal bound, its flag;
			final_interval->max_exists = true;
			final_interval->max = optional_interval.max;

		}

	}


	//The singleton cases have been evaluated. The final interval is not a singleton;
	return false;


}


/*
 * mcontroller_compute_state : computes the controller's part of the state, by executing each enabled computation;
 */

void mcontroller_compute_state(const machine_controller_t *const controller, const state_computation_args *const args) {

	//Cache the computations array;
	const controller_state_computation_t *const computations = controller->state_computations;

	//Cache the number of computations;
	const uint8_t nb_computations = controller->nb_state_computations;

	//For each computation to realise :
	for (uint8_t computation_index = 0; computation_index < nb_computations; computation_index++) {

		//Cache the computation pointer;
		const controller_state_computation_t *const computation = computations + computation_index;

		//If the computation is enabled :
		if (computation->enabled) {

			//Execute the computation;
			(*(computation->computation_function))(args);

		}

	}

}