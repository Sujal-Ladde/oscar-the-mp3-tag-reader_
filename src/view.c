#include "view.h"

/**
 * Displays the content of a single ID3v2 tag.
 *
 * @param mp3 File pointer to the MP3 file (positioned at the start of a tag).
 * @return e_success if the tag is successfully displayed, e_failure on error.
 *
 * @logic
 * 1. Reads the 4-byte tag name.
 * 2. Validates the tag name and retrieves its description.
 * 3. If the tag is "APIC", calls the `read_apic` function.
 * 4. Prints the tag name and its description.
 * 5. Reads the 4-byte tag size.
 * 6. Prints the tag size.
 * 7. Reads the tag data based on the size.
 * 8. Prints the tag data as a string.
 */
Status display_tag(FILE *mp3)
{
    if (!mp3)
    {
        fprintf(stderr, "ERROR: Invalid file pointer.\n");
        return e_failure;
    }

    char tag[4];
    if (fread(tag, 1, 4, mp3) != 4)
    {
        perror("ERROR: fread failed while reading tag name");
        return e_failure;
    }

    int tag_index;
    if (is_valid_tag_W_index(tag, &tag_index) == e_failure)
    {
        return e_failure;
    }

    if (strncmp(tag, "APIC", 4) == 0)
    {
        if (fseek(mp3, -4, SEEK_CUR) != 0)
        {
            perror("ERROR: fseek failed while rewinding for APIC");
            return e_failure;
        }
        if (read_apic(mp3, IMAGE_OUTPUT_PATH) == e_failure)
        {
            fprintf(stderr, "ERROR: Failed to read APIC tag.\n");
            return e_failure;
        }
        return e_success;
    }

    printf("\nThe tag is %.*s : %s\n", 4, tag, tagMappings[tag_index].description);

    uint8_t tag_size_bytes[4];
    if (fread(&tag_size_bytes, 1, 4, mp3) != 4)
    {
        perror("ERROR: fread failed while reading tag size");
        return e_failure;
    }

    int size = id3v2_tag_size(tag_size_bytes);
    printf("The size of the tag is %d bytes\n", size);

    if (fseek(mp3, 2, SEEK_CUR) != 0)
    {
        perror("ERROR: fseek failed while skipping tag flags");
        return e_failure;
    }

    uint8_t *tag_data = (uint8_t *)malloc(size);
    if (!tag_data)
    {
        perror("ERROR: malloc failed for tag data");
        return e_failure;
    }

    if (fread(tag_data, 1, size, mp3) != size)
    {
        perror("ERROR: fread failed while reading tag data");
        free(tag_data);
        return e_failure;
    }

    printf("The %.*s is ", 4, tag);
    for (int i = 0; i < size; i++)
    {
        printf("%c", tag_data[i]);
    }
    printf("\n");
    free(tag_data);
    return e_success;
}

/**
 * Displays detailed information about the ID3v2 tags in an MP3 file.
 *
 * @param mp3 File pointer to the MP3 file (opened in read binary mode).
 * @return e_success if the details are displayed successfully, e_failure on error.
 *
 * @logic
 * 1. Seeks to the position of the header size within the ID3v2 header (6 bytes from the start).
 * 2. Reads the 4-byte header size.
 * 3. Prints the calculated header size.
 * 4. Repeatedly calls the `display_tag` function to display individual tags until `display_tag` returns e_failure (indicating the end of the tags).
 * 5. Prints a message indicating the end of the header.
 */
Status display_deets(FILE *mp3)
{
    if (!mp3)
    {
        fprintf(stderr, "ERROR: Invalid file pointer.\n");
        return e_failure;
    }

    if (fseek(mp3, 6, SEEK_SET) != 0)
    {
        perror("ERROR: fseek failed while seeking header size");
        return e_failure;
    }

    char header_size_bytes[4];
    if (fread(header_size_bytes, 1, 4, mp3) != 4)
    {
        perror("ERROR: fread failed while reading header size");
        return e_failure;
    }
    printf("\nThe size of the Header is %u\n", id3v2_header_size(header_size_bytes));

    while (display_tag(mp3) != e_failure)
    {
        if (ferror(mp3))
        {
            perror("ERROR: Error occurred while displaying tags");
            return e_failure;
        }
    }

    fprintf(stdout, "\n\nEnd of the header.\n\n");
    return e_success;
}

