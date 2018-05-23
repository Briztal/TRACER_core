//
// Created by root on 3/10/18.
//

#include <malloc.h>
#include <Kernel/kernel.h>
#include <string.h>

#include "string.h"

#define SIZE_LIMIT (uint16_t) 255


//-------------------------------------- To string conversion --------------------------------------

/*
 * Following functions set the given array as the string representation of basic number types;
 */

void string_uint8_to_a(uint8_t i, char *data);

void string_int8_to_a(int8_t i, char *data);

void string_uint16_to_a(uint16_t i, char *data);

void string_int16_to_a(int16_t i, char *data);

void string_uint32_to_a(uint32_t i, char *data);

void string_int32_to_a(int32_t i, char *data);

void string_uint64_to_a(uint64_t i, char *data);

void string_int64_to_a(int64_t i, char *data);


size_t string_uint8_to_as(uint8_t i, char *data);

size_t string_uint16_to_as(uint16_t i, char *data);

size_t string_uint32_to_as(uint32_t i, char *data);

size_t string_uint64_to_as(uint64_t i, char *data);



/*
 * length_to_size : this function converts a string length (null termination and size of char non comprised) to a
 *  real array size;
 */

inline size_t length_to_size(const size_t length) {

    return  (length + 1) * sizeof(char);

}


//Create a string of the given length
string_t *string_create(const size_t string_length, const char *const heap_data) {

    //Create the string initializer;
    string_t init = {
            .length = string_length,
            .data = heap_data,
    };

    //If the allocation completed, cache a casted copy of the pointer. Compiler optimised;
    string_t *string = kernel_malloc_copy(sizeof(string_t), &init);

    //Return the created string;
    return string;

}


/*
 * string_create_from_symmetric : allocates heap space for the string, and copies symmetrically the source;
 *
 *  Calls string_create and returns the result;
 */

string_t *string_create_from_symmetric(const size_t string_length, const char *const stack_src) {

    //Cache the size of the zone to allocate zone;
    size_t size = length_to_size(string_length);

    //Create a memory space in the heap for data;
    void *ptr = kernel_mallocc(size);

    //Symmetric copy from stack to heap; dest is null-terminated;
    string_symmetric_copy(ptr, stack_src, string_length);

    //Built the string and return the result;
    return string_create(string_length, ptr);

}


/*
 * string_create_from_stack : allocates heap space for data, copies the source.
 *
 *  Calls string_create and returns the result;
 */
string_t *string_create_from_stack(const size_t string_length, const char *const stack_src) {

    //Cache the size of the zone to allocate zone;
    size_t size = length_to_size(string_length);

    //Create a memory space in the heap for data;
    void *ptr = malloc(size);

    //If the allocation failed, error;
    if (!ptr)
        return 0;//TODO ERROR.

    //Symmetric copy from stack to heap; NO NULL TERMINATION;
    memcpy(ptr, stack_src, string_length);

    //Null terminate the string;
    *((char *)ptr + string_length) = 0;

    //Built the string and return the result;
    return string_create(string_length, ptr);

}

/*
 * string_delete : a string deletion required two operations (data and struct deletion).
 *  This function implements both;
 */

void string_delete(const string_t *const string) {

    //Free the string's data;
    free((void *) string->data);

    //Free the string;
    free((void *)string);

}


/*
 * String move : creates a new string, and moves src's data to it. Then, resets src to an empty string;
 */

string_t *string_move(string_t *src) {

    //First, create a copycat of src, phagocyting its data;
    string_t *clone = string_create(src->length, src->data);

    //Then, allocate data in the heap for an empty string;
    char *ptr = kernel_mallocc(length_to_size(0));

    //Initialise the string to empty.
    *ptr = 0;

    //Create a stack initialiser;
    string_t init =  {
            .data = ptr,
            .length = 0,
    };

    //Re-initialise src with new data;
    memcpy(src, &init, sizeof(string_t));

    //Return the new string;
    return clone;

}




/*
 * string_resize : resizes a string to the given length;
 */

/*
void string_resize(string_t *string, size_t new_length) {
    
    //Cache the current length;
    size_t current_length = string->length;
    
    //A string can't be truncated;
    if (new_length < current_length) {
        //TODO ERROR;
        return;
    }
    
    //If the new size if the same that the current, nothing to do;
    if (new_length == current_length)
        return;
    
    //Determine current and new sizes;
    size_t current_size = length_to_size(current_length);
    size_t new_size = length_to_size(new_length);
    
    //First, let's reallocate the string's array to the required size;
    void *ptr = realloc(string->data, new_size);
    
    //If the allocation failed, error;
    if (!ptr)
        return;//TODO ERROR.
    
    //Cache the beginning of the string's new part;
    uint8_t *new_part = (uint8_t*) ptr + current_size;
    
    //Now, reset the new part of the string;
    for (size_t counter = new_size - current_size; counter--;) {
        
        //Reset the current byte of the new part;
        *(new_part++) = 0;
        
    }
    
    //Re-initialise the string;
    *string = {
            .data = ptr, 
            .length = new_length,
    };
}

 */
