#ifndef VIEW_H
#define VIEW_H

#include "common.h"

Status display_deets(FILE *mp3);
// Displays detailed information about all ID3v2 tags in an MP3 file.

Status display_tag(FILE *mp3);
// Displays the content of a single ID3v2 tag from the current file position.

Status read_one_tag(FILE *mp3, const char *tag);
// Searches for and displays the content of a specific ID3v2 tag.

Status read_apic(FILE *mp3, const char *output_path);
// Reads and extracts the data of an APIC (Attached Picture) ID3v2 tag.

#endif