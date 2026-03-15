
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id3_writer.h"
#include "id3_reader.h"
#include "id3_utils.h"


int edit_tag(const char *filename, const char *tag, const char *value){

    /*const params because we are collecting the data from the user and not gonna make changes to them, we are only gonna edit the struct data
    filename → MP3 file
    tag  → which field to modify
    value → new value
    
    Example command ./mp3tagreader -e -t song.mp3 "Believer"
        filename = "song.mp3"
        tag = "-t"
        value = "Believer"*/

    TagData *data;

    data=read_id3_tags(filename);

    if(data==NULL){

        return 1;
    }

    if(strcmp(tag,"-t")==0){
        /*so basically what we are doing here is freeing the already existing title value and initially it was like data->title---->"believer" this data->title was pointing to a memory which stored that believer string, so now we are freeing it and it now points to nothing data->title ----> ?
        and then we are now making it point to another memory which contains a string entered by the user, 
        
        for more brief on strdup, visit reader.c*/
        free(data->title);
        data->title=strdup(value);
    }

    else if (strcmp(tag, "-a") == 0)
    {
        free(data->artist);
        data->artist = strdup(value);
    }
    else if (strcmp(tag, "-A") == 0)
    {
        free(data->album);
        data->album = strdup(value);
    }
    else if (strcmp(tag, "-y") == 0)
    {
        free(data->year);
        data->year = strdup(value);
    }
    else if (strcmp(tag, "-g") == 0)
    {
        free(data->genre);
        data->genre = strdup(value);
    }

    else{
        printf("Invalid tag option\n");
        free_tag_data(data);
        return 1;
    }

    //write updated metadata back to the file using below write_id3_tags function

    if(write_id3_tags(filename,data)!=0){
        //if the writing back was not successful, free the data 
        free_tag_data(data);
        return 1;
    }

    free_tag_data(data);
    return 0;
}


/*
int write_id3_tags(const char *filename, const TagData *data){

    // const because we are not gonna make changes to the parameters passed, we are only gonna read them and utilize them, no modifications

    FILE *fp;
    FILE *temp;//for the temporary mp3 file which later we will be renaming to the original file and we will discard the original one

    char buffer[1024];
    size_t bytes; // stores how many bytes were read during each iteration of looping

    fp=fopen(filename,"rb");
    if(fp==NULL){ //maybe file doesnt exist, wrong path etc
        printf("Error opening file\n");
        return 1;
    }

    temp=fopen("temp.mp3","wb");
    if(temp==NULL){
        //if we cannot open the temp file, we will CLOSE THE ORIGINAL one aswell
        fclose(fp);
        printf("Error creating temporary file\n");
        return 1;
    }

    while((bytes=fread(buffer,1,sizeof(buffer),fp))>0){ //this loop copies original to temp

        fwrite(buffer,1,bytes,temp);

    }

    fclose(fp);
    fclose(temp);

    remove(filename);
    rename("temp.mp3", filename);

    return 0;
}

 this function takes in TagData structure which now contains the updated values. we will be creating a temporary mp3 file and write all the updated details + audio data from the old mp3 and then we rename it -> correction suggested by poovarasan sir

[ID3header + frames][Mp3 audio data]

we have modified the meta data section and thus we create a temp file and rename it again*/


/* this above version of function only copies the file byte for byte: nothing has changed, unchanged values are copied from original to temp and again renamed to original.

while((bytes = fread(buffer,1,sizeof(buffer),fp)) > 0)
{
    fwrite(buffer,1,bytes,temp);
}

That loop simply copies the file byte-for-byte.

So even though edit_tag() modifies the TagData structure in memory, that structure is never used to rewrite the frames in the file. The file copy operation ignores it completely.

edit_tag()
   ↓
read_id3_tags()
   ↓
modify struct
   ↓
write_id3_tags()
       ↓
       copy original file unchanged
       ↓
       rename temp → original
       
    what we have right now is 
    RAM (TagData struct)   → contains new title
    MP3 file on disk       → still contains old title
    
    we have to change completely*/


//read the above docs fully MUST
// so proper version of write_id3_tags is

