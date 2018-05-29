//
// Created by root on 5/11/18.
//

#include "llist.h"


/*
 * ------------------------------ Linked element ------------------------------
 */

/*
 * lelmt_concat : concatenates the couple of elements; If their predecessors / successors are allocated, error;
 */

void lelmt_concat(linked_element_t *prev, linked_element_t *next) {

    //First, if the first's output or the second's input is already allocated :
    if (prev->next || next->prev) {

        //Error. Association must not overwrite previous associations;
        return;//TODO EXCEPTION;

    }

    //The next of the first stream must be the second;
    prev->next = next;

    //The previous of the second string must be the first;
    next->prev = prev;

}


/*
 * lelmt_get_first : searches for the first element of the list;
 */

linked_element_t *lelmt_get_first(linked_element_t *element) {

    //Cache the previous element;
    linked_element_t *prev;

    //Search the linked list for the first element;
    while ((prev = element->prev))
        element = prev;

    //Return the first element;
    return element;

}


/*
 * lelmt_get_last : searches for the last element in the list;
 */

linked_element_t *lelmt_get_last(linked_element_t *element) {

    //Cache the next element;
    linked_element_t *next;

    //Search the linked list for the first stream in order of data propagation;
    while ((next = element->next))
        element = next;

    //Return the last element;
    return next;

}


/*
 * lelmt_insert_after : inserts the whole list where new belongs between prev and its eventual successor
 */

void lelmt_insert_after(linked_element_t *src_prev, linked_element_t *new_element) {

    linked_element_t *new_first = lelmt_get_first(new_element);
    linked_element_t *new_last = lelmt_get_last(new_element);

    //Cache the current successor of src_prev;
    linked_element_t *src_next = src_prev->next;

    //Concatenate src_prev and new_first;
    lelmt_concat(src_prev, new_first);

    //If the src element already had a src_next element :
    if (src_next) {

        //Concatenate src_next and new_last;
        lelmt_concat(src_next, new_last);

    }

}


/*
 * lelmt_insert_before : inserts the whole list where new belongs between next and its eventual predecessor;
 */

void lelmt_insert_before(linked_element_t *src_next, linked_element_t *new_element) {

    linked_element_t *new_first = lelmt_get_first(new_element);
    linked_element_t *new_last = lelmt_get_last(new_element);

    //Cache the predecessor of src_next;
    linked_element_t *src_prev = src_next->prev;

    //Concatenate src_next and new_last;
    lelmt_concat(src_next, new_last);

    //If the src element already had a src_next element :
    if (src_prev) {

        //Concatenate src_prev and new_first;
        lelmt_concat(src_prev, new_first);

    }

}


/*
 * ------------------------------ Linked list ------------------------------
 */

/*
 * llist_insert_begin : inserts the new element at the beginning of the list;
 */

void llist_insert_begin(linked_list_t *list, linked_element_t *element) {

    //If the maximum size is reached :
    if (list->nb_elements == list->max_nb_elements) {
        return;//TODO ERROR;
    }

    //If the list is currently empty (increase the number of elements by the way);
    if (list->nb_elements++ == 0) {

        //Set the element as the first and the last;
        list->first = list->last = element;

    } else {

        //Concatenate the new element and the list's first one;
        lelmt_concat(element, list->first);

        //Update the list's first element;
        list->first = element;

    }

}

/*
 * llist_insert_end : inserts the new element at the end of the list;
 */

void llist_insert_end(linked_list_t *list, linked_element_t *element) {

    //If the list is currently empty (increase the number of elements by the way);
    if (list->nb_elements++ == 0) {

        //Set the element as the first and the last;
        list->first = list->last = element;

    } else {

        //Concatenate the new element and the list's last one;
        lelmt_concat(list->last, element);

        //Update the list's last element;
        list->last = element;

    }

}


//Insert the new element after an element of the list;
void llist_insert_after(linked_list_t *list, linked_element_t *src, linked_element_t *element) {

    //If src is null, the list is supposedly empty;
    if (!src) {

        //If the list is not empty (update the number of elements by the way):
        if (list->nb_elements++) {

            //Error : null provided but list not empty;
            return;//TODO ERROR;

        }

        //Set the element as the first and the last;
        list->first = list->last = element;

        //Nothing else to do;
        return;

    }

    //src is not null; The case where src doesn't belong to the list is not caught;

    //If the successor is null, src is the last element of the list;
    if (!src->next) {

        //Update the list's last element;
        list->last = element;

    }

    //Increase the number of elements;
    list->nb_elements++;

    //insert the element after src;
    lelmt_insert_after(src, element);

}

/*
 * llist_insert_before : inserts the new element before an element of the list;
 */

void llist_insert_before(linked_list_t *list, linked_element_t *src, linked_element_t *element) {

    //If src is null, the list is supposedly empty;
    if (!src) {

        //If the list is not empty (update the number of elements by the way):
        if (list->nb_elements++) {

            //Error : null provided but list not empty;
            return;//TODO ERROR;

        }

        //Set the element as the first and the last;
        list->first = list->last = element;

        //Nothing else to do;
        return;

    }

    //src is not null; The case where src doesn't belong to the list is not caught;

    //If the successor is null, src is the first element of the list;
    if (!src->prev) {

        //Update the list's first element;
        list->first = element;

    }

    //Increase the number of elements;
    list->nb_elements++;

    //Insert the element before src;
    lelmt_insert_before(src, element);

}


