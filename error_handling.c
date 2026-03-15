#include <stdio.h>
#include <string.h>
#include "error_handling.h"

void display_error(const char *message)
//const means the function will not modify the message 
{
    fprintf(stderr,"Error: %s\n", message); /*we use stderr because errors should go to error stream, not normal output, unlike printf, fprintf is formatted print function and allows us to choose where the output goes

    in unix systems:
        stdin   → input
        stdout  → normal output
        stderr  → error output
    */

}

int check_id3_tag_presence(const char *filename){

    /*function returns int type
    1->tag present
    0-> no tag*/

    FILE *fp;
    char header[3]; // we will store first 3 bytes of file here exmple: ID3

    fp=fopen(filename,"rb"); //binary mode is important for mp3 files

    if(fp==NULL){

        return 0; //return 0 means no id3 detected bu technically couldnt open file
    }

    fread(header,1,3,fp); 
    /*
    fread(buffer, size, count, file)
        header[0] = 'I'
        header[1] = 'D'
        header[2] = '3'*/ 

    fclose(fp);

    if(strncmp(header,"ID3",3)==0){
        /*int strncmp(const char *str1, const char *str2, size_t n);
        n: The maximum number of characters to compare.
        returns:
        0 if first n characters of both are identical
        >0 if str1 is greater than str2 for a corresponding character
        <0 vice versa
        */
        return 1;
    }

    return 0;
}