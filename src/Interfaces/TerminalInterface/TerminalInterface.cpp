/*
  TerminalInterface.cpp - Part of TRACER

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
#include <TaskScheduler/TaskScheduler.h>

#ifdef ENABLE_TERMINAL_INTERFACE


#include "TerminalInterface.h"
#include <interface.h>
#include <Project/InterfaceCommands/_interface_data.h>
#include <DataStructures/StringParser.h>
#include "Project/InterfaceCommands/TerminalInterfaceCommands.h"
#include "../../hardware_language_abstraction.h"
#include "TerminalNode.h"


/*
 * init : this function initialises the serial, and sets up the command processing environment.
 *
 */
void TerminalInterface::init() {

    //Initialise the serial
    terminal_interface_link_t::begin();

    //Wait for the serial to  correctly init
    delay_ms(100);

    //Setup a correct command environment
    reset();


}


/*
 * echo : this function sends a String over the serial.
 *
 */

void TerminalInterface::echo(const string_t msg) {
    terminal_interface_link_t::send_str(msg + "\n");
}

/*
 * read_data : this function reads and saved the serial available data.
 *
 */

void TerminalInterface::read_data() {

    while (terminal_interface_link_t::available()) {

        //Read the serial
        char read_char = terminal_interface_link_t::read();

        //If the recieved char is a line feed or a carriage_id return
        if ((read_char == 10) || (read_char == 13)) {

            //If a char has effectively been received
            if (command_size) {

                //Parse and execute_progmem_style the command
                execute();

                //Reset the data_in
                reset();

                if (!TaskScheduler::available_spaces(255))
                    return;

            }

        } else {

            //Append the read_output char to data_in
            *(data_in++) = read_char;

            command_size++;
        }
    }
}


/*
 * reset : this function resets the command reception environment
 */

void TerminalInterface::reset() {
    command_size = 0;
    data_in = data_in_0;
}


void TerminalInterface::prepare_execution() {
    //Mark the end of the the received command
    *data_in = 0;

    //Display the revieved command
    echo("\n" + str(PROJECT_NAME) + "> " + str(data_in_0));

    //Setup and save the message state
    data_in = data_in_0;

}


//--------------------------------------Arguments Processing--------------------------------------


bool TerminalInterface::parse_arguments(char *) {
    return false;
}



float TerminalInterface::get_argument(char id) {
    return 0;
}


bool TerminalInterface::verify_arguments_presence(const char *identifiers) {
    return false;
}


//-----------------------------------------------------Execution--------------------------------------------------------

void TerminalInterface::execute() {

    prepare_execution();

    //Initialise the current current_node to the root;
    TerminalNode *current_node = command_tree;
    TerminalNode *current_sub_node;

    TerminalNode **sub_nodes = current_node->sub_nodes;

    //get the first word
    StringParser::get_next_word(&data_in, &command_size);

    uint8_t i;

    node_check:

    //Check every sub_node
    for (i = 0; i < current_node->sub_nodes_nb; i++) {
        current_sub_node = sub_nodes[i];

        const char* c = (*current_sub_node->name).c_str();

        //If the current word matches the current_node's name
        if (!strcmp(c, StringParser::word_buffer_0)) {

            //Re-init the current data
            current_node = current_sub_node;
            sub_nodes = current_node->sub_nodes;

            //if the new node is not a leaf, check sub nodes
            if (current_node->sub_nodes_nb) {

                //Go to the lower level
                StringParser::get_next_word(&data_in, &command_size);

                //check the new node
                goto node_check;

            } else {

                if (arguments_sequences_storage.available_spaces()) {

                    //Save the arguments sequence.
                    uint8_t index = arguments_sequences_storage.insert_argument(data_in, command_size);

                    //Create a struct in the heap to contain argument-related data.
                    terminal_interface_data_t *data = new terminal_interface_data_t();
                    data->node = current_node;
                    data->arguments_index = index;

                    //Create a task in the stack to contain task data
                    task_t t = task_t();
                    t.type = 255;
                    t.args = (void *)data;
                    t.task = current_node->function;

                    //Schedule the task
                    TaskScheduler::add_task(t);

                }

            }

        }

    }

    //If the current node didn't contain the required command : log the node's content
    log_tree_style(current_node, false);

}


//--------------------------------------------------Tree generation-----------------------------------------------------