/**
 * Reads and displays the content of a specific ID3v2 tag.
 *
 * @param mp3 File pointer to the MP3 file (opened in read binary mode).
 * @param given_tag The 4-byte tag name to search for.
 * @return e_success if the tag is found and displayed, e_failure otherwise.
 *
 * @logic
 * 1. Seeks to the beginning of the tag data area (10 bytes from the start).
 * 2. Enters a loop to read and check tags.
 * 3. Reads a 4-byte tag name.
 * 4. Checks if the read tag is a valid ID3v2 tag. If not, breaks the loop (end of tags).
 * 5. Compares the read tag with the `given_tag`.
 * 6. If they match, sets a flag, moves the file pointer back by 4 bytes, calls `display_tag` to display the tag, and returns e_success.
 * 7. If they don't match, reads the 4-byte tag size and skips the flags and tag data.
 * 8. After the loop, if the flag is still 0 (tag not found), prints an error message and returns e_failure.
 */
Status read_one_tag(FILE *mp3, const char *given_tag)
{
    if (!mp3 || !given_tag)
    {
        fprintf(stderr, "ERROR: Invalid input arguments.\n");
        return e_failure;
    }

    if (fseek(mp3, 10, SEEK_SET) != 0)
    {
        perror("ERROR: fseek failed while seeking tag area");
        return e_failure;
    }

    int flag = 0;
    while (1)
    {
        char tag[4];
        if (fread(tag, 1, 4, mp3) != 4)
        {
            if (feof(mp3))
            {
                break; // End of file reached
            }
            perror("ERROR: fread failed while reading tag name");
            return e_failure;
        }

        if (is_valid_tag(tag) == e_failure)
        {
            break; // Invalid tag, likely end of header
        }

        if (strncmp(tag, given_tag, 4) == 0)
        {
            flag = 1;
            if (fseek(mp3, -4, SEEK_CUR) != 0)
            {
                perror("ERROR: fseek failed while rewinding for display");
                return e_failure;
            }
            if (display_tag(mp3) == e_failure)
            {
                fprintf(stderr, "ERROR: Failed to display tag '%s'.\n", given_tag);
                return e_failure;
            }
            return e_success;
        }
        else
        {
            uint8_t tag_size_bytes[4];
            if (fread(&tag_size_bytes, 1, 4, mp3) != 4)
            {
                perror("ERROR: fread failed while reading tag size");
                return e_failure;
            }
            int size = id3v2_tag_size(tag_size_bytes);
            if (fseek(mp3, 2 + size, SEEK_CUR) != 0)
            {
                perror("ERROR: fseek failed while skipping tag data");
                return e_failure;
            }
        }
    }

    if (flag == 0)
    {
        fprintf(stderr, "ERROR: Tag '%s' Not Found\n", given_tag);
        return e_failure;
    }
    return e_success;
}

/**
 * Reads and extracts the embedded picture (APIC frame) from an MP3 file.
 *
 * @param mp3 File pointer to the MP3 file (positioned at the start of the APIC tag).
 * @return e_success if the APIC frame is successfully read and the image saved, e_failure on error.
 *
 * @logic
 * 1. Reads the 4-byte tag name and verifies it's "APIC".
 * 2. Reads the 4-byte tag size.
 * 3. Skips the 2-byte flags.
 * 4. Reads the text encoding byte.
 * 5. Reads the MIME type string until a null terminator.
 * 6. Reads the picture type byte.
 * 7. Reads the description string until a null terminator.
 * 8. Calculates the actual image data size.
 * 9. Allocates memory for the image data.
 * 10. Creates a new file using the description as the filename in write binary mode.
 * 11. Reads the image data from the MP3 file.
 * 12. Writes the image data to the newly created file.
 * 13. Closes the image file and frees the allocated memory.
 * 14. Prints a success message.
 */
