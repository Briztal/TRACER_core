/*
  TerminalInterface.h - Part of TRACER

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


#ifndef CODE_TERMINAL_INTERFACE
#define CODE_TERMINAL_INTERFACE

#include <Config/control_config.h>

#ifdef ENABLE_TERMINAL_INTERFACE

#include <Interaction/Languages/_language_data.h>
#include <Interaction/Protocols/Protocol.h>
#include "TerminalTree.h"

class Terminal {


public :

    //--------------------------------------Standard Interface functions --------------------------------------

    //The hardware initialisation function;
    static void init_message();

    //The data initialisation function;
    static void initialise_data(Protocol *protocol);


    //--------------------------------------Command Parsing----------------------------------

public:

    //Function to parse and analyse the received command, and schedule the execution of execute_command;
    static void parse(char *message);

    
private:

    //Function to show a log message if the parsing failed;
    static void log_parsing_error(const TerminalTree *const log_node);

   //This function will execute a TerminalCommand, after getting its arguments, and remove them after execution;
    static task_state_t execute_command(void *data_pointer);

    //Post execution function;
    static void confirm_command_execution(const controller_data_t *data);


private:

    //The command tree
    static const TerminalTree *command_tree;


    //------------------------------------ Output -----------------------------

public :

    //System alias for echo;
    static void log(const string_t msg);

    //System alias for echo;
    static void external_log(Protocol *protocol, const string_t msg);

    //System alias for a response (doesn't add //);
    static void respond(const String msg);

    //System alias for send_position;
    static void send_position(float*){}


private:

    static Protocol *output_protocol;


};


#endif

#endif