TerminalNode *TerminalInterface::generate_tree() {
    uint16_t command_counter = 0;

    uint8_t root_sons_nb = get_sub_nodes_nb(command_counter++);

    TerminalNode *root = new TerminalNode(new String("root"), root_sons_nb, new String("root"),  new String("none"), 0);

    //Initialise the current tree and the history.
    TerminalNode *current_tree = root;
    TerminalNode *tree_history[MAX_DEPTH];

    //Initialise the indices history
    uint8_t indices_history[MAX_DEPTH];
    uint8_t current_index = 0;
    uint8_t depth = 0;

    uint8_t tmp_nb;


    /*
     * Procedure for a go_lower :
     *  - save the index
     *  - save the current node
     *  - get the children number for the current node;
     *  - create a new current node
     *  - increment the depth;
     */

#define GO_LOWER(name, desc)\
    indices_history[depth] = current_index;\
    current_index = 0;\
    tree_history[depth] = current_tree;\
    tmp_nb = get_sub_nodes_nb(command_counter++);\
    current_tree = new TerminalNode(new String(#name), tmp_nb, new String(#desc), new String(""), 0);\
    depth++;


    /*
     * Procedure for a go_upper :
     *  - decrement the depth;
     *  - restore the previous index;
     *  - use it to add the current node to the previous node's sons;
     *  - increment the index;
     *  - restore the previous node;
     *  - increment the command counter.
     */

#define GO_UPPER()\
    depth--;\
    current_index = indices_history[depth];\
    tree_history[depth]->sub_nodes[current_index++] = current_tree;\
    current_tree = tree_history[depth];\
    command_counter++;


    /*
     * Procedure for a leaf creation :
     *  - create a leaf node;
     *  - add it to the current node;
     *  - increment the index;
     *  - increment the command counter.
     */

#define CREATE_LEAF(name, function, desc, args)\
    current_tree->sub_nodes[current_index++] = new TerminalNode(new String(#name), 0, new String(#desc), new String(#args), TerminalInterfaceCommands::_##function);\
    command_counter++;

#include "Project/Config/terminal_interface_config.h"

#undef GO_LOWER

#undef GO_UPPER

#undef CREATE_LEAF

    return root;
}


/*
 * get_sub_nodes_nb : this function determines the number of direct sub_nodes of a particular indice in the sub_nodes
 *  string.
 *
 */

uint8_t TerminalInterface::get_sub_nodes_nb(uint16_t command_index) {

    uint16_t max = (uint16_t) tree_summary->length();

    //If the command index is below the command nb, return 0 for safety
    if (command_index > max) return 0;

    //If the first command is a create_leaf of go_upper, 0 sons.
    char v = tree_summary->charAt(command_index++);
    if (v == (char)0 || v == (char)1)
        return 0;

    //If not, we will count to the next go_upper (0);

    uint8_t sons_nb = 0;
    uint8_t depth = 0;

    v = tree_summary->charAt(command_index++);

    //We stop counting when we detect a zero at depth zero, or when the all command have been listed.
    // -> keep while the depth is not zero or the value is not zero, and command are still to be processed.
    while (((depth || v)) && (command_index != max)) {
        //correct the depth if we go upper of lower
        if (v == 0) {
            depth--;
        } else if (v == (char)1) {
            if (!depth) sons_nb++;
        } else if (v == (char)2) {
            if (!depth) sons_nb++;
            depth++;
        }
        v = tree_summary->charAt(command_index++);
    }

    return sons_nb;

}


/*
 * generate_sub_nodes_string : this function generates a string, containing the summary of the tree creation :
 *  - 0 means go_upper
 *  - 1 means create_leaf
 *  - 2 means go_lower
 *
 *  This string is used to determine the number of sub_nodes of a particular node.
 *
 */
String * TerminalInterface::build_tree_summary() {

    String *s = new String();

    //Set the initial go_lower
    s->append((char)2);

#define GO_LOWER(...) s->append((char)2);

#define GO_UPPER(...) s->append((char)0);

#define CREATE_LEAF(...) s->append((char)1);

#include "Project/Config/terminal_interface_config.h"

#undef GO_UPPER

#undef GO_LOWER

#undef CREATE_LEAF

    return s;

}

//---------------------------------Functions called by TerminalInterfaceCommands------------------------------



/*
 * get_arguments : returns a pointer to the beginning of the required argument
 *
 */

char *TerminalInterface::get_arguments(uint8_t task_index, uint8_t *size) {
    return arguments_sequences_storage.get_argument(task_index, size);
}

/*
 * validate_task : removes the argument related to the task
 *
 */

void TerminalInterface::validate_task(uint8_t task_index) {
    arguments_sequences_storage.remove_argument(task_index);
}



/*
 * log_tree_style : displays a message, after a parsing or execution problem.
 *
 *  If the problem comes from the parsing (log_args = false), it will display the node's children.
 *
 *  If it comes from the execution (log_args = true), then it will display the node's args.
 *
 */

void TerminalInterface::log_tree_style(TerminalNode *log_node, bool log_args) {

    if (log_args) {

        //If the arguments parsing failed, display the correct syntax for arguments
        echo("Usage : " + *log_node->name + " " + *log_node->args_log);

    } else {

        //If the command parsing failes, display the last correct node's content.

        //init an empty string
        String s = "";

        //Fill it with the name and description of direct sub_nodes
        for (int i = 0; i < log_node->sub_nodes_nb; i++) {
            TerminalNode * t = log_node->sub_nodes[i];
            s += *t->name + "\t\t : " + *t->desc_log + "\n";
        }

        //Display
        echo(s);

    }

}

//---------------------------------Static declarations / definitions------------------------------


#define m TerminalInterface


unsigned char m::command_size = 0;

char tdatain_terminal[MAX_COMMAND_SIZE];
char *m::data_in = tdatain_terminal;
char *const m::data_in_0 = tdatain_terminal;

ArgumentsContainer m::arguments_sequences_storage = ArgumentsContainer(MAX_ARGS_SIZE, NB_PENDING_COMMANDS);


String *m::tree_summary = m::build_tree_summary();

//Create the command tree summary

//Build the command tree
TerminalNode *m::command_tree = m::generate_tree();



#undef m

#endif