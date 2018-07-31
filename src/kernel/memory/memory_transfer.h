/*
  memory_stream.h Part of TRACER

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

#ifndef TRACER_MEM_DESC_H
#define TRACER_MEM_DESC_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


/*
 * The memory map describes as simply as possible a succession of equally spaced memory blocks;
 *
 * 	Is is made to contain as less information as possible, to be passed as argument to transfer functions.
 *
 * 	For this reason, the size of a bloc and the number of available blocks, that are information that
 * 		must be common to both memory zones during a transfer, don't appear here.
 */

struct mem_map_t {

	//The first address of the memory zone;
	volatile void *start_address;

	//The size between two bloc's first address.
	const size_t block_spacing;

};


/*
 * The block descriptor contains the size of a block, and the number of blocks;
 *
 * 	A memory transfer involves a couple of memory maps, and only one bloc descriptor;
 */

struct blocks_desc_t {

	//The size of a data bloc;
	size_t block_size;

	//The current number of available blocks in the memory zone;
	size_t nb_blocks;

};


//------------------------------------------------------ Transfer ------------------------------------------------------

//Merge the couple of bloc descriptors;
bool block_desc_merge(const struct blocks_desc_t *in0, const struct blocks_desc_t *in1,struct blocks_desc_t *out);


//------------------------------------------------ Memory map transfers ------------------------------------------------

//Transfer memory by chunks of one byte;
void mmap_transfer_8(struct mem_map_t src, struct mem_map_t dst, size_t blocks_size, size_t nb_blocks);

//Transfer memory by chunks of 2 bytes;
void mmap_transfer_16(struct mem_map_t src, struct mem_map_t dst, size_t block_size, size_t nb_blocks);

//Transfer memory by chunks of 4 bytes;
void mmap_transfer_32(struct mem_map_t src, struct mem_map_t dst, size_t block_size, size_t nb_blocks);



//-------------------------------------------------- Memory Descriptor ------------------------------------------------

/*
 * The memory descriptor : describes completely a succession of memory blocks.
 *
 * 	Composed by a memory map and a bloc descriptor;
 */

struct mem_desc_t {

	//The memory map;
	struct mem_map_t memory_map;

	//The current bloc descriptor;
	struct blocks_desc_t bloc_desc;

};


//--------------------------------------------------- Address query ----------------------------------------------------

//Get the address of a bloc;
volatile void *mem_desc_get_bloc(const struct mem_desc_t *mem_desc, size_t bloc_id);

//Get a pointer to the given offset of a particular bloc;
volatile void *mem_desc_get_bloc_member(const struct mem_desc_t *mem_desc, size_t bloc_id, size_t offset_in_bloc);

//Transfer the maximal amount of memory from a memory descriptor to another; Returns the number of copied blocks;
size_t mem_desc_transfer(const struct mem_desc_t *src, const struct mem_desc_t *dst);

#endif //TRACER_MEM_DESC_H
