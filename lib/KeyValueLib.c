#include "KeyValueLib.h"

int fd = 0;

/** @brief Open the key-value database.
 */
int open_KeyValueDB() {
    fd = open("/dev/key_value_DB_char", O_RDWR);
    if(fd < 0) {
        return -1;
    }
    return 0;
}

/** @brief inserts an element to the database.
 *  @param key The key of the value
 *  @param val The value
 *  @param val_len The size of the value
 *  Returns a pointer to the  value.
 */
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

/** @brief Gets an element with the given
 *  key from the database.
 *	@param key The key for the value to get
 *  @param val Where to save the value
 *  @param val_len The size of the value
 */
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


/** @brief Removes an element in the database with the given key.
 *  @param key The key of the element to remove.
 */
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


int close_KeyValueDB(){
	int cl = close(fd);
	if(cl!=0){
		fprintf(stdout, "%s\n", strerror(cl));
		return -1;
	}
	return 0;
}

int closeAndSave_KeyValueDB(char *filename) {

	save_KeyValueDB(filename);
	int cl = close(fd);
	if(cl!=0) {
		fprintf(stdout, "%s\n", strerror(cl));
		return -1;
	}
	return 0;

}

int readNext(void **buffer, int *len){

	int val_len= 0;
	int key = 0;
	*len = sizeof(int) + sizeof(size_t);
	*buffer = malloc(*len);
	if(*buffer == NULL) {
		return 0;
	}
	int qret= read(fd,*buffer, *len);
	if(qret<0){
			return 0;
	}
	memcpy(&key, *buffer, sizeof(int));
	memcpy(&val_len, *buffer + sizeof(int), sizeof(size_t));
	*buffer = realloc(*buffer, *len+val_len);
	qret= read(fd,*buffer + *len, val_len);
	if(qret<0){
			return 0;
	}
	*len += val_len;
	fprintf(stdout, "Returned from iterator:");
 	printf("key: %d, size:%d, value:%s\n", key, val_len, *buffer + sizeof(int) + sizeof(size_t));
 	fprintf(stdout, "\n");
 	return 1;
}

int save_KeyValueDB(char *filename) {
	int tot_len = 0;
	int savefd = open(filename, O_WRONLY | O_CREAT);
	void *buf = NULL;
	if(savefd<0) {
		fprintf(stderr, "couldnt save %s\n", filename);	
	}
	char *str = malloc(4);

	str[0] = VSJ_SAVE;
	int ff=0;
	ff=write(fd, str, sizeof(int));
	if(ff < 0) {
		fprintf(stderr, "KVL: saveKVDB: %s\n", strerror(ff) );
	}
	int q=1;
	while (q){
		q=readNext(&buf, &tot_len);
		printf("tot_len: %d\n", tot_len);
		write(savefd, buf, tot_len);
		free(buf);
	}
}

int load_KeyValueDB(char *filename) {
	int val_len = 0;
	int key = 0;
	int len = sizeof(int) + sizeof(size_t);
	void *buf = NULL;
	int loadfd = open(filename, O_RDONLY);
	if(loadfd<0) {
		fprintf(stderr, "couldnt load %s\n", filename);	
	}
	buf = malloc(len);
	if(buf == NULL) {
		return -1;
	}
	int err = read(loadfd,buf, len);
	if(err<0){
		return -1;
	}
	memcpy(&key, buf, sizeof(int));
	memcpy(&val_len, buf + sizeof(int), sizeof(size_t));
	free(buf);
	buf = malloc(val_len);
	if(buf == NULL) {
		return -1;
	}
	err = read(loadfd,buf, val_len);
	if(err<0){
		return -1;
	}
	printf("key: %d, size:%d, value:%s\n", key, val_len, buf);
	return insert_elem(key, buf, val_len); 
}