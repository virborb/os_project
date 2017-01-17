
#include "KeyValueLib.h"
#include "testKeyValueLib.h"
#include <stdio.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



int main() {
    if(open_KeyValueDB() < 0) {
    	perror("open");
    	exit(-1);
    } else {
    	printf("open success\n");
    }
    char str[20][BUFLEN] = {0};
    int keys[20];
    char errstr[BUFLEN] = {'\0'};
    int ret;
    char *letters = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
    srand(467556);
    /*
    for(int i = 0; i < 20; i++){
        printf("Type in a short string to send to the kernel module:\n");
        scanf("%[^\n]%*c", str[i]);                // Read in a string (with spaces)
        printf("Type the key for the string :");
                //    "(the test will not work if duplicate keys are added):\n");
        scanf("%d", &keys[i]);
        int c;
        while((c=getchar()) != '\n' && c != EOF);
    }*/
    for(int i = 0; i < 20; i++){
        randomStr(str[i], letters, BUFLEN);
        keys[i] = i*31;
    }

    printf("Writing messages to the device [%s].\n", str);
    for(int i = 0; i < 20; i++){
        if(insert_elem(keys[i], str[i], strlen(str[i]) + 1) < 0) {
            errorExit("Insert with error", 1);
        }
    }
    printf("Inserts was successful\n");
    char test[BUFLEN] = {0};
    char *retptr = NULL;
    printf("Press ENTER to read back from the device...\n");
    getchar();
    for(int i = 0; i < 20; i++){
        retptr = get_elem(keys[i], test, BUFLEN);
        if(retptr == NULL) {
    	    errorExit("Error in get_elem", 1);
        } else if(!strncmp(test, str[i], BUFLEN)){
    	    printf("get_elem successfull for key %d , returned value: %s\n",
                    keys[i], test);
        }else{
            snprintf(errstr, BUFLEN - 1,"get_elem failed for key %d ,"
                    " returned value: %s, expected value: %s\n",
                    keys[i], test, str[i]);
            errorExit(errstr, 0);
        }
    }

    printf("\n\nSaving the keyvalue store to fp.txt.\n");

    char *loc="fp.txt";
    if(save_KeyValueDB(loc)<0){
        perror("close");
        exit(-1);
    }else {
        printf("Save done without errors\n");
    }

    printf("\n\nTesting remove.\n");

    for(int i = 0; i < 20; i++){
        if(remove_elem(keys[i]) < 0) {
            snprintf(errstr, BUFLEN - 1,"remove_elem failed for key %d with Error\n",
                    keys[i]);
            errorExit(errstr, 1);
        } else if(!(NULL == get_elem(3, test, strlen(str[i])) && errno == ENOKEY)){
            snprintf(errstr, BUFLEN - 1,"get_elem failed for key %d ,"
                    " key still exists\n", keys[i]);
            errorExit(errstr, 0);
        }
    }
    printf("remove success\n");
    /*    if(load_KeyValueDB(loc)<0) {
        perror("load");
        exit(-1);
    }*/
   /* if(remove_elem(3) < 0) {
        perror("remove");
        exit(-1);
    } else {
        printf("remove success\n");
    }*/

    if(0 > load_KeyValueDB(loc)){
        errorExit("Error when running load_KeyValueDB", 1);
    }
    for(int i = 0; i < 20 ; i++){
        retptr = get_elem(keys[i], test, BUFLEN);
        if(retptr == NULL) {
            snprintf(errstr, BUFLEN - 1,"get_elem failed for key %d ,"
                    " after load_KeyValueDB\n", keys[i]);
            errorExit(errstr, 1);
        } else if (strncmp(test, str[i], BUFLEN)){
            snprintf(errstr, BUFLEN - 1,"get_elem failed for key %d ,"
                    " after load_KeyValueDB, Expected val: %s, returned: %s\n",
                    keys[i], str[i], test);
            errorExit(errstr, 0);
        }
    }
    printf("get_elem was successful for the keys after load_KeyValueDB\n\n");


    if(closeAndSave_KeyValueDB(loc)<0){
        perror("closeAndSave_KeyValueDB");
        exit(-1);
    }else {
        printf("closeAndSave_KeyValueDB done without errors\n\n");
    }

    if(open_KeyValueDB() < 0) {
        perror("open");
        exit(-1);
    } else {
        printf("open success\n");
    }

    for(int i = 0; i < 20; i++){
        if(remove_elem(keys[i]) < 0) {
            snprintf(errstr, BUFLEN - 1,"remove_elem failed for key %d with Error\n",
                    keys[i]);
            errorExit(errstr, 1);
        } else if(!(NULL == get_elem(3, test, strlen(str[i])) && errno == ENOKEY)){
            snprintf(errstr, BUFLEN - 1,"get_elem failed for key %d ,"
                    " key still exists\n", keys[i]);
            errorExit(errstr, 0);
        }
    }
    printf("remove success\n");

    if(0 > load_KeyValueDB(loc)){
        errorExit("Error when running load_KeyValueDB", 1);
    }
    for(int i = 0; i < 20 ; i++){
        retptr = get_elem(keys[i], test, BUFLEN);
        if(retptr == NULL) {
            snprintf(errstr, BUFLEN - 1,"get_elem failed for key %d ,"
                    " after load_KeyValueDB\n", keys[i]);
            errorExit(errstr, 1);
        } else if (strncmp(test, str[i], BUFLEN)){
            snprintf(errstr, BUFLEN - 1,"get_elem failed for key %d ,"
                    " after load_KeyValueDB, Expected val: %s, returned: %s\n",
                    keys[i], str[i], test);
            errorExit(errstr, 0);
        }
    }


    if(close_KeyValueDB()==0){
        printf("close_KeyValueDB ran without errors\n");
    }else{
        errorExit("Error when calling close_KeyValueDB", 0);
    }
}

void errorExit(const char *msg, int err){
    if(err){
        perror(msg);
    } else {
        printf(msg);
    }
    close_KeyValueDB();
    exit(-1);
}

void randomStr(char *str, char *letters, int n){
    int i;
    int nroflet = strlen(letters);
    str[0] = letters[rand()%nroflet];
    for(i = 1; i < (n-1); i++){
        str[i] = letters[rand()%nroflet];
        if(!(rand()%3)){
            break;
        }
    }
    if(i < n -1){
        str[i+1] = '\0';
    } else {
        str[n - 1] = '\0';
    }
}
