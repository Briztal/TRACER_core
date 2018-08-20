/*
  memory_stream.c Part of TRACER

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

#include "memory_stream.h"

#include <kernel/syscall.h>

//--------------------------------------------------- Private headers --------------------------------------------------

//Close the pipe, or program the closing;
void _stream_pipe_close(struct stream_pipe_t *pipe);


//----------------------------------------------------- Destruction ----------------------------------------------------

/*
 * stream_delete : deletes the registered transfer if there is one, and delete the memory zone;
 */

void stream_delete(struct stream_t *const stream) {

	//If the stream is null :
	if (!stream) {

		//Error;
		kernel_error("memory_stream.c : stream : the stream is null;");

		//Never reached;
		return;

	}

	//If the stream has a registered pipe :
	if (stream->registered_pipe) {

		//Error;
		kernel_error("memory_stream.c : stream : the stream has a registered pipe");

	}

	//Cache the reference to the stream's owned pipe;
	struct stream_pipe_t *pipe = stream->owned_pipe;

	//If the stream owns a pipe :
	if (pipe) {

		//Delete all its dynamic data;
		(*(pipe->destructor))(pipe);

		//Delete the pipe struct;
		kernel_free(pipe);

	}

	//Delete the stream data and structure;
	(*(stream->deleter))(stream);

}


/*
 * stream_discard : calls the stream's discard function if it exists and if data was actually transferred;
 */

inline void stream_discard(struct stream_t *const stream, const size_t nb_transferred_blocks) {

	//If the transfer has happened and the function is not null : 
	if (nb_transferred_blocks && stream->discard_blocks) {

		//Call the function;
		(*(stream->discard_blocks))(stream, nb_transferred_blocks);

	}

}


//---------------------------------------------------- Read - write ----------------------------------------------------

/*
 * transfer_blocks : setups the transfer and calls the stream transfer function;
 */

size_t stream_transfer(struct stream_t *const stream, struct mem_desc_t *const mem) {

	//If the stream has an opened pipe registered :
	if (stream->registered_pipe) {

		//Error, can't read on a stream with a registered pipe;
		kernel_error("memory_stream.c : stream_transfer : the stream has an opened pipe registered;");

	}

	//Create the stream's block descriptor;
	struct blocks_desc_t stream_descriptor = {
		.block_size = stream->block_size,
		.nb_blocks = (*(stream->get_available_blocks))(stream),
	};

	//Create the transfer's block descriptor;
	struct blocks_desc_t descriptor;

	//Merge the couple of bloc descriptors; Will assert if a transfer can happen;
	bool transferable = block_desc_merge(&stream_descriptor, &mem->bloc_desc, &descriptor);

	//If no transfer can happen, nothing to do;
	if (!transferable) {
		return 0;
	}

	//Transfer blocks, and return the size of the copy;
	(*(stream->transfer_blocks))(stream, &mem->memory_map, &descriptor);

	//Discard some blocks;
	stream_discard(stream, descriptor.nb_blocks);

	//Return the number of read blocks;
	return descriptor.nb_blocks;

}


/*
 * stream_read : executes the transfer if the stream is output;
 */

size_t stream_read(struct stream_t *const stream, struct mem_desc_t *const dst) {

	//If the stream is an input stream :
	if (stream->input_stream) {

		//Error, wrong direction. Do nothing;
		return 0;

	}

	//Execute the transfer;
	return stream_transfer(stream, dst);

}


/*
 * stream_write : executes the transfer if the stream is input;
 */

size_t stream_write(struct stream_t *const stream, struct mem_desc_t *const src) {

	//If the stream is an output stream :
	if (!stream->input_stream) {

		//Error, wrong direction. Do nothing;
		return 0;


	}

	//Execute the transfer;
	return stream_transfer(stream, src);

}


//---------------------------------------------------- Stream Pipes ----------------------------------------------------

//Activate a stream pipe, given the owner stream and the client stream; Both streams will be opened by the pipe;
//TODO FILENAMES + PIPE FILENAME
void stream_pipe_open(struct stream_t *const owner, struct stream_t *const client, size_t transfer_size) {

	//If a null pointer was provided :
	if (!owner) {

		//Error, impossible to open the pipe;
		kernel_error("memory_stream.c : stream_pipe_open : the owner is null;");

		//Never reached;
		return;

	}

	//If a null pointer was provided :
	if (!client) {

		//Error, impossible to open the pipe;
		kernel_error("memory_stream.c : stream_pipe_open : the client is null;");

		//Never reached;
		return;

	}

	//If the transfer size is null :
	if (!transfer_size) {

		//Error, a bloc transfer has a non null size;
		kernel_error("memory_stream.c : stream_pipe_open : null transfer size;");

	}



	//Cache the pipe the stream owns;
	struct stream_pipe_t *pipe = owner->owned_pipe;

	//If the stream owns no pipe :
	if (!pipe) {

		//Error, impossible to open the pipe;
		kernel_error("memory_stream.c : stream_pipe_open : the provided stream owns no pipe;");

		//Never reached;
		return;

	}


	//If the pipe is not closed:
	if (pipe->state != STREAM_PIPE_CLOSE) {

		//Error, impossible to open the pipe;
		kernel_error("memory_stream.c : stream_pipe_open : the required pipe is not closed;");

	}

	//If both streams block size don't match :
	if (client->block_size != owner->block_size) {

		//Error, streams must equal block sizes;
		kernel_error("memory_stream.c : stream_pipe_open : streams have different block sizes;");

	}

	//If both streams have the same direction :
	if (client->input_stream == owner->input_stream) {

		//Error, streams must have opposite directions;
		kernel_error("memory_stream.c : stream_pipe_open : streams have the same direction;");

	}


	//Register the pipe in both owner and client;
	owner->registered_pipe = client->registered_pipe = pipe;

	//Update the pipe's client;
	pipe->client_stream = client;

	//Update the pipe's data according to the new client and transfer size;
	(*(pipe->open))(pipe);

	//Set the watermark to the appropriate function;
	(*(pipe->update_watermark))(pipe, transfer_size);

	//Save the transfer size;
	pipe->transfer_size = transfer_size;

	//Update the pipe's state;
	pipe->state = (volatile enum stream_pipe_state_t) STREAM_PIPE_OPEN;

	//Start the transfer; When done, will call confirm;
	stream_pipe_prepare_transfer(pipe);

}

