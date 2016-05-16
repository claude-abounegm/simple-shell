/* 
 * File:   circular_array.h
 * Author: Claude Abounegm
 *
 */

#ifndef CIRCULAR_ARRAY_H
#define CIRCULAR_ARRAY_H

#include <stdlib.h> // malloc, free
#include <string.h> // strdup

// A circular array with a static capacity.
typedef struct circular_array {
    // The array of strings.
    char** array;

    // The offset to which we need to add the next item to.
    size_t offset;

    // The count of how many items are
    // currently available in the array.
    size_t count;

    // The actual size of the array.
    size_t size;

    // How many items were added in the array
    // since allocation.
    size_t additions;
} circular_array;


// Initializes the circular array to a usable state.
void circ_alloc(circular_array* arr, size_t size);

// Frees the memory used by the circular array. The array cannot be used
// before calling circ_alloc() again.
void circ_free(circular_array* arr);


// Pushes the item at the next empty stop in the array. If the array is full,
// the oldest item is replaced with the new one.
void circ_push(circular_array* arr, char* str);


// Gets the string located relative to (offset + index).
char* circ_getRelativeAt(const circular_array* arr, int index);

// Gets the string located relative to (index).
char* circ_getAbsoluteAt(const circular_array* arr, int index);


// Traverses the array in from oldest to newest. The array, index and string are
// passed as arguments in the callback function.
void circ_foreach(const circular_array* arr, void (*fn)(const circular_array* arr, int i, const char* str));

// Traverses the array in from newest to oldest. The array, index and string are
// passed as arguments in the callback function.
void circ_foreach_reverse(const circular_array* arr, void (*fn)(const circular_array* arr, int i, const char* str));

/*
// Used only for testing
void dump_array(circular_array* arr);*/

#endif /* CIRCULAR_ARRAY_H */