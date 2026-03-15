#ifndef ID3_UTILS_H
#define ID3_UTILS_H

//structure to hold the id3 metadata

typedef struct {
    char *version; /**< Version of the ID3 tag */
    char *title;   /**< Title of the song */
    char *artist;  /**< Artist of the song */
    char *album;   /**< Album name */
    char *year;    /**< Year of release */
    char *comment; /**< Comment */
    char *genre;   /**< Genre */
    // Add other fields as needed

    //all of them are pointers and not fixed arrays because pointers allow dynamic allocation

} TagData;


TagData* create_tag_data(); //returns a pointer to TagData structure


void free_tag_data(TagData *data);

#endif 
