/* 
 * File:   circular_array.c
 * Author: Claude Abounegm
 */

#include "circular_array.h"

void circ_alloc(circular_array* arr, size_t size)
{
    arr->array = (char**) malloc(size * sizeof (char*));
    arr->size = size;
    arr->offset = 0;
    arr->count = 0;
    arr->additions = 0;
}

void circ_free(circular_array* arr)
{
    // free memory allocated using malloc()
    int i;
    for (i = 0; i < arr->count; ++i)
        free(circ_getRelativeAt(arr, i));
    free(arr->array);

    // reset to defaults to avoid misusage
    arr->array = NULL;
    arr->size = 0;
    arr->offset = 0;
    arr->count = 0;
    arr->additions = 0;
}

void circ_push(circular_array* arr, char* str)
{
    // if count has reached its maximum, then we free the item
    // at the current index, as we are going to replace it.
    if (arr->count == arr->size)
        free(arr->array[arr->offset]);
        // otherwise, we just increment count until we reach cap.
    else if (arr->count < arr->size)
        ++arr->count;

    // for easier memory management, we just allocate our own strings.
    // we can then free them later as we want, without worrying 
    // about external factors.
    arr->array[arr->offset] = strdup(str);
    // advance to the next index.
    arr->offset = (arr->offset + 1) % arr->size;
    // increment the total count
    ++arr->additions;
}

char* circ_getRelativeAt(const circular_array* arr, int index)
{
    return circ_getAbsoluteAt(arr, (arr->offset + index));
}

char* circ_getAbsoluteAt(const circular_array* arr, int index)
{
    if (index < 0)
        return NULL;

    return arr->array[index % arr->count];
}

void circ_foreach(const circular_array* arr, void (*fn)(const circular_array* arr, int i, const char* str))
{
    int i;
    for (i = 0; i < arr->count; ++i)
        fn(arr, i, circ_getRelativeAt(arr, i));
}

void circ_foreach_reverse(const circular_array* arr, void (*fn)(const circular_array* arr, int i, const char* str))
{
    int i;
    for (i = arr->count - 1; i >= 0; --i)
        fn(arr, i, circ_getRelativeAt(arr, i));
}

/*
void dump_array(circular_array* arr)
{
    int i;
    for (i = 0; i < arr->count; ++i)
        printf("[%d]: \"%s\"\n", i, arr->array[i]);
    printf("\n");
}*/