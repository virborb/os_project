
#include "KeyValueLib.h"

int fdin = 0;
int fdout = 0;

int open_KeyValueDB() {
    fdin = open("test_in.txt", O_RDWR);
    if(fdin < 0) {
        return -1;
    }
    fdout = open("test_out.txt", O_RDWR);
    if(fdin < 0) {
        close(fdin);
        return -1;
    }
    return 0;
}

int insert_elem(char *key, int key_len, char *val, int val_len) {
    int tot_len = val_len + MAX_KEY_LEN + 1;
    if(key_len > MAX_KEY_LEN || val_len > MAX_VAL_LEN) {
        return -1;
    }
    char *str = calloc(tot_len, 1);
	if (str == NULL) {
		return -1;
	}
	char *str_ptr;
	str_ptr = str;
	str[0] = PUT;
	str_ptr++;
	memcpy(str_ptr, key, key_len);
	str_ptr += MAX_KEY_LEN;
	memcpy(str_ptr, val, val_len);
	if(write(fdout, str, tot_len) < 0) {
        return -1;
	}
	free(str);
	return 0;
}

char *get_elem(char *key, int key_len) {
    int tot_len = MAX_KEY_LEN + 1;
    int val_len = 0;
    char *val;
    if(key_len > MAX_KEY_LEN) {
        return NULL;
    }
    char *str = calloc(tot_len, 1);
	if (str == NULL) {
		return NULL;
	}
	char *str_ptr;
	str_ptr = str;
	str[0] = GET;
	str_ptr++;
	memcpy(str_ptr, key, key_len);
	if(write(fdout, str, tot_len) < 0) {
        return NULL;
	}
	free(str);
    if(read(fdin, &val_len, 2) < 0) {
        return NULL;
	}
	val = calloc(val_len+1, 1);
	if (val == NULL) {
		return NULL;
	}
    if(read(fdin, &val, val_len) < 0) {
        return NULL;
	}
	return val;
}

int remove_elem(char *key, int key_len) {
    int tot_len = MAX_KEY_LEN + 1;
    if(key_len > MAX_KEY_LEN) {
        return -1;
    }
    char *str = calloc(tot_len, 1);
	if (str == NULL) {
		return -1;
	}
	char *str_ptr;
	str_ptr = str;
	str[0] = DEL;
	str_ptr++;
	memcpy(str_ptr, key, key_len);
	if(write(fdout, str, tot_len) < 0) {
        return -1;
	}
	free(str);
	return 0;
}
