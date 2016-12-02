#ifndef KEY_VALUE_LIB_H
#define KEY_VALUE_LIB_H

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PUT 0
#define GET 1
#define DEL 2

#define MAX_KEY_LEN 4
#define MAX_VAL_LEN 10

int open_KeyValueDB();

int insert_elem(int key, char *val, int val_len);

char *get_elem(int key);

int remove_elem(int key);

#endif