/*
 * llist_remove_begin : removes the first element of the list;
 */

linked_element_t *llist_remove_begin(linked_list_t *list) {

    //If the list is empty :
    if (!list->nb_elements) {

        //error;
        return 0;//TODO ERROR;

    } else {

        //Cache the first element;
        linked_element_t *elmt = list->first;

        //Cache the next element;
        linked_element_t *next = elmt->next;

        //Reset the first's neighbors;
        elmt->next = elmt->prev = 0;

        //If the next element exists :
        if (next) {

            //Reset its predecessor;
            next->prev = 0;

        }

        //Update the first element of the list;
        list->first = next;

        //Update the number of elements;
        list->nb_elements--;

        //Return the previous first element;
        return elmt;

    }

}

//Remove the last element of the list;
linked_element_t *llist_remove_end(linked_list_t *list) {

    //If the list is empty :
    if (!list->nb_elements) {

        //error;
        return 0;//TODO ERROR;

    } else {

        //Cache the last element;
        linked_element_t *elmt = list->last;

        //Cache the previous element;
        linked_element_t *prev = elmt->prev;

        //Reset the last's neighbors;
        elmt->next = elmt->prev = 0;

        //If the previous element exists :
        if (prev) {

            //Reset its successor;
            prev->next = 0;

        }

        //Update the last element of the list;
        list->last = prev;

        //Update the number of elements;
        list->nb_elements--;

        //Return the previous last element;
        return elmt;

    }

}


/*
 * ------------------------------ Linked list ------------------------------
 */

/*
 * lring_insert_after : insert an element after the current element of the ring;
 */

void lring_insert_after(linked_ring_t * ring, linked_element_t * element) {

    //If the maximum size is reached :
    if (ring->nb_elements == ring->max_nb_elements)
        return;//TODO ERROR;

    //If the ring has no elements (increase it by the way);
    if (ring->nb_elements++ == 0) {

        //Set the current element as the new one;
        ring->current = element;

        //Concatenate the new element with himself;
        lelmt_concat(element, element);

    } else {

        //Cache the current second element;
        linked_element_t *first = ring->current, *second = first->next;

        //Link the element between the first and the second;
        first->next = second->prev = element;
        element->prev = first;
        element->next = second;

    }

}


void lring_insert_before(linked_ring_t * ring, linked_element_t * element) {

    //If the maximum size is reached :
    if (ring->nb_elements == ring->max_nb_elements)
        return;//TODO ERROR;

    //If the ring has no elements (increase it by the way);
    if (ring->nb_elements++ == 0) {

        //Set the current element as the new one;
        ring->current = element;

        //Concatenate the new element with himself;
        lelmt_concat(element, element);

    } else {

        //Cache first and second element;
        linked_element_t *second = ring->current, *first = second->prev;

        //Link the element between the first and the second;
        first->next = second->prev = element;
        element->prev = first;
        element->next = second;

    }

}


//Focus on the next element;
void lring_incr(linked_ring_t * ring) {

    //If the ring is empty, nothing to do;
    if (!ring->nb_elements) {
        return;
    }

    //Set the current element as the current's next one;
    ring->current = ring->current->next;

}

//Focus on the previous element;
void lring_decr(linked_ring_t * ring) {

    //If the ring is empty, nothing to do;
    if (!ring->nb_elements) {
        return;
    }

    //Set the current element as the current's next one;
    ring->current = ring->current->prev;

}


/*
 * llist_direct_process : This function executes process for all elements after the provided one:
 */

void llist_direct_process(linked_element_t *element, void (*process)(linked_element_t *)) {

    //If process is a null pointer, nothing to do;
    if (!process)
        return;

    do {

        //Process the stream;
        (*process)(element);

        //Update the stream;
        element = element->next;

        //While there is stream to process in the linked list;
    } while (element);

}


/*
 * llist_direct_process_full : Searches for the first element, and executes process for all elements after;
 */

void llist_direct_process_full(linked_element_t *element, void (*process)(linked_element_t *)) {

    //Search for the first element;
    linked_element_t *first = lelmt_get_first(element);

    //Process from the first element;
    llist_direct_process(first, process);

}


/*
 * llist_reverse_process : This function executes process for all elements before the provided one:
 */

void llist_reverse_process(linked_element_t *element, void (*process)(linked_element_t *)) {

    //If process is a null pointer, nothing to do;
    if (!process)
        return;

    do {

        //Process the element;
        (*process)(element);

        //Update the stream;
        element = element->prev;

        //While there is stream to process in the linked list;
    } while (element);

}


/*
 * Searches for the last element, and executes process for all elements before;
 */

void llist_reverse_process_full(linked_element_t *element, void (*process)(linked_element_t *)) {

    //Get the last element;
    linked_element_t *last = lelmt_get_last(element);

    //Propagate data from this stream;
    llist_reverse_process(last, process);

}

