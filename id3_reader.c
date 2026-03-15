
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id3_utils.h"
#include "id3_reader.h"
#include "error_handling.h"


TagData* read_id3_tags(const char *filename) {

    //function returns a pointer to the metadata strcture
    
    FILE *fp;
    TagData *data; //pointer to structure that will store all metadata
    char header[10]; /*id3 header is always 10 bytes

    ID3 header
        3 bytes  identifier
        2 bytes  version
        1 byte   flags
        4 bytes  tag size
    */

    fp=fopen(filename,"rb");

    if(fp==NULL){
        //file doesnt exist or permission denied or wrong path
        display_error("Unable to open file");
        return NULL;
    }

    if(!check_id3_tag_presence(filename)){
        display_error("ID3 tag not found");
        fclose(fp);
        return NULL;
    }

    fread(header,1,10,fp); //read 10 bytes from file

    data=create_tag_data(); //allocates memory for metadata
    if(!data){ //if memory allocation failed
        fclose(fp);
        return NULL;
    }

    data->version=(char *)malloc(8); //allocate memory for version string, example: ID3v2.3 -> 7 characters + null terminator
    sprintf(data->version,"ID3v2.%d.%d",header[3],header[4]);
    /* header[3] contains version number. example 03->version 2.3 thus ID3v2.3, edit*: enhanced for version like 2.3.4 using header[4] , check more about sprintf and why these two lines are beautiful
    //we can also use strdup which is often a better way
    
    */

    

    while(1){ //this loop runs only when there are 4 bytes that represent frame id, else, it wont enter in, also, we read frames inside this loop, each frame stores one piece of metadata like TIT2->title TPE1 artist etc

        char frame_id[5] = {0}; //frame ids are 4 characters like TIT2 TPE1 etc and then a null terminator

        if(fread(frame_id,1,4,fp) != 4)
            break;
        
        int frame_size; //number of bytes that contains frame metadata
        char *frame_data; //pointer that will hold the actual text data

    
        if(frame_id[0]==0)
            break; //if framme id starts with 0, metadata section ended ie there are no more frames like TIT2 etc 

        /* we are not gonna make use of this below written fread because ID3 stores integers in big endian while x86 cpus reads integers in little endian , so bytes get interpreted backwards

        example
        Example frame size inside the MP3 file: 00 00 00 15
        Meaning: 21 bytes
        But your CPU interprets it as: 15 00 00 00 which is a huge number
        
        fread(&frame_size,4,1,fp); /*for reading 4 size bytes NOTE: we have used &frame_size because prototype of fread is
        size_t fread(void *ptr, size_t size, size_t count, FILE *stream); and it expects a pointer or rather fread needs an address where it can put the bytes it reads, so &frame_size gives the address
        */

        unsigned char size_bytes[4];
        fread(size_bytes,1,4,fp); //this will store the size in big endian format and we need to reconstruct to store in little endian which cpu can understand properly

        

        frame_size=(size_bytes[0] << 24) |
             (size_bytes[1] << 16) |
             (size_bytes[2] << 8)  |
             (size_bytes[3]);



             /*explanation : we are manually reconstructing it, 
             Example : 00 00 00 15
            Becomes: 0<<24 | 0<<16 | 0<<8 | 21
            Result: frame_size = 21
*/

        if(frame_size > 100000)
        {
            fseek(fp, frame_size + 2, SEEK_CUR);
            continue;
        }
        if(frame_size <= 0)
            {
                fseek(fp, 2, SEEK_CUR);  // skip flag bytes
                continue;
            }
       
        fseek(fp,2,SEEK_CUR); //frame header contains 2 flag bytes and we skip them

        frame_data=(char *)malloc(frame_size+1);//allocate memory for frame data and then an extra byte for null terminator

        fread(frame_data,1,frame_size,fp); //read metadata string

        frame_data[frame_size]='\0'; //for null terminating

        if(strcmp(frame_id,"TIT2")==0){
            //i f the frame is a title, 
            data->title=strdup(frame_data+1);
            /*
            why not data->title=frame_data?
            the data inside id3v2 frames usually start with first byte as a text encoding 
            example 
            [encoding byte][actual text]
            Example in memory  00 42 65 6C 69 65 76 65 72
            00 → encoding (ISO-8859-1)
            42 → B
            65 → e
            6C → l
            69 → i
            65 → e
            76 → v
            65 → e
            72 → r
            so if we copy directly we would have string like "\0Believer" which is wrong, so by writing frame+1 we skip the encoding byte, so now framedata+1 points directly to "Believer" 

        ALSO IMPORTANT : 
            if we had written data->title = frame_data; and later when we free(frame_data); both data->title and frame_data point to the same memory and both would be freed and now data->title points to an invalid memory A DANGLING POINTER and this later causes segmentation fault and undefined behaviour
            
            so we make use of strdup;
            its used to 1) allocate memory 2) copy string

            so this
            data->title = strdup(frame_data + 1);
            creates a new independent string that is in a different memory like 

            frame_data  -----> "Believer"
            data->title -----> "Believer"

            instead of 

            frame_data AND data->title ------>"Believer" in case of data->title=frame_data            */
        }
        //similarly
        else if (strcmp(frame_id, "TPE1") == 0)
        {
            data->artist = strdup(frame_data + 1);
        }
        else if (strcmp(frame_id, "TALB") == 0)
        {
            data->album = strdup(frame_data + 1);
        }
        else if (strcmp(frame_id, "TYER") == 0|| strcmp(frame_id,"TDRC")==0)
        {
            data->year = strdup(frame_data + 1);
        }
        else if (strcmp(frame_id, "TCON") == 0)
        {
            data->genre = strdup(frame_data + 1);
        }

        free(frame_data);

    }

    fclose(fp);
    return data;

}





void view_tags(const char *filename) {
    TagData *data = read_id3_tags(filename);
    if (!data) {
        display_error("Failed to read ID3 tags.");
        return;
    }
    display_metadata(data);
    free_tag_data(data);
}


void display_metadata(const TagData *data){

    //const because we are not gonna modify it

    if(data==NULL){
        display_error("No metadata available");
        return;
    }

    printf("\nMp3 TAG DETAILS :)\n");

    printf("Version : %s\n", data->version ? data->version : "Unknown");
    printf("Title   : %s\n", data->title ? data->title : "Unknown");
    printf("Artist  : %s\n", data->artist ? data->artist : "Unknown");
    printf("Album   : %s\n", data->album ? data->album : "Unknown");
    printf("Year    : %s\n", data->year ? data->year : "Unknown");
    printf("Genre   : %s\n", data->genre ? data->genre : "Unknown");

    printf(":):):):):):):):):):):):):)\n\n");
    
}