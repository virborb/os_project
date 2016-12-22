
#include "KeyValueLib.h"
#include <stdio.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main() {
    if(open_KeyValueDB() < 0) {
    	perror("open");
    	exit(-1);
    } else {
    	printf("open success\n");
    }
    char str[100] = {0};
    printf("Type in a short string to send to the kernel module:\n");
   scanf("%[^\n]%*c", str);                // Read in a string (with spaces)
   printf("Writing message to the device [%s].\n", str);
    if(insert_elem(3, str, strlen(str)) < 0) {
    	perror("insert");
    	exit(-1);
    } else {
    	printf("insert success\n");
    }
    char test[20] = {0};
    printf("Press ENTER to read back from the device...\n");
    getchar();
    get_elem(3, test, strlen(str));
    if(test == NULL) {
    	perror("get");
    	exit(-1);
    } else {
    	printf("get success ,%s\n", test);
    }
    /*if(remove_elem(3) < 0) {
    	perror("remove");
    	exit(-1);
    } else {
    	printf("remove success\n");
    }*/

    char *loc="fp.txt";
    if(save_KeyValueDB(loc)<0){
        perror("close");
        exit(-1);
    }
    if(remove_elem(3) < 0) {
        perror("remove");
        exit(-1);
    } else {
        printf("remove success\n");
    }
    if(load_KeyValueDB(loc)<0){
        perror("load");
        exit(-1);
    }
    if(remove_elem(3) < 0) {
        perror("remove");
        exit(-1);
    } else {
        printf("remove success\n");
    }
}
