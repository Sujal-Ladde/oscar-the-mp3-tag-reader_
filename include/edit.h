
#ifndef EDIT_H
#define EDIT_H

#include "common.h"

int check_tag(FILE *mp3, const char *tag);
// Checks if a specific ID3v2 tag exists in the MP3 file and returns its offset.

Status edit_tags(FILE *mp3, FILE *new_mp3, const char *tag_name, const char *data, const char *file_name);
// Edits an existing ID3v2 tag or adds it if it's missing.

int final_header_offset(FILE *mp3);
// Determines the file offset marking the end of the ID3v2 header.

Status change_Header_size(FILE *mp3, FILE *new_mp3, int final_offset);
// Updates the size field within the ID3v2 header of the new MP3 file.

Status replace_file(const char *file_name); // Replaces the original file with the contents of the temporary "new.mp3" file.

Status replace_image(FILE *mp3, FILE *new_mp3, FILE *img, const char *MIME, const char *image_name, const char *file_name);
// Replaces the embedded picture within the ID3v2 tags of an MP3 file.

Status add_tag(FILE *mp3, FILE *new_mp3, const char *tag_name, const char *data, const char *file_name);
// Adds a new ID3v2 tag to the MP3 file.

Status add_image(FILE *mp3, FILE *new_mp3, FILE *img, const char *MIME, const char *image_name, const char *file_name);
// Adds a new image to the ID3v2 tags of an MP3 file.

#endif