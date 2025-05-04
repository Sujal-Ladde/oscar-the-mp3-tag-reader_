#include "common.h"
uint8_t size = 0;

struct mp3
{
};
void get_deets(FILE *mp3)
{
    char tag[5];

    fread(tag, 1, 5, mp3);
    printf("\nthe tag is %s\n", tag);

    fseek(mp3, ftell(mp3) + 2, SEEK_SET);

    fread(&size, 1, 1, mp3);
    printf("the size of the tag is %d\n", size);
    char *tpe = (char *)malloc((size) * sizeof(char));

    fseek(mp3, ftell(mp3) + 2, SEEK_SET);
    fread(tpe, 1, size, mp3);

    printf("the %s is ", tag);

    for (int i = 0; i < size; i++)
    {
        printf("%c", tpe[i]);
    }
    printf("\n");
    free(tpe);
}

void display_deets(FILE *mp3)
{
    if (is_valid_file(mp3) == e_failure)
    {
        fprintf(stderr, "invalid file");
    }
    printf("after id3 %ld\n", ftell(mp3));

    fseek(mp3, ftell(mp3) + 3, SEEK_SET);
    unsigned char size_bytes[4];
    printf("size is %d\n", *size_bytes);
    fread(size_bytes, 1, 4, mp3);
    uint16_t size = id3v2_tag_size(size_bytes);
    printf("size is %d\n", size );
    printf("after size %ld\n", ftell(mp3));


    for (int i = 0; i < size ; i++)
    {
        char chai = 0;
        int current_pos = ftell(mp3);
        char pot_tag[5];
        fread(&chai, 1, 1, mp3);
        if (isalnum(chai))
        {
            fseek(mp3, current_pos - 1, SEEK_SET);
            fread(pot_tag, 1, 5, mp3);
            current_pos = ftell(mp3);
            if (is_valid_tag(pot_tag) == e_success)
            {
                fseek(mp3, current_pos - 5, SEEK_SET);

                get_deets(mp3);
            }
            else
                fseek(mp3, current_pos - 3, SEEK_SET);
        }
    }

    printf("\n");
}

int main()
{
    FILE *mp3 = fopen("sample1.mp3", "r");
    display_deets(mp3);
    fclose(mp3);
}