/*
 * stream_pipe_close : attempts to close a pipe. If possible, un-registers itself and close both owned streams;
 *  If not possible, schedules its closing, will be done at the current transfer's confirmation;
 */

void stream_pipe_close(struct stream_pipe_t *const pipe) {

	//If a null pointer was provided :
	if (!pipe) {

		//Error;
		kernel_error("memory_stream.c : stream_pipe_close : null pointer provided;");

		//Never reached;
		return;

	}

	//If the pipe is already closed :
	if ((pipe->state == STREAM_PIPE_CLOSE) || (pipe->state == STREAM_PIPE_CLOSE_REQUIRED)) {

		//Error, deleted a closed stream;
		kernel_error("memory_stream.c : stream_pipe_close : Attempted to delete a closed stream;");

	}

	//TODO REMOVE FROM FILESYSTEM;

	//The pipe is open;

	//If the pipe can be closed :
	if ((*(pipe->close))(pipe)) {

		//Effectively close the pipe;
		_stream_pipe_close(pipe);


	} else {
		//If the pipe cannot be closed :

		//Update the state to STREAM_PIPE_CLOSE_REQUIRED to call _close at next confirmation;
		pipe->state = (volatile enum stream_pipe_state_t) STREAM_PIPE_CLOSE_REQUIRED;

	}

}


/*
 * _stream_pipe_close : actually closes the pipe. Must be called with caution;
 */

void _stream_pipe_close(struct stream_pipe_t *const pipe) {

	//Disable the trigger;
	(*(pipe->disable_trigger))(pipe);

	//Cache the pipe's owner and client;
	struct stream_t *const owner = pipe->owner_stream, *const client = pipe->client_stream;

	//Un-register the stream from the client and the owner;
	owner->registered_pipe = 0;
	client->registered_pipe = 0;

	//Reset the client;
	pipe->client_stream = 0;

	//Update the pipe's state;
	pipe->state = (volatile enum stream_pipe_state_t) STREAM_PIPE_CLOSE;

}


/*
 * stream_pipe_prepare_transfer : checks that the pipe is not null and can be started, and prepares the transfer;
 * 
 * called after a transfer confirmation, and when the memory zone is updated;
 */

void stream_pipe_prepare_transfer(struct stream_pipe_t *const pipe) {

	//If the pipe is null :
	if (!pipe) {

		//Nothing to do;
		return;

	}

	//If the pipe is not open :
	if (pipe->state != STREAM_PIPE_OPEN) {

		//Nothing to do; If started, no need to start, and if closed or in closing, can't restart;
		return;

	}

	//Cache the client;
	struct stream_t *client = pipe->client_stream;

	//Get the current bloc descriptor;

	//Mark the pipe STARTED;
	pipe->state = (volatile enum stream_pipe_state_t) STREAM_PIPE_STARTED;

	//Create an empty bloc descriptor, to get the current client's size;
	//Determine the
	// current bloc descriptor of the client channel; Will include the current size;
	size_t nb_client_blocs = (*(client->get_available_blocks))(client);

	//If the current available size is greater than the size of a transfer :
	if (nb_client_blocs >= pipe->transfer_size) {

		//The pipe is open. We can prepare the transfer;
		(*(pipe->prepare_transfer))(pipe);

		//Enable the trigger;
		(*(pipe->enable_trigger))(pipe);

	} else {

		//If there is no room for a transfer for instance :

		//Mark the pipe OPEN;
		pipe->state = (volatile enum stream_pipe_state_t) STREAM_PIPE_OPEN;

		//Disable the interrupt;
		(*(pipe->disable_trigger))(pipe);

	}

}


/*
 * stream_pipe_confirm_transfer : discards data in the couple of memory zones;
 */

void stream_pipe_confirm_transfer(struct stream_pipe_t *const pipe, const size_t nb_transferred_blocks) {

	//If the pipe's closing is required :
	if (pipe->state == STREAM_PIPE_CLOSE_REQUIRED) {

		//Close the pipe;
		_stream_pipe_close(pipe);

	}

	//Disable the trigger;

	//Cache the pipe's owner and client;
	struct stream_t *const owner = pipe->owner_stream, *const client = pipe->client_stream;

	//Force the state to STARTED, to avoid transfer preparations before all zones have been updated;
	pipe->state = (volatile enum stream_pipe_state_t) STREAM_PIPE_STARTED;


	//Discard the provided number of blocks in both streams;
	stream_discard(owner, nb_transferred_blocks);
	stream_discard(client, nb_transferred_blocks);

	//Set the state to OPEN, to allow the transfer preparation;
	pipe->state = (volatile enum stream_pipe_state_t) STREAM_PIPE_OPEN;

	//Now that both memory zones are up to date, prepare a new transfer;
	stream_pipe_prepare_transfer(pipe);

}
