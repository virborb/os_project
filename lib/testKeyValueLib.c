
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
    if(insert_elem(3, "value", 5) < 0) {
    	perror("insert");
    	exit(-1);
    } else {
    	printf("insert success\n");
    }
    char *test = get_elem(3);
    if(test == NULL) {
    	perror("get");
    	exit(-1);
    } else {
    	printf("get success\n");
    }
    if(remove_elem(3) < 0) {
    	perror("remove");
    	exit(-1);
    } else {
    	printf("remove success\n");
    }
}