Status read_apic(FILE *mp3, const char *output_path)
{
    if (!mp3)
    {
        fprintf(stderr, "ERROR: Invalid file pointer.\n");
        return e_failure;
    }

    char tag[5];
    if (fread(tag, 1, 4, mp3) != 4)
    {
        perror("ERROR: fread failed while reading tag name");
        return e_failure;
    }
    tag[4] = '\0';
    printf("\nThe tag is %s\n", tag);

    if (strncmp(tag, "APIC", 4) == 0)
    {
        unsigned char size_bytes[4];
        if (fread(&size_bytes, 1, 4, mp3) != 4)
        {
            perror("ERROR: fread failed while reading tag size");
            return e_failure;
        }
        unsigned int tag_size = id3v2_tag_size(size_bytes);
        printf("The size of the Tag is %u\n", tag_size);

        if (fseek(mp3, 2, SEEK_CUR) != 0)
        {
            perror("ERROR: fseek failed while skipping tag flags");
            return e_failure;
        }
        int frame_size_pos = ftell(mp3);

        char text_encoding;
        if (fread(&text_encoding, 1, 1, mp3) != 1)
        {
            perror("ERROR: fread failed while reading text encoding");
            return e_failure;
        }

        char MIME_type[100];
        int i = 0;
        while (i < 99)
        {
            if (fread(&MIME_type[i], 1, 1, mp3) != 1)
            {
                perror("ERROR: fread failed while reading MIME type");
                return e_failure;
            }
            if (MIME_type[i] == '\0')
            {
                MIME_type[i + 1] = '\0';
                break;
            }
            i++;
        }
        if (i == 99)
            MIME_type[99] = '\0';
        printf("the mime type is %s\n", MIME_type);

        char picture_type;
        if (fread(&picture_type, 1, 1, mp3) != 1)
        {
            perror("ERROR: fread failed while reading picture type");
            return e_failure;
        }
        printf("the pic type is %#x\n", picture_type);

        char discription[100];
        i = 0;
        while (i < 99)
        {
            if (fread(&discription[i], 1, 1, mp3) != 1)
            {
                perror("ERROR: fread failed while reading description");
                return e_failure;
            }
            if (discription[i] == '\0')
            {
                discription[i + 1] = '\0';
                break;
            }
            i++;
        }
        if (i == 99)
            discription[99] = '\0';
        printf("\nthe description of the image is - %s\n", discription);

        int actual_size = tag_size - (ftell(mp3) - frame_size_pos);
        if (actual_size < 0)
        {
            fprintf(stderr, "ERROR: Calculated image size is negative.\n");
            return e_failure;
        }

        char *image_data = (char *)malloc(actual_size);
        if (!image_data)
        {
            perror("ERROR: malloc failed for image data");
            return e_failure;
        }

        char image__file[256];
        strcpy(image__file, output_path);
        strcat(image__file, discription);

        FILE *image = fopen(image__file, "wb");
        if (!image)
        {
            perror("ERROR: fopen failed to create image file");
            free(image_data);
            return e_failure;
        }

        if (fread(image_data, 1, actual_size, mp3) != actual_size)
        {
            perror("ERROR: fread failed while reading image data");
            fclose(image);
            free(image_data);
            return e_failure;
        }

        if (fwrite(image_data, 1, actual_size, image) != actual_size)
        {
            perror("ERROR: fwrite failed while writing image data");
            fclose(image);
            free(image_data);
            return e_failure;
        }

        if (fclose(image) != 0)
        {
            perror("ERROR: fclose failed for image file");
            free(image_data);
            return e_failure;
        }
        free(image_data);

        fprintf(stdout, "The image file named \"%s\" is Successfully created.\n", discription);
        return e_success;
    }
    else
    {
        fprintf(stderr, "ERROR: Expected APIC tag, but found %s\n", tag);
        return e_failure;
    }
}