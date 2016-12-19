#ifndef KEY_VALUE_LIB_H
#define KEY_VALUE_LIB_H

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define VSJ_GET  0
#define VSJ_SET  1
#define VSJ_DEL  2
#define VSJ_SAVE 3

int open_KeyValueDB();

int insert_elem(int key, void *val, size_t val_len);

void *get_elem(int key, void *val, size_t val_len);

int remove_elem(int key);

int close_KeyValueDB();
int save_KeyValueDB(char *filename);
int closeAndSave_KeyValueDB(char *filename);

#endif