//---------------------------------------- Primitive types constructors ----------------------------------------



//-------------------------------------- Assignment operators--------------------------------------

#define uint_to_a(i, max_digits, nb_bits)\
    /*Declare a temporary array;*/\
    char t[max_digits];\
    \
    /*Convert the int to its string value in reverse order in t;*/\
    uint8_t data_length = string_uint##nb_bits##_to_as(i, t);\
    \
    /*Create and return a string of the given length with uint data;*/\
    return string_create_from_symmetric(data_length, t);\

#define int_to_a(i, max_digits, nb_bits)\
    \
    /*First, determine the sign and take the absolute value of i*/\
    bool negative;\
    if ((negative = (i) < 0)) (i) = -(i);\
    \
    /*Declare a temporary array;*/\
    char t[(max_digits) + 1];\
    \
    /*Convert the int to its string value in reverse order in t;*/\
    uint8_t data_length = string_uint##nb_bits##_to_as((uint##nb_bits##_t)(i), t);\
    \
    /*Write the sign and increase the length;*/\
    if (negative) {t[data_length++] = '-';}\
    \
    /*Create and return a string of the given length with int data;*/\
    return string_create_from_symmetric(data_length, t);\


#define integer_constructor(length, max_digits)\
    string_t *str_u##length(uint##length##_t i) {\
        uint_to_a(i, max_digits, length);\
    }\
    string_t *str_s##length(int##length##_t i) {\
        int_to_a(i, max_digits, length);\
    }


/*
 * Implement all initializer, for all uints and ints of all sizes;
 */

integer_constructor(8, 3);

integer_constructor(16, 5);

integer_constructor(32, 10);

integer_constructor(64, 20);

#undef uint_to_a



/*
 * setTo : this function sets the string as the representation of the given float,
 *  with the given resolution, ie the given number of figures after comma.
 */

string_t *str_f(float f, uint8_t resolution) {

    //Before anything, let's determine the sign and take the absolute value;
    bool negative;
    float abs = ((negative = (f < 0))) ? -f : f;

    //The first step will be to call a constructor for the integer part;
    string_t *int_string;

    //Declare the fractional part;
    float frac;


    /*
     * First, we will determine the optimal function to call to convert the integer part;
     *  bounds will depend on the sign;
     *
     * As float integer part can be signed, we will only call the signed int constructor;
     *
     * As signed numbers absolute bounds differ of 1 (ex : -128, 127), the negative bound will be treated of the next
     *  greater type (except for the greatest type;
     */

    if (abs <= (float) ((uint8_t) -1 >> 1)) {
        //1 byte integer part :
        int_string = str_s8((int8_t) abs);
        frac = f - (float) (int8_t) f;
    } else if (abs < (float) ((uint16_t) -1 >> 1)) {
        //2 bytes integer part :
        int_string = str_s16((int16_t) abs);
        frac = f - (float) (int16_t) f;
    } else if (abs < (float) ((uint32_t) -1 >> 1)) {
        //4 bytes integer part :
        int_string = str_s32((int32_t) abs);
        frac = f - (float) (int32_t) f;
    } else if (abs < (float) ((uint64_t) -1 >> 1)) {
        //4 bytes integer part :
        int_string = str_s64((int64_t) abs);
        frac = f - (float) (int64_t) f;
    } else {

        //Create and return a string containing ovf
        int_string = string_create(3, "ovf");
        return int_string;

    }

    //If the fractional part is negative, take its absolute value;
    if (frac < 0)
        frac = -frac;

    //Create a temp array to contain the frac part. + 1 from the dot; no null termination required; cache a copy;
    char frac_part[resolution + 1], *insertion_ptr = frac_part;

    //Insert the dot;
    *(insertion_ptr++) = '.';

    //For each digit under the comma :
    for (; resolution--;) {

        //Multiply the fractional part by 10;
        frac *= (float) 10;

        //Get the digit;
        char c = (char) ((uint8_t) frac + (uint8_t) 48);

        frac -= (float) (uint8_t) frac;

        //Insert the digit;
        *(insertion_ptr++) = c;

    }

    //Create a string from the stack array;
    //string_t *frac_string = string_create_from_stack(resolution + 1, frac_part);TODO

    //For instance, return the integer part;
    //TODO TSTRING;
    return int_string;
}

//--------------------------------------------- Memory copy utils ---------------------------------------------


/*
 * symmetric_copy : this function turns dst to the symmetric of sym_str;
 */

