#ifndef _PARSE_CFG_H
#define _PARSE_CFG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#define JSON_CFG_SIZE 64

typedef struct {
    unsigned int offset;
    unsigned int len;
    unsigned int mtdofs;
    unsigned char mtdnum;
    char name[32];
}_info;

typedef struct {
    _info *kernel_info;
    _info *dts_info;
}img_info;

#pragma pack(0)

int parse_info(char *file_info, img_info *info);

#endif