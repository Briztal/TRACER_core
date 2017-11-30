/*
  config.h - Part of TRACER

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

#ifndef TRACER_INTERFACES
#define TRACER_INTERFACES

/*
 * This files simplifies the interface manipulation : it includes the correct files, and defines a macro for the main
 *  interface.
 */

#include "config.h"

#ifdef ENABLE_TERMINAL_INTERFACE
#include <Interfaces/TerminalInterface/TerminalInterface.h>
#ifdef MAIN_CI_TERMINAL
#define CI TI
#endif
#endif

#ifdef ENABLE_GCODE_INTERFACE
#include <Interfaces/GCodeInterface/GCodeInterface.h>
#ifdef MAIN_CI_GCODE
#define CI GI
#endif
#endif

#ifdef ENABLE_PROGRAM_INTERFACE
#include <Interfaces/ProgramInterface/ProgramInterface.h>
#ifdef MAIN_CI_PROGRAM
#define CI PI
#endif
#endif


void initialise_interfaces();

void read_interfaces();

#endif
