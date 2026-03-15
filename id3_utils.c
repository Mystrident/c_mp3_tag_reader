#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "id3_utils.h"


TagData* create_tag_data() {

    TagData *data = (TagData *)malloc(sizeof(TagData)); /*allocates memory for the struct
    malloc(sizeof(TagData) -> allocates enough bytes to store the entire struct, and then we type cast the void* pointer into a TagData* pointer
    */

    if (data) { //if data allocation is successful , we initialize pointers to NULL, IMPORTANT -> uninitialized poitners may contain garbage addresses and can cause segmentation faults

        data->version = NULL;
        data->title = NULL;
        data->artist = NULL;
        data->album = NULL;
        data->year = NULL;
        data->comment = NULL;
        data->genre = NULL;
        
    }
    return data; //returns the initialized struct
}


void free_tag_data(TagData *data) {
    if (data) {
        free(data->version);
        free(data->title);
        free(data->artist);
        free(data->album);
        free(data->year);
        free(data->comment);
        free(data->genre);
        free(data);//frees the struct itself
    }
}




