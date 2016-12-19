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

int readNext(){

	int val_len=100;
	char *buf=malloc(sizeof(char)*val_len);

	if(		read(fd,buf,val_len)<0)
		return 0;
	fprintf(stdout, "Returned from iterator %s\n", buf);
 	
 	return 1;
}

int save_KeyValueDB(char *filename) {
	FILE *fp = fopen(filename,"w");
	if(fp<0) {
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
		q=readNext();	
	} 
}
