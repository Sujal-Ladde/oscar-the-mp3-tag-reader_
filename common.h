#ifndef COMMOM_H


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
    e_success,
    e_failure
} Status;


Status is_valid_tag(const char *tag);
Status is_valid_file(FILE * mp3);
unsigned int id3v2_tag_size(unsigned char size_bytes[4]);



#endif