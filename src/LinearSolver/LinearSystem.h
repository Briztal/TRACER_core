/*
  LinearSystem.h - Part of TRACER

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

#ifndef TRACER_LINEARSOLVER_H
#define TRACER_LINEARSOLVER_H

#include "CoefficientArray.h"

#include "Matrix.h"

class LinearSystem {

    //----------------------------------------- Initialisation -----------------------------------------

public:

    //Constructor;
    LinearSystem(uint8_t nbInputs, uint8_t nbOutputs);

    //Destructor;
    ~LinearSystem();


    //----------------------------------------- Equations add -----------------------------------------

    //Add a simple equation, where a single variable appears in the right member (with coefficient 1);
    void addSimpleEquation(uint8_t left_index, float *right_member, uint8_t right_member_size);

    //Add an equation, with no particular constraints on neither right of left members;
    void addEquation(const float *left_member, const uint8_t left_member_size, const float *right_member,
                     const uint8_t right_member_size);


    //----------------------------------------- Fields -----------------------------------------

private:

    //The number of input coordinates, ie, the size of left members;
    const uint8_t nbInputs;

    //The number of output coordinates, ie, the size of right members;
    const uint8_t nbOutputs;

    /*
     * The equation structure : contains two coefficients arrays, each related to a different set of variable;
     */

    struct equation_t {

        //The left member
        CoefficientArray *left_member = nullptr;

        //The right member
        CoefficientArray *right_member = nullptr;

    };

    //The current number of equations saved;
    uint8_t nbEquations;

    //The equation array;
    equation_t *equations;


    //----------------------------------------- Resolution -----------------------------------------

public:

    //Compute the resolution matrix;
    Matrix *solveSystem();

    //Extract the matrix related to the right members system;
    const Matrix *extractMatrix() const;


private:

    //Solve the left members system;
    uint8_t solveLeftMembersSystem();

    //Find an equation that has a non null coefficient, and switch it with the current position;
    float findAndSwitch(uint8_t equation_index, const uint8_t nb_equations);


    //Sum the last columns of the matrix;
    void mergePseudoCoordinated(const uint8_t nb_parts, Matrix *matrix);


    //----------------------------------------- Display -----------------------------------------

public:

    //Display the content of equations;
    void display();

};



#endif //TRACER_LINEARSOLVER_H
