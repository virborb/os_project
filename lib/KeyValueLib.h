#ifndef KEY_VALUE_LIB_H
#define KEY_VALUE_LIB_H

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define PUT 0
#define GET 1
#define DEL 2

#define MAX_KEY_LEN 64
#define MAX_VAL_LEN 1024

int open_KeyValueDB();

int insert_elem(char *key, int key_len, char *val, int val_len);

char *get_elem(char *key, int key_len);

int remove_elem(char *key, int key_len);

#endif

