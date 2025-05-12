#include "edit.h"

/**
 * Checks for an ID3v2 tag and returns its offset.
 *
 * @param mp3 File pointer to the MP3 file (read binary mode).
 * @param tag 4-byte tag identifier to search for (e.g., "TIT2").
 * @return Offset of the tag if found, 0 otherwise (or on error).
 *
 * @logic
 * 1. Seek to the start of ID3v2 tags.
 * 2. Loop through tags:
 *     - Read tag identifier.
 *     - If invalid, break.
 *     - If it matches the target tag, return the offset.
 *     - Read tag size and skip the tag.
 * 3. Return 0 if the tag is not found.
 * 4. Includes error handling for file operations.
 */
int check_tag(FILE *mp3, const char *tag)

{
    fseek(mp3, 10, SEEK_SET);
    int flag = 0;
    while (1)
    {
        char curent_tag[4];
        fread(curent_tag, 1, 4, mp3);
        if (is_valid_tag(curent_tag) == e_failure)
        {
            break;
        }
        else if (strncmp(tag, curent_tag, 4) == 0)
        {
            flag = 1;
            fseek(mp3, ftell(mp3) - 4, SEEK_SET);
            fprintf(stdout, "LOG: Found the offset of the tag.\n");
            return ftell(mp3);
        }
        uint8_t Tag_size[4];
        fread(&Tag_size, 1, 4, mp3);
        int size = id3v2_tag_size(Tag_size);
        fseek(mp3, ftell(mp3) + 2 + size, SEEK_SET);
    }
    if (flag == 0)
    {
        return 0;
    }
}

/**
 * Edits an ID3v2 tag or adds it if missing.
 *
 * @param mp3 Original MP3 file (read binary).
 * @param new_mp3 Temporary MP3 file (write binary).
 * @param tag_name Tag to edit (e.g., "TIT2").
 * @param data New tag data.
 * @param file_name Original MP3 filename.
 * @return e_success if edited/added, e_failure on error.
 *
 * @logic
 * 1. Validate tag.
 * 2. Find tag offset.
 * 3. If not found, ask to create and call add_tag.
 * 4. If found:
 *     - Copy header till tag.
 *     - Write new tag data.
 *     - Copy remaining data.
 * 5. Replace original file.
 * 6. Update ID3v2 header size.
 * 7. Replace again.
 * 8. Return status.
 */
Status edit_tags(FILE *mp3, FILE *new_mp3, const char *tag_name, const char *data, const char *file_name)
{

    if (is_valid_tag(tag_name) == e_failure)
    {
        fprintf(stderr, "ERROR: Entered tag is invalid!\n");
        return e_failure;
    }

    uint8_t data_len = strlen(data);
    int offset = check_tag(mp3, tag_name);

    if (offset == 0)
    {
        fprintf(stderr, "WARNING: Entered tag not found, do you want to create one?\n");
        fprintf(stdout, "if Yes, Enter: 1 \n");
        fprintf(stdout, "else, Enter: 0 to exit\n");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            fprintf(stderr, "ERROR: Invalid input for choice.\n");
            return e_failure;
        }
        if (choice)
        {
            return add_tag(mp3, new_mp3, tag_name, data, file_name); // Corrected return
        }
        else
        {
            return e_failure;
        }
    }

    // Offset is valid, proceed with editing.
    char *header_till_tag = (char *)malloc(offset + 4); // Use malloc
    if (header_till_tag == NULL)
    {
        perror("malloc failed");
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }

    fseek(mp3, 0, SEEK_SET);
    if (fread(header_till_tag, 1, offset + 4, mp3) != offset + 4)
    {
        fprintf(stderr, "ERROR: Failed to read header.\n");
        free(header_till_tag);
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }
    if (fwrite(header_till_tag, 1, offset + 4, new_mp3) != offset + 4)
    {
        fprintf(stderr, "ERROR: Failed to write header.\n");
        free(header_till_tag);
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }
    free(header_till_tag); // Free allocated memory

    if (fseek(new_mp3, 3, SEEK_CUR) != 0)
    {
        perror("fseek failed");
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }

    if (fwrite(&data_len, 1, 1, new_mp3) != 1)
    {
        perror("fwrite failed");
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }

    if (fseek(new_mp3, 2, SEEK_CUR) != 0)
    {
        perror("fseek failed");
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }
    if (fwrite(data, 1, data_len, new_mp3) != data_len)
    {
        perror("fwrite failed");
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }

    uint8_t size_in_mp3[4];
    if (fread(size_in_mp3, 1, 4, mp3) != 4)
    {
        fprintf(stderr, "ERROR: Failed to read size.\n");
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }

    int size = id3v2_tag_size(size_in_mp3);
    if (fseek(mp3, 2 + size, SEEK_CUR) != 0)
    {
        perror("fseek failed");
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }

    if (copy_remaining_bits(mp3, new_mp3) == e_failure)
    {
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }

    fclose(mp3);
    fclose(new_mp3);

    if (replace_file(file_name) == e_failure)
        return e_failure;

    char mp3__file[MAX_PATH_LENGTH];
    strcpy(mp3__file, MP3_FILES_PATH);
    strcat(mp3__file, file_name);

    mp3 = fopen(mp3__file, "rb");
    if (mp3 == NULL)
    {
        perror("fopen failed");
        return e_failure;
    }

    char temp__mp3__file[MAX_PATH_LENGTH];
    strcpy(temp__mp3__file, MP3_FILES_PATH);
    strcat(temp__mp3__file, "new.mp3");

    new_mp3 = fopen(temp__mp3__file, "wb");
    if (new_mp3 == NULL)
    {
        perror("fopen failed");
        fclose(mp3);
        return e_failure;
    }

    int final_offset = final_header_offset(mp3);

    if (change_Header_size(mp3, new_mp3, final_offset) == e_failure)
    {
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }
    fclose(mp3);
    fclose(new_mp3);
    if (replace_file(file_name) == e_failure)
        return e_failure;

    fprintf(stdout, "LOG: successfully edited the %s tag with \"%s\".\n", tag_name, data);
    return e_success;
}

