#include "common.h"


ID3TagMapping tagMappings[NUM_TAGS] = {
    {"AENC", "Audio encryption"},
    {"APIC", "Attached picture"},
    {"ASPI", "Audio seek point index"},
    {"COMM", "Comments"},
    {"COMR", "Commercial frame"},
    {"ENCR", "Encryption method registration"},
    {"EQU2", "Equalization (2)"},
    {"ETCO", "Event timing codes"},
    {"GEOB", "General encapsulated object"},
    {"GRID", "Group identification registration"},
    {"LINK", "Linked information"},
    {"MCDI", "Music CD identifier"},
    {"MLLT", "MPEG location lookup table"},
    {"OWNE", "Ownership frame"},
    {"PRIV", "Private frame"},
    {"PCNT", "Play counter"},
    {"POPM", "Popularimeter"},
    {"POSS", "Position synchronisation frame"},
    {"RBUF", "Recommended buffer size"},
    {"RVA2", "Relative volume adjustment (2)"},
    {"RVRB", "Reverb"},
    {"SEEK", "Seek frame"},
    {"SIGN", "Signature frame"},
    {"SYLT", "Synchronized lyric/text"},
    {"SYTC", "Synchronized tempo codes"},
    {"TALB", "Album/Movie/Show title"},
    {"TBPM", "BPM (beats per minute)"},
    {"TCOM", "Composer"},
    {"TCON", "Content type"},
    {"TCOP", "Copyright message"},
    {"TDEN", "Encoding time"},
    {"TDLY", "Playlist delay"},
    {"TDOR", "Original release time"},
    {"TDRC", "Recording time"},
    {"TDRL", "Release time"},
    {"TDTG", "Tagging time"},
    {"TENC", "Encoded by"},
    {"TEXT", "Lyricist/Text writer"},
    {"TFLT", "File type"},
    {"TIPL", "Involved people list"},
    {"TIT1", "Content group description"},
    {"TIT2", "Title/songname/content description"},
    {"TIT3", "Subtitle/Description refinement"},
    {"TKEY", "Initial key"},
    {"TLAN", "Language(s)"},
    {"TLEN", "Length"},
    {"TMCL", "Musician credits list"},
    {"TMED", "Media type"},
    {"TMOO", "Mood"},
    {"TOAL", "Original album/movie/show title"},
    {"TOFN", "Original filename"},
    {"TOLY", "Original lyricist(s)/text writer(s)"},
    {"TOPE", "Original artist(s)/performer(s)"},
    {"TOWN", "File owner/licensee"},
    {"TPE1", "Lead performer(s)/Soloist(s)"},
    {"TPE2", "Band/orchestra/accompaniment"},
    {"TPE3", "Conductor/performer refinement"},
    {"TPE4", "Interpreted, remixed, or otherwise modified by"},
    {"TPOS", "Part of a set"},
    {"TPRO", "Produced notice"},
    {"TPUB", "Publisher"},
    {"TRCK", "Track number/Position in set"},
    {"TRSN", "Internet radio station name"},
    {"TRSO", "Internet radio station owner"},
    {"TSOA", "Album sort order"},
    {"TSOP", "Performer sort order"},
    {"TSOT", "Title sort order"},
    {"TSRC", "ISRC (international standard recording code)"},
    {"TSSE", "Software/Hardware and settings used for encoding"},
    {"TSST", "Set subtitle"},
    {"TYER", "Year"},
    {"TXXX", "User defined text information frame"},
    {"UFID", "Unique file identifier"},
    {"USER", "Terms of use"},
    {"USLT", "Unsynchronized lyric/text transcription"},
    {"WCOM", "Commercial information"},
    {"WCOP", "Copyright/Legal information"},
    {"WOAF", "Official audio file webpage"},
    {"WOAR", "Official artist/performer webpage"},
    {"WOAS", "Official audio source webpage"},
    {"WORS", "Official internet radio station homepage"},
    {"WPAY", "Payment"},
    {"WPUB", "Publishers official webpage"},
    {"WXXX", "User defined URL link frame"}};

char valid_MIME[4][3] = {"jpg", "png", "bmp", "gif"};

