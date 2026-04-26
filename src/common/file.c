#include "./file.h"
#include "../common/macro.h"

#include <stdlib.h>

File File_create(const char* filepath) {

    File file = {0};
    file.filename = filepath;

    file.c_file = fopen(filepath, "rb");

    // get the file size
    fseek(file.c_file, 0, SEEK_END);
    file.size = ftell(file.c_file);
    file.left = file.size;

    rewind(file.c_file);
    return file;
}

u64 File_read(File* file){

    if (!file->source) { 
        file->source = (char*)malloc(MIN(file->size, BUFFER_LIMIT) + 1);

        if (!file->source) {
            fclose(file->c_file);
            fprintf(stderr, "Error: Memory allocation failed\n");
            return 0;
        }
    }

    u64 bytes_read = 0;
    if (file->left > 0) {
        bytes_read = fread(file->source, 1, MIN(file->left, BUFFER_LIMIT), file->c_file);

        file->left -= bytes_read;
        file->source[bytes_read] = '\0';
    } else {
        return bytes_read;
    }

    return bytes_read;
}

/* free the allocated memroy for the file*/
void File_destroy(File *file) {
    if (file->c_file) fclose(file->c_file);
    if (file->source) free(file->source);
}