/**
 * Replaces the original file with the new one.
 *
 * @param file_name Name of the file to replace.
 * @return e_success if replaced, e_failure on error.
 *
 * @logic
 * 1. Delete the original file.
 * 2. Rename "new.mp3" to the original name.
 */
Status replace_file(const char *file_name)
{
    char mp3__file[MAX_PATH_LENGTH];
    strcpy(mp3__file, MP3_FILES_PATH);
    strcat(mp3__file, file_name);

    const char *old_file = mp3__file;
    remove(old_file);

    char temp__mp3__file[MAX_PATH_LENGTH];
    strcpy(temp__mp3__file, MP3_FILES_PATH);
    strcat(temp__mp3__file, "new.mp3");

    char old_name[256];
    strcpy(old_name, temp__mp3__file);
    const char *new_name = mp3__file;
    if (rename(old_name, new_name) != 0)
    {
        perror("rename failed");
        return e_failure;
    }
    fprintf(stdout, "LOG: Successfully replaced the old file with the new one\n");
    return e_success;
}

/**
 * Finds the offset of the end of the ID3v2 header.
 *
 * @param mp3 File pointer to the MP3 file.
 * @return Offset of the end of the ID3v2 header.
 *
 * @logic
 * 1. Seek to the start of the tags (after the 10-byte header).
 * 2. Loop through tags until an invalid tag is found.
 * 3. Return the file pointer position before the invalid tag.
 */
int final_header_offset(FILE *mp3)
{
    fseek(mp3, 10, SEEK_SET);

    while (1)
    {
        char tag[4];
        fread(tag, 1, 4, mp3);
        if (is_valid_tag(tag) == e_failure)
        {
            fseek(mp3, ftell(mp3) - 4, SEEK_SET);
            return ftell(mp3);
        }
        uint8_t tag_size[4];
        fread(tag_size, 4, 1, mp3);
        int size_tag = id3v2_tag_size(tag_size);

        fseek(mp3, ftell(mp3) + 2 + size_tag, SEEK_SET);
    }
}

/**
 * Updates the size of the Header field in the ID3v2 header of the new MP3 file.
 *
 * @param mp3 File pointer to the original MP3 file.
 * @param new_mp3 File pointer to the new MP3 file being written.
 * @param final_offset The offset indicating the end of the ID3v2 tags.
 * @return e_success if the size was updated, e_failure on error.
 *
 * @logic
 * 1. Seek to the beginning of both files.
 * 2. Read and write the initial 6 bytes of the header.
 * 3. Calculate the new header size (final_offset - 10).
 * 4. Convert the new size to the ID3v2 size encoding.
 * 5. Write the encoded size to the new file.
 * 6. Copy the rest of the header (excluding the original size).
 * 7. Copy the remaining audio data from the original file.
 */
