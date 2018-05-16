//
// Created by root on 5/12/18.
//

#ifndef TRACER_COPY_STREAM_H
#define TRACER_COPY_STREAM_H

#include "connection.h"

/*
 * The copy stream is a stream composition, that copies data it transmits.
 *
 *  Its data flux function determines the amount of data to transit, declares an array of the appropriate size, and
 *  passes it to the transmitter, that copies its data in it.
 *
 *  Then, it passes the array to the receiver that processes it;
 */

typedef struct {

    //--------- Composition ---------

    //The stream structure;
    connection_flux_t flux;

    //--------- Transmission ---------

    //The function for the transmitter to copy its data;
    void (*copy_data_in)(void *tx_struct, void *data_array, size_t size);

    //The function for the transmitter to receive its data;
    void (*copy_data_from)(void *rx_struct, void *data_array, size_t size);

} element_array_flux_t;


//The data flux function for a copy stream;
void elemeny_array_flux(connection_flux_t *flux);



#endif //TRACER_COPY_STREAM_H
