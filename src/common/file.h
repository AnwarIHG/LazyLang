#pragma once

#ifndef _FILE_H
#define _FILE_H


#include "types.h"

#include <stdio.h>

#define BUFFER_LIMIT 524288 // 0.5 MB

typedef struct {
    const char* filename;
    char* source;
    FILE* c_file;
    u64 size;
    u64 left;
} File;


/* 
 * this function will open the file
 * */
File File_create(const char* filepath);

/* 
 * this function will open the file
 * */
File File_create(const char* filepath);

u64 File_read(File* file);

/* free the allocated memroy for the file*/
void File_destroy(File *file);

#endif //_FILE_H