Status change_Header_size(FILE *mp3, FILE *new_mp3, int final_offset)
{
    if (fseek(mp3, 0, SEEK_SET) != 0)
    {
        perror("fseek failed");
        return e_failure;
    }

    char header_start[6];
    if (fread(header_start, 1, 6, mp3) != 6)
    {
        perror("fread failed");
        return e_failure;
    }
    if (fwrite(header_start, 1, 6, new_mp3) != 6)
    {
        perror("fwrite failed");
        return e_failure;
    }

    int size = final_offset;
    char *encoded_size = convert_header_size(size);
    if (encoded_size == NULL)
    {
        fprintf(stderr, "convert_header_size failed\n");
        return e_failure;
    }
    if (fwrite(encoded_size, 1, 4, new_mp3) != 4)
    {
        perror("fwrite failed");
        free(encoded_size);
        return e_failure;
    }
    free(encoded_size);

    size_t header_size = final_offset - 10;
    char *header_rest = (char *)malloc(header_size);
    if (header_rest == NULL)
    {
        perror("malloc failed");
        return e_failure;
    }

    if (fseek(mp3, 10, SEEK_SET) != 0)
    {
        perror("fseek failed");
        free(header_rest);
        return e_failure;
    }
    if (fread(header_rest, 1, header_size, mp3) != header_size)
    {
        perror("fread failed");
        free(header_rest);
        return e_failure;
    }
    if (fwrite(header_rest, 1, header_size, new_mp3) != header_size)
    {
        perror("fwrite failed");
        free(header_rest);
        return e_failure;
    }
    free(header_rest);

    if (fseek(mp3, final_offset, SEEK_SET) != 0)
    {
        perror("fseek failed");
        return e_failure;
    }

    if (copy_remaining_bits(mp3, new_mp3) == e_failure)
    {
        return e_failure;
    }

    fprintf(stdout, "LOG: Successfully encoded the new header size.\n");
    return e_success;
}

/**
 * Replaces the embedded picture in an MP3 file.
 *
 * @param mp3 Original MP3 file (read binary).
 * @param new_mp3 Temporary MP3 file (write binary).
 * @param img Image file to embed (read binary).
 * @param MIME MIME type of the image (e.g., "jpeg", "png").
 * @param image_name Name of the image.
 * @param file_name Original MP3 filename.
 * @return e_success if replaced, e_failure on error.
 *
 * @logic
 * 1. Check if "APIC" tag exists. If not, ask to add.
 * 2. Copy header before "APIC" tag.
 * 3. Read image data.
 * 4. Construct new "APIC" tag with new image data.
 * 5. Write the new "APIC" tag to the new file.
 * 6. Skip the old "APIC" tag in the original file.
 * 7. Copy remaining data.
 * 8. Replace original file with the new one.
 * 9. Update the ID3v2 header size.
 * 10. Replace the file again.
 */