int write_id3_tags(const char *filename, const TagData *data){

    FILE *fp = fopen(filename,"rb");
    FILE *temp = fopen("temp.mp3","wb");

    if(!fp || !temp)
        return 1;

    char header[10]; //allocating buffer for header
    fread(header,1,10,fp); //reading header from original
    fwrite(header,1,10,temp); //writing id3 header to temp

    while(1){//break out when frames end

        char frame_id[5] = {0};
        unsigned char size_bytes[4]; //frame size is stored as 4 bytes, if you are curious, we use unsigned char because we want the size in byte stream like 00 00 00 15 instead of 21 bytes
        int frame_size;// this will store the actual frame size 21 after we convert the byte stream

        if(fread(frame_id,1,4,fp)!=4)
            break; //if we cannot read 4 bytes of frame_id, we have reached end of meta data section since there is no meta data left cuz we know that metadata starts with frame id and we exit the loop

        if(frame_id[0]==0)
            break; //we also add this check because some mp3 files are padded with zeros, and this stops parsing correctly

        fread(size_bytes,1,4,fp); //read the next 4 bytes containing frame size, but this will be in big endian and we need to reconstruct manually

        //reconstruction
        frame_size = (size_bytes[0]<<24) |
             (size_bytes[1]<<16) |
             (size_bytes[2]<<8)  |
             (size_bytes[3]);
             /*This converts the 4 bytes into an integer.
            00 00 00 15
            0<<24
            0<<16
            0<<8
            21 
            frame_size = 21*/

            if(frame_size <= 0)
            {
                fseek(fp, 2, SEEK_CUR);
                continue;
            }

        char flags[2];
        fread(flags,1,2,fp); //frames contain 2 flag bytes 

        char *frame_data = malloc(frame_size+1); //allocating buffer for reading frame data ALONG WITH THE ENCODING BYTE 00 Believer where 00 → encoding byte Believer → actual title
        fread(frame_data,1,frame_size,fp);
        frame_data[frame_size] = '\0';// 

        //checking if frame is title and we have a new title to write or not
        if(strcmp(frame_id,"TIT2")==0 && data->title){

            int new_size=strlen(data->title)+1;  //+1 accounts for encoding byte cuz frame structure requires [encoding byte][text]

            unsigned char new_size_bytes[4] = {
                    (new_size>>24)&0xFF,
                    (new_size>>16)&0xFF,
                    (new_size>>8)&0xFF,
                    new_size&0xFF
            };//we convert the little endian back to big endian

            fwrite("TIT2",1,4,temp);// write new frame id
            fwrite(new_size_bytes,1,4,temp);//write new frame size
            fwrite(flags,1,2,temp);//write the original flags

            char encoding = 0;
            fwrite(&encoding,1,1,temp); //writing the encoding byte
            fwrite(data->title,1,strlen(data->title),temp);//write the new title string
        }

        //checking if frame is artist
        else if(strcmp(frame_id,"TPE1")==0 && data->artist){

            int new_size=strlen(data->artist)+1;

            unsigned char new_size_bytes[4] = {
                    (new_size>>24)&0xFF,
                    (new_size>>16)&0xFF,
                    (new_size>>8)&0xFF,
                    new_size&0xFF
            };

            fwrite("TPE1",1,4,temp);
            fwrite(new_size_bytes,1,4,temp);
            fwrite(flags,1,2,temp);

            char encoding = 0;
            fwrite(&encoding,1,1,temp);
            fwrite(data->artist,1,strlen(data->artist),temp);
        }

        //checking if frame is album
        else if(strcmp(frame_id,"TALB")==0 && data->album){

            int new_size=strlen(data->album)+1;

            unsigned char new_size_bytes[4] = {
                    (new_size>>24)&0xFF,
                    (new_size>>16)&0xFF,
                    (new_size>>8)&0xFF,
                    new_size&0xFF
            };

            fwrite("TALB",1,4,temp);
            fwrite(new_size_bytes,1,4,temp);
            fwrite(flags,1,2,temp);

            char encoding = 0;
            fwrite(&encoding,1,1,temp);
            fwrite(data->album,1,strlen(data->album),temp);
        }

        //checking if frame is year
        else if((strcmp(frame_id,"TYER")==0 || strcmp(frame_id,"TDRC")==0) && data->year){

            int new_size=strlen(data->year)+1;

            unsigned char new_size_bytes[4] = {
                    (new_size>>24)&0xFF,
                    (new_size>>16)&0xFF,
                    (new_size>>8)&0xFF,
                    new_size&0xFF
            };

            fwrite("TYER",1,4,temp);
            fwrite(new_size_bytes,1,4,temp);
            fwrite(flags,1,2,temp);

            char encoding = 0;
            fwrite(&encoding,1,1,temp);
            fwrite(data->year,1,strlen(data->year),temp);
        }

        //checking if frame is genre
        else if(strcmp(frame_id,"TCON")==0 && data->genre){

            int new_size=strlen(data->genre)+1;

            unsigned char new_size_bytes[4] = {
                    (new_size>>24)&0xFF,
                    (new_size>>16)&0xFF,
                    (new_size>>8)&0xFF,
                    new_size&0xFF
            };

            fwrite("TCON",1,4,temp);
            fwrite(new_size_bytes,1,4,temp);
            fwrite(flags,1,2,temp);

            char encoding = 0;
            fwrite(&encoding,1,1,temp);
            fwrite(data->genre,1,strlen(data->genre),temp);
        }

        //if we are not editing the title frame, other frames must remain the same unchanged, 
        else
        {
            //if we are not editing the title frame, other frames must remain the same unchanged, so frames like TPE1 TALB etc will remain the same
            fwrite(frame_id,1,4,temp);
            fwrite(size_bytes,1,4,temp);
            fwrite(flags,1,2,temp);
            fwrite(frame_data,1,frame_size,temp);
        }

        free(frame_data);
    }

    char buffer[1024]; //buffer for copying audio data after metadata section gets over
    size_t bytes;

    //ive written how these work in the above doc of the same function
    while((bytes=fread(buffer,1,1024,fp))>0)
        fwrite(buffer,1,bytes,temp);

    fclose(fp);
    fclose(temp);

    remove(filename);
    rename("temp.mp3",filename);

    return 0;
}