/**
 * Checks if a given tag is a valid ID3v2 tag and returns its index.
 *
 * @param tag The 4-byte tag name to check.
 * @param tag_index Pointer to an integer where the tag's index will be stored.
 * @return e_success if the tag is valid, e_failure otherwise.
 *
 * @logic
 * 1. Iterate through the `tagMappings` array.
 * 2. Compare the input `tag` with each tag in the array.
 * 3. If a match is found, store the index and return e_success.
 * 4. If no match is found after iterating through all mappings, return e_failure.
 */
Status is_valid_tag_W_index(const char *tag, int *tag_index)
{
    for (int i = 0; i < 83; i++)
    {
        if (strncmp(tag, tagMappings[i].tag, 4) == 0)
        {
            *tag_index = i;
            return e_success;
        }
    }
    return e_failure;
}

/**
 * Checks if a given tag is a valid ID3v2 tag.
 *
 * @param tag The 4-byte tag name to check.
 * @return e_success if the tag is valid, e_failure otherwise.
 *
 * @logic
 * 1. Iterate through the `tagMappings` array.
 * 2. Compare the input `tag` with each tag in the array.
 * 3. If a match is found, return e_success.
 * 4. If no match is found after iterating through all mappings, return e_failure.
 */
Status is_valid_tag(const char *tag)
{
    for (int i = 0; i < 83; i++)
    {
        if (strncmp(tag, tagMappings[i].tag, 4) == 0)
        {
            return e_success;
        }
    }
    return e_failure;
}

/**
 * Converts a 4-byte tag name to its corresponding index in the tag mappings.
 *
 * @param tag The 4-byte tag name.
 * @return The index of the tag in `tagMappings`, or 0 if not found.
 *
 * @logic
 * 1. Iterate through the `tagMappings` array.
 * 2. Compare the input `tag` with each tag in the array.
 * 3. If a match is found, return the index.
 * 4. If no match is found, return 0 (the index of the first tag).
 */
int flag_to_tag(char *tag)
{
    for (int i = 0; i < NUM_TAGS; i++)
    {
        if (strcmp(tag, tagMappings[i].tag) == 0)
        {
            return i;
        }
    }

    return 0;
}

/**
 * Checks if a given file is a valid ID3v2 tagged MP3 file.
 *
 * @param mp3 File pointer to the MP3 file.
 * @return e_success if it's a valid ID3v2 file, e_failure otherwise.
 *
 * @logic
 * 1. Seek to the beginning of the file.
 * 2. Read the first 3 bytes.
 * 3. Compare these bytes with the "ID3" identifier.
 * 4. Return e_success if they match, e_failure otherwise.
 */
Status is_valid_file(FILE *mp3)
{
    fseek(mp3, 0, SEEK_SET);
    char file_type[4];
    fread(file_type, 1, 3, mp3);
    if (strcmp("ID3", file_type))
    {
        return e_failure;
    }
    return e_success;
}

/**
 * Checks if a given image filename has a valid image extension.
 *
 * @param image_name The filename of the image.
 * @return A pointer to the MIME type string if valid, NULL otherwise.
 *
 * @logic
 * 1. Extract the file extension from the image name.
 * 2. Compare the extracted extension with a list of valid image extensions ("jpg", "png", "bmp", "gif").
 * 3. If a match is found, allocate memory and return the corresponding MIME type ("image/").
 * 4. If no match is found, return NULL.
 */
char *is_valid_image(const char *image_name)
{

    char *MIME = (char *)malloc(4 * sizeof(char));
    int image_name_len = strlen(image_name);
    for (int i = image_name_len - 3, j = 0; i <= image_name_len; i++, j++)
    {
        MIME[j] = image_name[i];
    }

    for (int i = 0; i < 4; i++)
    {
        if (strncmp(valid_MIME[i], MIME, 3) == 0)
            return MIME;
    }
    return NULL;
}

/**
 * Copies the remaining data from a source file to a destination file.
 *
 * @param source File pointer to the source file.
 * @param destination File pointer to the destination file.
 * @return e_success on successful copy, e_failure on error.
 *
 * @logic
 * 1. Read one byte at a time from the source file.
 * 2. Write the read byte to the destination file.
 * 3. Continue until the end of the source file is reached.
 */
Status copy_remaining_bits(FILE *source, FILE *destination)
{
    uint8_t temp;
    while (fread(&temp, 1, 1, source) > 0)
    {
        fwrite(&temp, 1, 1, destination);
    }
    return e_success;
}

