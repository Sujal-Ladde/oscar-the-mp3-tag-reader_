#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


struct mp3{};

void display_deets(FILE *mp3)
{
    fseek(mp3, 9, SEEK_SET);

    char tag[5];
    fread(tag, 1, 5, mp3);
    printf("\nthe tag is %s\n", tag);

    printf("current offset = %ld\n", ftell(mp3));

    fseek(mp3, ftell(mp3) + 3, SEEK_SET);

    printf("current offset = %ld\n", ftell(mp3));

    char size;
    fread(&size, 1, 1, mp3);
    printf("the size of the tag is %d\n", size);

    fseek(mp3, ftell(mp3) + 3, SEEK_SET);

    char title[size + 1];
    fread(title, 1, size - 1, mp3);
    printf("the title is");

    for (int i = 0; i < 46; i++)
    {
        printf("%c", title[i]);
    }
    printf("\n");

    fread(tag, 1, 5, mp3);
    printf("\nthe tag is %s\n", tag);

    fseek(mp3, ftell(mp3) + 2, SEEK_SET);

    fread(&size, 1, 1, mp3);
    printf("the size of the tag is %d\n", size);
    char tpe[size + 1];

    fseek(mp3, ftell(mp3) + 2, SEEK_SET);
    fread(tpe, 1, size, mp3);

    printf("the tpe is ");

    for (int i = 0; i < size; i++)
    {
        printf("%c", tpe[i]);
    }
    printf("\n");

    fread(tag, 1, 5, mp3);
    printf("\nthe tag is %s\n", tag);
    fseek(mp3, ftell(mp3) + 2, SEEK_SET);

    fread(&size, 1, 1, mp3);
    printf("the size of the tag is %d\n", size);
    char talb[size + 1];

    fseek(mp3, ftell(mp3) + 2, SEEK_SET);
    fread(talb, 1, size, mp3);

    printf("the talb is ");

    for (int i = 0; i < size; i++)
    {
        printf("%c", talb[i]);
    }

    printf("\n");

    fread(tag, 1, 5, mp3);
    printf("\nthe tag is %s\n", tag);
    fseek(mp3, ftell(mp3) + 2, SEEK_SET);
    fread(&size, 1, 1, mp3);
    printf("the size of the tag is %d\n", size);
    char tyer[size + 1];

    fseek(mp3, ftell(mp3) + 2, SEEK_SET);
    fread(tyer, 1, size, mp3);

    printf("the tyer is ");

    for (int i = 0; i < size; i++)
    {
        printf("%c", tyer[i]);
    }

    printf("\n");

    fread(tag, 1, 5, mp3);
    printf("\nthe tag is %s\n", tag);
    fseek(mp3, ftell(mp3) + 2, SEEK_SET);
    fread(&size, 1, 1, mp3);
    printf("the size of the tag is %d\n", size);
    char tcon[size + 1];

    fseek(mp3, ftell(mp3) + 2, SEEK_SET);
    fread(tcon, 1, size, mp3);

    printf("the tcon is ");

    for (int i = 0; i < size; i++)
    {
        printf("%c", tcon[i]);
    }

    printf("\n");

    fread(tag, 1, 5, mp3);
    printf("\nthe tag is %s\n", tag);
    fseek(mp3, ftell(mp3) + 2, SEEK_SET);
    fread(&size, 1, 1, mp3);
    printf("the size of the tag is %d\n", size);
    char comm[size + 1];

    fseek(mp3, ftell(mp3) + 2, SEEK_SET);
    fread(comm, 1, size, mp3);

    printf("the comm is ");

    for (int i = 0; i < size; i++)
    {
        printf("%c", comm[i]);
    }

    printf("\n");
    printf("current offset = %ld\n", ftell(mp3));
}
int main()
{
    FILE *mp3 = fopen("sample.mp3", "r");
    display_deets(mp3);
}