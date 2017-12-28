/*
  Project.h - Part of TRACER

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

#ifndef TRACER_PROJECT_H
#define TRACER_PROJECT_H


#include "MachineController.h"
#include "TemperatureController.h"

class Project {

    //--------------------------- Data Initialisation ---------------------------

public:

    //Initialise the class in a safe state;
    static inline void initialise_data() {

        //Initialise the machine controller;
        MachineController::initialise_data();

        //Initialise the temperature controller;
        TemperatureController::initialise_data();

    }


};


#endif //TRACER_PROJECT_H