Status replace_image(FILE *mp3, FILE *new_mp3, FILE *img, const char *MIME, const char *image_name, const char *file_name)
{
    int image_offset = check_tag(mp3, "APIC");
    if (image_offset == 0)
    {
        fprintf(stderr, "WARNING: Image not found, do you want to add one?\n");
        fprintf(stdout, "if Yes, Enter: 1 \n");
        fprintf(stdout, "else, Enter: 0 to exit\n");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            perror("scanf failed");
            return e_failure;
        }
        if (choice)
        {
            if (add_image(mp3, new_mp3, img, MIME, image_name, file_name) == e_failure)
            {
                return e_failure;
            }
            return e_success;
        }
        else
            return e_failure;
    }
    if (fseek(mp3, 0, SEEK_SET) != 0)
    {
        perror("fseek failed");
        return e_failure;
    }
    int image_name_len = strlen(image_name);

    char header_till_tag[image_offset + 4];
    if (fread(header_till_tag, 1, image_offset + 4, mp3) != image_offset + 4)
    {
        perror("fread failed");
        return e_failure;
    }
    if (fwrite(header_till_tag, 1, image_offset + 4, new_mp3) != image_offset + 4)
    {
        perror("fwrite failed");
        return e_failure;
    }

    int size = size_of_the_file(img);
    char *image_data = (char *)malloc(size * sizeof(char));
    if (!image_data)
    {
        perror("malloc failed");
        return e_failure;
    }
    if (fread(image_data, size, 1, img) != 1)
    {
        perror("fread failed");
        free(image_data);
        return e_failure;
    }
    if (fseek(img, 0, SEEK_SET) != 0)
    {
        perror("fseek failed");
        free(image_data);
        return e_failure;
    }

    size += 12 + strlen(image_name) + 1;
    char *Encode_size = convert_size(size);
    if (!Encode_size)
    {
        perror("convert_size failed");
        free(image_data);
        return e_failure;
    }
    if (fwrite(Encode_size, 4, 1, new_mp3) != 1)
    {
        perror("fwrite failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    char old_size[4];
    if (fread(old_size, 4, 1, mp3) != 1)
    {
        perror("fread failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    if (fseek(mp3, 2, SEEK_CUR) != 0)
    {
        perror("fseek failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    if (fseek(new_mp3, 2, SEEK_CUR) != 0)
    {
        perror("fseek failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    int old_image_size = id3v2_tag_size(old_size);

    int frame_size_pos = ftell(mp3);

    char text_encoding;
    char MIME_type[10];
    char picture_type;
    char discription[100];

    if (fread(&text_encoding, 1, 1, mp3) != 1)
    {
        perror("fread failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    if (fwrite(&text_encoding, 1, 1, new_mp3) != 1)
    {
        perror("fwrite failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    int i = 0;
    while (1)
    {
        char x;
        if (fread(&x, 1, 1, mp3) != 1)
        {
            perror("fread failed");
            free(image_data);
            free(Encode_size);
            return e_failure;
        }
        if (x == '\0')
        {
            MIME_type[i] = x;
            break;
        }
        else
            MIME_type[i] = x;
        i++;
    }
    char new_MIME_type[10] = "image/";
    strcat(new_MIME_type, MIME);
    if (fwrite(new_MIME_type, 10, 1, new_mp3) != 1)
    {
        perror("fwrite failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    if (fread(&picture_type, 1, 1, mp3) != 1)
    {
        perror("fread failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    if (fwrite(&picture_type, 1, 1, new_mp3) != 1)
    {
        perror("fwrite failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    i = 0;
    while (1)
    {
        char x;
        if (fread(&x, 1, 1, mp3) != 1)
        {
            perror("fread failed");
            free(image_data);
            free(Encode_size);
            return e_failure;
        }
        if (x == '\0')
        {
            discription[i] = x;
            break;
        }
        else
            discription[i] = x;
        i++;
    }
    if (fwrite(image_name, image_name_len + 1, 1, new_mp3) != 1)
    {
        perror("fwrite failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    if (fwrite(image_data, size, 1, new_mp3) != 1)
    {
        perror("fwrite failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    int actual_size = old_image_size - (ftell(mp3) - frame_size_pos);
    printf("the actual size is %d\n", actual_size);

    if (fseek(mp3, ftell(mp3) + actual_size, SEEK_SET) != 0)
    {
        perror("fseek failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    if (copy_remaining_bits(mp3, new_mp3) == e_failure)
    {
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    free(image_data);

    if (fclose(mp3) != 0)
    {
        perror("fclose failed");
        free(Encode_size);
        return e_failure;
    }
    if (fclose(new_mp3) != 0)
    {
        perror("fclose failed");
        free(Encode_size);
        return e_failure;
    }

    if (replace_file(file_name) == e_failure)
    {
        free(Encode_size);
        return e_failure;
    }

    char mp3__file[MAX_PATH_LENGTH];
    strcpy(mp3__file, MP3_FILES_PATH);
    strcat(mp3__file, file_name);

    mp3 = fopen(mp3__file, "rb");
    if (!mp3)
    {
        perror("fopen failed");
        free(Encode_size);
        return e_failure;
    }

    char temp__mp3__file[MAX_PATH_LENGTH];
    strcpy(temp__mp3__file, MP3_FILES_PATH);
    strcat(temp__mp3__file, "new.mp3");

    new_mp3 = fopen(temp__mp3__file, "wb");
    if (!new_mp3)
    {
        perror("fopen failed");
        fclose(mp3);
        free(Encode_size);
        return e_failure;
    }
    int final_offset = final_header_offset(mp3);
    if (final_offset == -1)
    {
        fclose(mp3);
        fclose(new_mp3);
        free(Encode_size);
        return e_failure;
    }
    if (change_Header_size(mp3, new_mp3, final_offset) == e_failure)
    {
        fclose(mp3);
        fclose(new_mp3);
        free(Encode_size);
        return e_failure;
    }

    if (fclose(mp3) != 0)
    {
        perror("fclose failed");
        free(Encode_size);
        return e_failure;
    }
    if (fclose(new_mp3) != 0)
    {
        perror("fclose failed");
        free(Encode_size);
        return e_failure;
    }
    if (replace_file(file_name) == e_failure)
    {
        free(Encode_size);
        return e_failure;
    }
    free(Encode_size);
    return e_success;
}

/**
 * Adds a new ID3v2 tag to the MP3 file.
 *
 * @param mp3 Original MP3 file (read binary).
 * @param new_mp3 Temporary MP3 file (write binary).
 * @param tag_name 4-byte tag identifier to add (e.g., "TIT2").
 * @param data Data for the new tag.
 * @param file_name Original MP3 filename.
 * @return e_success if added, e_failure on error.
 *
 * @logic
 * 1. Copy the initial ID3v2 header to the new file.
 * 2. Copy existing tags (excluding APIC) to the new file.
 * 3. Write the new tag name and its encoded size.
 * 4. Write the new tag data.
 * 5. Copy the remaining audio data.
 * 6. Replace the original file.
 * 7. Update the ID3v2 header size.
 * 8. Replace the file again.
 */
Status add_tag(FILE *mp3, FILE *new_mp3, const char *tag_name, const char *data, const char *file_name)
{
    if (fseek(mp3, 0, SEEK_SET) != 0)
    {
        perror("fseek (mp3, 0, SEEK_SET) failed");
        return e_failure;
    }

    char first_6_bytes[6];
    if (fread(first_6_bytes, 1, 6, mp3) != 6)
    {
        perror("fread (first 6 bytes) failed");
        return e_failure;
    }
    if (fwrite(first_6_bytes, 1, 6, new_mp3) != 6)
    {
        perror("fwrite (first 6 bytes) failed");
        return e_failure;
    }

    char header_size[4];
    if (fread(header_size, 1, 4, mp3) != 4)
    {
        perror("fread (header size) failed");
        return e_failure;
    }
    if (fwrite(header_size, 1, 4, new_mp3) != 4)
    {
        perror("fwrite (header size) failed");
        return e_failure;
    }

    int Apic_offset = check_tag(mp3, "APIC");
    if (fseek(mp3, 10, SEEK_SET) != 0)
    {
        perror("fseek (mp3, 10, SEEK_SET) failed");
        return e_failure;
    }

    if (Apic_offset == 0)
    {
        int int_header_size = id3v2_header_size(header_size);
        char *old_header = (char *)malloc(int_header_size);
        if (!old_header)
        {
            perror("malloc (old_header) failed");
            return e_failure;
        }
        if (fread(old_header, 1, int_header_size, mp3) != int_header_size)
        {
            perror("fread (old_header) failed");
            free(old_header);
            return e_failure;
        }
        if (fwrite(old_header, 1, int_header_size, new_mp3) != int_header_size)
        {
            perror("fwrite (old_header) failed");
            free(old_header);
            return e_failure;
        }
        free(old_header);
    }
    else
    {
        char *old_header = (char *)malloc(Apic_offset - 10);
        if (!old_header)
        {
            perror("malloc (old_header) failed");
            return e_failure;
        }
        if (fread(old_header, 1, Apic_offset - 10, mp3) != Apic_offset - 10)
        {
            perror("fread (old_header) failed");
            free(old_header);
            return e_failure;
        }
        if (fwrite(old_header, 1, Apic_offset - 10, new_mp3) != Apic_offset - 10)
        {
            perror("fwrite (old_header) failed");
            free(old_header);
            return e_failure;
        }
        free(old_header);
    }

    if (fwrite(tag_name, 1, 4, new_mp3) != 4)
    {
        perror("fwrite (tag_name) failed");
        return e_failure;
    }

    size_t data_size = strlen(data);
    char *encoded_size = convert_size(data_size);
    if (!encoded_size)
    {
        perror("convert_size failed");
        return e_failure;
    }
    if (fwrite(encoded_size, 1, 4, new_mp3) != 4)
    {
        perror("fwrite (encoded_size) failed");
        free(encoded_size);
        return e_failure;
    }
    free(encoded_size);

    if (fseek(new_mp3, 2, SEEK_CUR) != 0)
    {
        perror("fseek (new_mp3, 2, SEEK_CUR) failed");
        return e_failure;
    }
    if (fwrite(data, 1, data_size, new_mp3) != data_size)
    {
        perror("fwrite (data) failed");
        return e_failure;
    }

    if (copy_remaining_bits(mp3, new_mp3) == e_failure)
    {
        perror("copy_remaining_bits failed");
        return e_failure;
    }

    if (fclose(mp3) != 0)
    {
        perror("fclose (mp3) failed");
        return e_failure;
    }
    if (fclose(new_mp3) != 0)
    {
        perror("fclose (new_mp3) failed");
        return e_failure;
    }

    if (replace_file(file_name) == e_failure)
        return e_failure;

    char mp3__file[MAX_PATH_LENGTH];
    strcpy(mp3__file, MP3_FILES_PATH);
    strcat(mp3__file, file_name);

    mp3 = fopen(mp3__file, "rb");
    if (!mp3)
    {
        perror("fopen (mp3) failed");
        return e_failure;
    }
    char temp__mp3__file[MAX_PATH_LENGTH];
    strcpy(temp__mp3__file, MP3_FILES_PATH);
    strcat(temp__mp3__file, "new.mp3");

    new_mp3 = fopen(temp__mp3__file, "wb");
    if (!new_mp3)
    {
        perror("fopen (new_mp3) failed");
        fclose(mp3);
        return e_failure;
    }
    int final_offset = final_header_offset(mp3);
    if (final_offset == -1)
    {
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }
    if (change_Header_size(mp3, new_mp3, final_offset) == e_failure)
    {
        fclose(mp3);
        fclose(new_mp3);
        return e_failure;
    }
    if (fclose(mp3) != 0)
    {
        perror("fclose (mp3) failed");
        return e_failure;
    }
    if (fclose(new_mp3) != 0)
    {
        perror("fclose (new_mp3) failed");
        return e_failure;
    }
    if (replace_file(file_name) == e_failure)
        return e_failure;

    fprintf(stdout, "LOG: successfully created the %s tag with \"%s\".\n", tag_name, data);
    return e_success;
}

/**
 * Adds a new image to the ID3v2 tags of an MP3 file.
 *
 * @param mp3 Original MP3 file (read binary).
 * @param new_mp3 Temporary MP3 file (write binary).
 * @param img Image file to add (read binary).
 * @param MIME MIME type of the image (e.g., "jpeg").
 * @param image_name Name of the image.
 * @param file_name Original MP3 filename.
 * @return e_success if added, e_failure on error.
 *
 * @logic
 * 1. Copy the existing ID3v2 header to the new file.
 * 2. Write the "APIC" tag identifier to the new file.
 * 3. Read the image data from the image file.
 * 4. Encode the size of the new "APIC" frame.
 * 5. Write the encoded size and flags to the new file.
 * 6. Write the text encoding, MIME type, picture type, description, and image data to the new file.
 * 7. Copy the remaining audio data from the original file to the new file.
 * 8. Replace the original MP3 file with the new one.
 * 9. Update the ID3v2 header size in the new file.
 * 10. Replace the original MP3 file again.
 */

Status add_image(FILE *mp3, FILE *new_mp3, FILE *img, const char *MIME, const char *image_name, const char *file_name)
{
    printf("the final offset of the header before adding is %d\n", final_header_offset(mp3));
    char *nulls[4] = {'\0', '\0', '\0', '\0'};
    if (fseek(mp3, 0, SEEK_SET) != 0)
    {
        perror("fseek (mp3, 0, SEEK_SET) failed");
        return e_failure;
    }
    char first_6_bytes[6];
    if (fread(first_6_bytes, 1, 6, mp3) != 6)
    {
        perror("fread (first 6 bytes) failed");
        return e_failure;
    }

    char header_size[4];
    if (fread(header_size, 1, 4, mp3) != 4)
    {
        perror("fread (header size) failed");
        return e_failure;
    }

    int int_header_size = end_of_header(mp3);
    if (fseek(mp3, 0, SEEK_SET) != 0)
    {
        perror("fseek (mp3, 0, SEEK_SET) failed");
        return e_failure;
    }
    char *old_header = (char *)malloc(int_header_size * sizeof(char));
    if (!old_header)
    {
        perror("malloc (old_header) failed");
        return e_failure;
    }
    if (fread(old_header, 1, int_header_size, mp3) != int_header_size)
    {
        perror("fread (old_header) failed");
        free(old_header);
        return e_failure;
    }
    if (fwrite(old_header, 1, int_header_size, new_mp3) != int_header_size)
    {
        perror("fwrite (old_header) failed");
        free(old_header);
        return e_failure;
    }
    free(old_header);

    char *apic_tag = "APIC";
    if (fwrite(apic_tag, 1, 4, new_mp3) != 4)
    {
        perror("fwrite (apic_tag) failed");
        return e_failure;
    }

    int size = size_of_the_file(img);
    char *image_data = (char *)malloc(size * sizeof(char));
    if (!image_data)
    {
        perror("malloc (image_data) failed");
        return e_failure;
    }
    if (fread(image_data, 1, size, img) != size)
    {
        perror("fread (image_data) failed");
        free(image_data);
        return e_failure;
    }
    if (fseek(img, 0, SEEK_SET) != 0)
    {
        perror("fseek (img, 0, SEEK_SET) failed");
        free(image_data);
        return e_failure;
    }

    size += strlen(MIME) + 1 + 1 + 1 + strlen(image_name) + 1;
    char *Encode_size = convert_size(size);
    if (!Encode_size)
    {
        perror("convert_size failed");
        free(image_data);
        return e_failure;
    }
    if (fwrite(Encode_size, 1, 4, new_mp3) != 4)
    {
        perror("fwrite (Encode_size) failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    printf("encode_size%d\n", size);
    if (fwrite(nulls, 1, 2, new_mp3) != 2)
    {
        perror("fwrite (nulls) failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    uint8_t text_encoding = 0;
    if (fwrite(&text_encoding, 1, 1, new_mp3) != 1)
    {
        perror("fwrite (text_encoding) failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    char new_MIME_type[10] = "image/";
    strcat(new_MIME_type, MIME);
    if (fwrite(new_MIME_type, 1, 10, new_mp3) != 10)
    {
        perror("fwrite (new_MIME_type) failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    uint8_t picture_type = 0;
    if (fwrite(&picture_type, 1, 1, new_mp3) != 1)
    {
        perror("fwrite (picture_type) failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    int image_name_len = strlen(image_name);
    if (fwrite(image_name, 1, image_name_len + 1, new_mp3) != image_name_len + 1)
    {
        perror("fwrite (image_name) failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    if (fwrite(image_data, 1, size_of_the_file(img), new_mp3) != size_of_the_file(img))
    {
        perror("fwrite (image_data) failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    int final_offset = ftell(new_mp3);

    if (copy_remaining_bits(mp3, new_mp3) == e_failure)
    {
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    if (fclose(mp3) != 0)
    {
        perror("fclose (mp3) failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    if (fclose(new_mp3) != 0)
    {
        perror("fclose (new_mp3) failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    if (replace_file(file_name) == e_failure)
    {
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    char mp3__file[MAX_PATH_LENGTH];
    strcpy(mp3__file, MP3_FILES_PATH);
    strcat(mp3__file, file_name);

    mp3 = fopen(mp3__file, "rb");
    if (!mp3)
    {
        perror("fopen (mp3) failed");
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    char temp__mp3__file[MAX_PATH_LENGTH];
    strcpy(temp__mp3__file, MP3_FILES_PATH);
    strcat(temp__mp3__file, "new.mp3");

    new_mp3 = fopen(temp__mp3__file, "wb");
    if (!new_mp3)
    {
        perror("fopen (new_mp3) failed");
        fclose(mp3);
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    printf("The final header offset is %d\n", final_offset);
    if (change_Header_size(mp3, new_mp3, final_offset) == e_failure)
    {
        fclose(mp3);
        fclose(new_mp3);
        free(image_data);
        free(Encode_size);
        return e_failure;
    }

    if (replace_file(file_name) == e_failure)
    {
        fclose(mp3);
        fclose(new_mp3);
        free(image_data);
        free(Encode_size);
        return e_failure;
    }
    free(image_data);
    free(Encode_size);
    fprintf(stdout, "LOG: successfully added the image \"%s\"to the \"%s\".\n", image_name, file_name);
    return e_success;
}