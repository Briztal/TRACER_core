/*
  GCodeInterfaceCommands.h - Part of TRACER

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

#include <config.h>

#ifdef ENABLE_GCODE_INTERFACE

#include <TaskScheduler/TaskScheduler.h>


class GCodeCommands {


    /*
     * Here is declared the method you will actually implement, that takes the index of the arguments in the
     * arguments storage.
     */

public :


#define GO_UPPER()

#define GO_LOWER(i)

#define COMMAND(i, name)\
    static task_state_t name(char *);

#define GO_LOWER_COMMAND(i, fname) COMMAND(i, fname)

#include <Project/Config/gcode_interface_config.h>

#undef GO_UPPER
#undef GO_LOWER
#undef GO_LOWER_COMMAND
#undef COMMAND


//TODO COMMENT AND COMMENT TERMINALCOMMANDS

#define PARSE_ARGUMENTS(arguments) {if (!GCodeInterface::parse_arguments(arguments)) return invalid_arguments;}

#define REQUIRE_ALL_ARGUMENTS(arg_string) {if (!GCodeInterface::verify_all_identifiers_presence(arg_string)) return invalid_arguments;}

#define REQUIRE_ONE_ARGUMENTS(arg_string) {if (!GCodeInterface::verify_one_identifiers_presence(arg_string)) return invalid_arguments;}

#define CHECK_ARGUMENT(identifier) (GCodeInterface::verify_identifier_presence(identifier))

#define GET_ARG(identifier) GCodeInterface::get_argument(identifier)

#define GET_ARG_VALUE(identifier) GCodeInterface::get_argument_value(identifier)
};


#endif //TRACER_GCODEINTERFACECOMMANDS_H