/**
 * Extracts the size of an ID3v2 tag frame from its 4-byte size field.
 * (Note: This function assumes the standard ID3v2 size encoding).
 *
 * @param data Pointer to the 4-byte size field of the tag frame.
 * @return The size of the tag frame as an unsigned integer.
 *
 * @logic
 * 1. Performs bitwise left shifts and OR operations to combine the four bytes into a 32-bit integer.
 * 2. The bytes are interpreted in big-endian order.
 */
unsigned int id3v2_tag_size(const unsigned char *data)
{
    return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

/**
 * Extracts the size of the ID3v2 header from its 4-byte size field.
 * (Note: The header size encoding has 7 bits per byte, with the highest bit being 0).
 *
 * @param data Pointer to the 4-byte size field of the ID3v2 header.
 * @return The size of the ID3v2 header as an unsigned integer.
 *
 * @logic
 * 1. Performs bitwise left shifts and OR operations to combine the four bytes.
 * 2. Only the lower 7 bits of each byte are considered.
 */
unsigned int id3v2_header_size(const unsigned char *data)
{
    return (data[0] << 21) | (data[1] << 14) | (data[2] << 7) | data[3];
}

/**
 * Gets the size of a file in bytes.
 *
 * @param file File pointer to the file.
 * @return The size of the file in bytes, or -1 on error.
 *
 * @logic
 * 1. Seek to the end of the file.
 * 2. Get the current file position (which is the size).
 * 3. Seek back to the beginning of the file.
 */
int size_of_the_file(FILE *file)
{
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}

/**
 * Converts an integer size to a 4-byte array (standard ID3v2 size encoding).
 *
 * @param num The integer size to convert.
 * @return A pointer to a dynamically allocated 4-byte array representing the size.
 * The caller is responsible for freeing this memory. Returns NULL on error.
 *
 * @logic
 * 1. Allocate 4 bytes of memory.
 * 2. Extract each byte of the integer using bitwise right shifts and AND operations.
 * 3. Store the bytes in big-endian order in the allocated array.
 */
char *convert_size(int num)
{
    unsigned char *bytes = (uint8_t *)malloc(4 * sizeof(char));
    bytes[0] = (num >> 24) & 0xFF;
    bytes[1] = (num >> 16) & 0xFF;
    bytes[2] = (num >> 8) & 0xFF;
    bytes[3] = num & 0xFF;

    return bytes;
}

/**
 * Converts an integer size to a 4-byte array (ID3v2 header size encoding - 7 bits per byte).
 *
 * @param num The integer size to convert.
 * @return A pointer to a dynamically allocated 4-byte array representing the encoded size.
 * The caller is responsible for freeing this memory. Returns NULL on error.
 *
 * @logic
 * 1. Allocate 4 bytes of memory.
 * 2. Extract 7 bits at a time from the integer using bitwise right shifts and AND operations.
 * 3. Store the 7-bit values in the allocated array.
 */
char *convert_header_size(int num)
{
    unsigned char *bytes = (uint8_t *)malloc(4 * sizeof(char));
    bytes[0] = (num >> 21) & 0x7F;
    bytes[1] = (num >> 14) & 0x7F;
    bytes[2] = (num >> 7) & 0x7F;
    bytes[3] = num & 0x7F;

    return bytes;
}

/**
 * Determines the file offset marking the end of the ID3v2 header.
 *
 * @param mp3 File pointer to the MP3 file.
 * @return The offset (in bytes) where the audio data begins.
 *
 * @logic
 * 1. Seek to the start of the tag data area (10 bytes from the beginning).
 * 2. Continuously read 4 bytes as a potential tag name.
 * 3. validate ID3v2 tag.
 * 4. If not a valid tag, the current file pointer position minus 4 is the end of the header.
 * 5. If it is a valid tag, read the 4-byte tag size.
 * 6. Skip the 2-byte flags and the tag data based on the read size.
 * 7. Repeat until an invalid tag is encountered.
 */
int end_of_header(FILE *mp3)
{
    fseek(mp3, 10, SEEK_SET);
    while (1)
    {
        char curent_tag[4];
        fread(curent_tag, 1, 4, mp3);
        if (is_valid_tag(curent_tag) == e_failure)
        {
            return ftell(mp3) - 4;
        }
        uint8_t Tag_size[4];
        fread(&Tag_size, 1, 4, mp3);
        int size = id3v2_tag_size(Tag_size);
        fseek(mp3, ftell(mp3) + 2 + size, SEEK_SET);
    }
}
