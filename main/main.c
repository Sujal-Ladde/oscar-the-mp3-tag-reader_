#include "edit.h"
#include "view.h"
#include "common.h"

Status main(int argc, char *argv[])
{

    if (argc == 1)
    {
        fprintf(stdout, "MP3 ID3v2 Metadata Toolkit\n\n");
        fprintf(stdout, "This project provides a command-line interface for managing ID3v2 tags in MP3 audio files. It enables users to inspect, modify, and enhance the metadata associated with their music library.\n\n");
        fprintf(stdout, "Key features include comprehensive tag viewing capabilities, allowing users to see all or specific tag values. The project also supports tag editing, enabling modification of existing information. Furthermore, it offers the ability to add new tags to MP3 files.\n\n");
        fprintf(stdout, "For media management, the toolkit includes functionality for handling embedded album art. Users can replace existing cover images or add new ones, specifying the image file, MIME type, and a descriptive name.\n\n");
        fprintf(stdout, "This tool aims to be a versatile solution for anyone needing to interact directly with the ID3v2 metadata of their MP3 files.\n");
        fprintf(stdout, "\n\ncheck --info for usage options.\n");
    }
    else if (strcmp(argv[1], "--info") == 0)
    {

        fprintf(stdout, "\n");
        fprintf(stdout, "Usage: ./a.out [FLAGS...] [SOURCE FILE]  \n");
        fprintf(stdout, "       ./a.out -v [SOURCE FILE] [TAG FLAG] ...  \n");
        fprintf(stdout, "       ./a.out -e [SOURCE FILE] [TAG FLAG] \"[DATA]\" \n");
        fprintf(stdout, "\n");
        fprintf(stdout, "[FLAGS...]\n");
        fprintf(stdout, "\t-t, to view all the tags in the ID3 V2\n");
        fprintf(stdout, "\t-v to view the tags from the Audio file.\n");
        fprintf(stdout, "\t-e, to edit the data of the audio file.\n");
        fprintf(stdout, "[SOURCE FILE]\n");
        fprintf(stdout, "\tThe name of the source file you want to read the data from.\n");
        fprintf(stdout, "[TAG FLAG]\n");
        fprintf(stdout, "\tMention if you want to read a particular tag from the file.\n\tIf Tag not mentioned the DEFAULTS TO DEISPLY ALL THE TAGS.\n");
        fprintf(stdout, "\tIn case of \033[1mEditing \033[0mit is the TAG you want to edit.\n");
        fprintf(stdout, "[DATA]\n");
        fprintf(stdout, "\tThe meta data you want to replce with.\n\tthe data must be in double inverted commas\n");
        fprintf(stdout, "\t\033[1mWARNING-- \033[0mUsed Only for editing.\n");
        fprintf(stdout, "\n");
        return 0;
    }
    else if (strcmp(argv[1], "-t") == 0)
    {
        fprintf(stdout, "\t The Tags in ID3 Version 2 are :\n");
        for (int i = 0; i < NUM_TAGS; i++)
        {

            fprintf(stdout, "%-5d %-6s: %s\n", i, tagMappings[i].tag, tagMappings[i].description);
        }
    }

    else if (strcmp(argv[1], "-e") == 0)
    {

        if (argv[3] == NULL || argc % 2 == 0)
        {
            fprintf(stdout, "ERROR : Too few arguments, check --info for more details\n");
            return e_failure;
        }

        char mp3__file[MAX_PATH_LENGTH];
        strcpy(mp3__file, MP3_FILES_PATH);
        strcat(mp3__file, argv[2]);

        FILE *mp3 = fopen(mp3__file, "r");

        char temp__mp3__file[MAX_PATH_LENGTH];
        strcpy(temp__mp3__file, MP3_FILES_PATH);
        strcat(temp__mp3__file, "new.mp3");

        FILE *new_mp3 = fopen(temp__mp3__file, "wb");

        int flag_tag = flag_to_tag(argv[3]);

        if (flag_tag == 0)
        {
            fprintf(stdout, "ERROR : Invalid Flag\n");
            fclose(mp3);
            return e_failure;
        }
        else
        {
            if (strcmp(argv[3], "APIC") == 0)
            {
                char *MIME = is_valid_image(argv[4]);

                if (MIME == NULL)
                {
                    fprintf(stdout, "ERROR : Invalid Image file\n");
                    return e_failure;
                }
                char image__file[MAX_PATH_LENGTH];
                strcpy(image__file, IMAGE_INPUT_PATH);
                strcat(image__file, argv[4]);
                FILE *img = fopen(image__file, "rb");

                replace_image(mp3, new_mp3, img, MIME, argv[4], argv[2]);
                fclose(img);
                free(MIME);
                return e_success;
            }
            edit_tags(mp3, new_mp3, tagMappings[flag_tag].tag, argv[4], argv[2]);
        }
    }

    else if (strcmp(argv[1], "-v") == 0)
    {
        if (argv[2] == NULL)
        {
            fprintf(stdout, "ERROR : Too few arguments, check --info\n");
            return e_failure;
        }

        if (argv[3] == NULL)
        {

            char mp3__file[MAX_PATH_LENGTH];
            strcpy(mp3__file, MP3_FILES_PATH);
            strcat(mp3__file, argv[2]);

            FILE *mp3 = fopen(mp3__file, "r");

            if (is_valid_file(mp3) == e_failure)
            {
                fprintf(stderr, "ERROR: Invalid File\n");
                fclose(mp3);

                return e_failure;
            }
            display_deets(mp3);
            fclose(mp3);
        }

        else
        {

            char mp3__file[MAX_PATH_LENGTH];
            strcpy(mp3__file, MP3_FILES_PATH);
            strcat(mp3__file, argv[2]);

            FILE *mp3 = fopen(mp3__file, "r");
            if (is_valid_file(mp3) == e_failure)
            {
                fprintf(stderr, "ERROR: Invalid File\n");
                fclose(mp3);

                return e_failure;
            }

            for (int i = 3; i < argc; i++)

            {

                int flag_tag = flag_to_tag(argv[i]);

                if (flag_tag == 0)
                {
                    fprintf(stdout, "ERROR : Invalid tag\n");
                    fclose(mp3);
                }
                else
                {

                    if (strncmp(argv[i], "APIC", 4) == 0)
                    {
                        read_apic(mp3, IMAGE_OUTPUT_PATH);
                    }
                    read_one_tag(mp3, tagMappings[flag_tag].tag);
                }
            }

            fclose(mp3);
        }
    }
}
