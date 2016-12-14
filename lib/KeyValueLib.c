#include "KeyValueLib.h"

int fd = 0;

int open_KeyValueDB() {
    fd = open("/dev/key_value_DB_char", O_RDWR);
    if(fd < 0) {
        return -1;
    }
    return 0;
}

int insert_elem(int key, void *val, size_t val_len) {
    int tot_len = val_len + sizeof(int) + 1;
    char *str = malloc(tot_len);
	if (str == NULL) {
		return -1;
	}
	char *str_ptr;
	str_ptr = str;
	str[0] = VSJ_SET;
	str_ptr++;
	memcpy(str_ptr, &key, sizeof(int));
	str_ptr += sizeof(int);
	memcpy(str_ptr, val, val_len);
	if(write(fd, str, tot_len) < 0) {
        return -1;
	}
	free(str);
	return 0;
}

void *get_elem(int key, void *val, size_t val_len) {
    int tot_len = sizeof(int) + 1;
    char *str = malloc(tot_len);
	if (str == NULL) {
		return NULL;
	}
	char *str_ptr;
	str_ptr = str;
	str[0] = VSJ_GET;
	str_ptr++;
	memcpy(str_ptr, &key, sizeof(int));
	if(write(fd, str, tot_len) < 0) {
        return NULL;
	}
	free(str);
    if(read(fd, val, val_len) < 0) {
        return NULL;
	}
	return val;
}

int remove_elem(int key) {
    int tot_len = sizeof(int) + 1;
    char *str = malloc(tot_len);
	if (str == NULL) {
		return -1;
	}
	char *str_ptr;
	str_ptr = str;
	str[0] = VSJ_DEL;
	str_ptr++;
	memcpy(str_ptr, &key, sizeof(int));
	if(write(fd, str, tot_len) < 0) {
        return -1;
	}
	free(str);
	return 0;
}
