#include "KeyValueLib.h"

int fd = 0;

int open_KeyValueDB() {
    fd = open("/dev/key_value_DB_char", O_RDWR);
    if(fd < 0) {
        return -1;
    }
    return 0;
}

int insert_elem(int key, char *val, int val_len) {
    int tot_len = MAX_VAL_LEN + MAX_KEY_LEN + 1;
    if(val_len > MAX_VAL_LEN) {
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
	memcpy(str_ptr, &key, MAX_KEY_LEN);
	str_ptr += MAX_KEY_LEN;
	memcpy(str_ptr, val, val_len);
	if(write(fd, str, tot_len) < 0) {
        return -1;
	}
	free(str);
	return 0;
}

char *get_elem(int key) {
    int tot_len = MAX_KEY_LEN + 1;
    char *val;
    char *str = calloc(tot_len, 1);
	if (str == NULL) {
		return NULL;
	}
	char *str_ptr;
	str_ptr = str;
	str[0] = GET;
	str_ptr++;
	memcpy(str_ptr, &key, MAX_KEY_LEN);
	if(write(fd, str, tot_len) < 0) {
        return NULL;
	}
	free(str);
	val = calloc(MAX_VAL_LEN+1, 1);
	if (val == NULL) {
		return NULL;
	}
    if(read(fd, &val, MAX_VAL_LEN) < 0) {
        return NULL;
	}
	return val;
}

int remove_elem(int key) {
    int tot_len = MAX_KEY_LEN + 1;
    char *str = calloc(tot_len, 1);
	if (str == NULL) {
		return -1;
	}
	char *str_ptr;
	str_ptr = str;
	str[0] = DEL;
	str_ptr++;
	memcpy(str_ptr, &key, MAX_KEY_LEN);
	if(write(fd, str, tot_len) < 0) {
        return -1;
	}
	free(str);
	return 0;
}
