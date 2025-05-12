#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define NUM_TAGS 84

#define MAX_PATH_LENGTH 256 // Define a maximum length for file paths

#define IMAGE_INPUT_PATH  "data/image_input/"
#define IMAGE_OUTPUT_PATH "data/image_output/"
#define MP3_FILES_PATH    "data/mp3_files/"


typedef struct
{
    char tag[5];
    char description[100];
} ID3TagMapping;

extern ID3TagMapping tagMappings[NUM_TAGS];

typedef enum
{
    e_success,
    e_failure
} Status;

Status is_valid_tag_W_index(const char *tag, int *tag_index);
// Checks if a tag is valid and returns its index.

Status is_valid_tag(const char *tag);
// Checks if a tag is a valid ID3v2 tag.

Status is_valid_file(FILE *mp3);
// Checks if a file is a valid ID3v2 tagged MP3 file.

unsigned int id3v2_tag_size(const unsigned char *data);
// Extracts the size of an ID3v2 tag frame.

unsigned int id3v2_header_size(const unsigned char *data);
// Extracts the size of the ID3v2 header.

int flag_to_tag(char *flag);
// Converts a command-line flag to its tag index.

Status copy_remaining_bits(FILE *source, FILE *destination);
// Copies the remaining data from one file to another.

int size_of_the_file(FILE *file);
// Gets the size of a file in bytes.

char *is_valid_image(const char *image_name);
// Checks if an image filename has a valid extension and returns its MIME type.

char *convert_size(int);
// Converts an integer size to a 4-byte array (standard ID3v2 encoding).

int end_of_header(FILE *mp3);
// Determines the file offset marking the end of the ID3v2 header.

char *convert_header_size(int num);
// Converts an integer size to a 4-byte array (ID3v2 header size encoding).

#endif