void string_symmetric_copy(void *dst, const void *sym_str, size_t size) {

    //Convert dst and src to a uint8_t pointer, as arithmetic on void * is illegal;
    uint8_t *d = (uint8_t *) dst;
    uint8_t *s = (uint8_t *) sym_str;

    //Increment the dst pointer;
    d += size - (uint8_t) 1;

    //Nullify the end;
    *(d + 1) = 0;

    //For each char :
    for (; size--;) {

        //Symmetric copy from src to dst;
        *(d--) = *(s++);

    }

}


//-------------------------------------- C style functions --------------------------------------

/*
 * length : this function determines the length of the string (without the \0 comprised);
 */

uint8_t string_length(const char *in_buffer) {

    uint8_t char_count = 0;

    //While the current char is not null;
    while (*(in_buffer++)) {

        //Increment the size of the copied buffer.
        char_count++;
    }

    //Return the number of characters before the end of the string
    return char_count;

}


//TODO RE-COMMENT, THIS IS THE OLD FUNCTION

/*
 * string_count_until_char : this function will return the number of chars to enumerate from the string's beginning
 * before encountering the required char;
 */

uint8_t string_count_until_char(const char *in_buffer, const char limit_char) {

    uint8_t char_count = 0;
    //Initialise a cache for the current char
    char c = *(in_buffer++);

    //While the current char is not null, or the limit char
    while (c && (c != limit_char)) {

        //Update the current char
        c = *(in_buffer++);

        //Increment the size of the copied buffer.
        char_count++;
    }

    //Return the number of characters before the limit char (or the end of the string
    return char_count;

}


/*
 * string_lstrip : this function counts the number of time that 'verif_char' is present (consecutively) at the beginning
 *  of in_buffer. For example, with in_buffer containing   '0' '0' '0' '0' '5' '2' '\0', the result of the command
 *      lstrip(in_buffer, '0') will be 4.
 *
 */

uint8_t string_lstrip(const char *in_buffer, const char verif_char) {

    //initialise_hardware a counter
    uint8_t counter = 0;

    //Initialise a cache var for the current char
    char c = *(in_buffer++);

    //While the current char is not null, and is equal to 'verif_char'
    while (c && (c == verif_char)) {

        //Increment the presence counter
        counter++;

        //Update the char
        c = *(in_buffer++);

    }

    return counter;

}


/*
 * count_words : this function determines the number of words in a char sequence.
 */

uint8_t string_count_words(const char *in_buffer) {

    //Initialise a word counter.
    uint8_t nb_words = 0;

    //initialise_hardware a size for the current word.
    uint8_t size;

    do {

        //Remove unnecessary nb_spaces at the beginning of the char sequence.
        in_buffer += string_lstrip(in_buffer, ' ');

        //Get the size of the next word (will ne zero only if the sequence is finished).
        size = string_count_until_char(in_buffer, ' ');

        //increment the data pointer at the end of the word.
        in_buffer += size;

        //Stop working if the size is zero (sequence finished)
    } while (size);

    //Return the number of words.
    return nb_words;

}


/*
 * strcmp : compares the two provided strings, and return true if they are equal;
 */

bool string_strcmp(const char *string0, const char *string1) {

    //Comparison loop;
    while (true) {

        //Cache the current char of string0 and increment string0;
        char c = *(string0++);

        //Compare with the current char of string1 and increment string1. If chars are different :
        if (c != *(string1++)) {

            //Found two different chars at the same index -> not equal;
            return false;

        }

        //Now, we are shure that both chars are the same;

        //If the current char is a null cleanup :
        if (!c) {

            //End of string found for both strings : both strings are the same;
            return true;

        }


        //If not, re-iterate;
    }

    //End case, never happens as strings are null terminated;
    return false;

}





//---------------------------------------- To string Privates ----------------------------------------

#define uint_to_as(i, data)\
    /*Initialise the size counter*/\
    size_t data_length = 0;\
    \
    /*Set the current digit:*/\
    do {\
        /*Increment the size counter;*/\
        data_length++;\
        \
        /*Set the current case of data to the current digit;*/\
        *((data)++) = (char) ((uint8_t)((i) % (uint8_t) 10) + (uint8_t)48);\
        \
    } while ((i)/=10);\
    /*While number is superior to zero;*/\
    \
    return data_length;


/*
 * _uint8_to_as : this function will copy the 1 byte unsigned int in the array;
 */

size_t string_uint8_to_as(uint8_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}


/*
 * _uint16_to_as : this function will copy the 1 byte unsigned int in the array;
 */

size_t string_uint16_to_as(uint16_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}


/*
 * _uint32_to_as : this function will copy the 1 byte unsigned int in the array;
 */

size_t string_uint32_to_as(uint32_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}


/*
 * _uint64_to_as : this function will copy the 1 byte unsigned int in the array;
 */

size_t string_uint64_to_as(uint64_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}

#undef uint_to_as
