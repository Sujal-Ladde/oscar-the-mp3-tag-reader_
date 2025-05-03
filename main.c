#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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
    fseek(mp3, 10, SEEK_SET);

    get_deets(mp3);

    get_deets(mp3);

    get_deets(mp3);

    get_deets(mp3);

    get_deets(mp3);

    get_deets(mp3);

    printf("\n");
    printf("current offset = %ld\n", ftell(mp3));
}
int main()
{
    FILE *mp3 = fopen("dj.mp3", "r");
    display_deets(mp3);
    fclose(mp3);
}