#include "common.h"

char id3tags[84][5]={ "AENC",
    "APIC",
    "ASPI",
    "COMM",
    "COMR",
    "ENCR",
    "EQU2",
    "ETCO",
    "GEOB",
    "GRID",
    "LINK",
    "MCDI",
    "MLLT",
    "OWNE",
    "PRIV",
    "PCNT",
    "POPM",
    "POSS",
    "RBUF",
    "RVA2",
    "RVRB",
    "SEEK",
    "SIGN",
    "SYLT",
    "SYTC",
    "TALB",
    "TBPM",
    "TCOM",
    "TCON",
    "TCOP",
    "TDEN",
    "TDLY",
    "TDOR",
    "TDRC",
    "TDRL",
    "TDTG",
    "TENC",
    "TEXT",
    "TFLT",
    "TIPL",
    "TIT1",
    "TIT2",
    "TIT3",
    "TKEY",
    "TLAN",
    "TLEN",
    "TMCL",
    "TMED",
    "TMOO",
    "TOAL",
    "TOFN",
    "TOLY",
    "TOPE",
    "TOWN",
    "TPE1",
    "TPE2",
    "TPE3",
    "TPE4",
    "TPOS",
    "TPRO",
    "TPUB",
    "TRCK",
    "TRSN",
    "TRSO",
    "TSOA",
    "TSOP",
    "TSOT",
    "TSRC",
    "TSSE",
    "TSST",
    "TYER",
    "TXXX",
    "UFID",
    "USER",
    "USLT",
    "WCOM",
    "WCOP",
    "WOAF",
    "WOAR",
    "WOAS",
    "WORS",
    "WPAY",
    "WPUB",
    "WXXX"};


Status is_valid_tag(const char *tag){
    for(int i=0;i<83;i++){
        if(strcmp(tag,id3tags[i])==0){
            return e_success;
        }
    }
    return e_failure;
}

Status is_valid_file(FILE * mp3){
    char file_type[4];
    fread(file_type,1,3,mp3);
    if(strcmp("ID3",file_type)){
        return e_failure;
    }
    return e_success;
}

unsigned int id3v2_tag_size(unsigned char size_bytes[4]) {
    return (size_bytes[0] << 21) |
           (size_bytes[1] << 14) |
           (size_bytes[2] << 7)  |
           (size_bytes[3]);
}