/**
 * @file   testebbchar.c
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief  A Linux user space program that communicates with the ebbchar.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/ebbchar.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
*/
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<linux/random.h>
#include<stdbool.h>
#include <syscall.h>
#include <sys/types.h>
#include <sys/wait.h>


#define VSJ_GET 0
#define VSJ_SET 1
#define VSJ_DEL 2
#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)


int main(){
   int ret, fd, testint;
   char *strarr[20];
   int intarr[20];
   int keyarr[40];//index 0-19 int 20-39 *char
   size_t sizearr[20];
   int err;
   bool ignorearr[40] = {false};
   char buf[48];
   char retbuf[48];
   pid_t pid;

   pid = fork();
   if(pid < 0){
     return -1;
   }
   printf("Starting device test\n");
   fd = open("/dev/key_value_DB_char", O_RDWR);             // Open the device with read/write access
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }
   buf[0] = VSJ_SET;
   for(int i = 0; i < 20; ++i){
       /*syscall(SYS_getrandom,&keyarr[i], sizeof(int), 0);
       syscall(SYS_getrandom,&keyarr[i+20],sizeof(int), 0);*/
       syscall(SYS_getrandom, &sizearr[i],sizeof(size_t), 0);
       sizearr[i] = sizearr[i]%41 + 1;
       strarr[i] = malloc(sizearr[i]);
       syscall(SYS_getrandom, &intarr[i], sizeof(int), 0);
       syscall(SYS_getrandom, strarr[i], sizearr[i], 0);
       strarr[i][sizearr[i]-1] = '\0';
       if( pid == 0){
         keyarr[i] = i;
         keyarr[i+20] = i+20;
       }else{
         keyarr[i] = i+ 50;
         keyarr[i+20] = i + 100;
       }
       memcpy(&buf[1], &keyarr[i+20], sizeof(int));
       memcpy(&buf[1+sizeof(int)], strarr[i], sizearr[i]);
       ret = write(fd, buf, sizearr[i]+sizeof(int)+1);
       if(ret < 0){
           printf("ERROR in write, key:%d, str:%s, i:%d\n, size:%zd, \n buf:%hhd\n bufsize:%zd\nerror: %s\n",
                    keyarr[i+20], strarr[i], i, sizearr[i], buf[0], strnlen(buf, 48),strerror(errno));
            ignorearr[i+20] = true;
       }
       memcpy(&buf[1], &keyarr[i], sizeof(int));
       memcpy(&buf[1+sizeof(int)], &intarr[i], sizeof(int));
       ret = write(fd, buf, 1 + sizeof(int)*2);
       if(ret < 0){
           printf("ERROR in write SET, key:%d, int:%d, i:%d\n, error: %s\n",
                    keyarr[i], intarr[i], i, strerror(errno));
            ignorearr[i] = true;
       }
   }

   for(int i = 0; i < 20; ++i){
       buf[0] = VSJ_GET;
       if(ignorearr[i]){
           continue;
       }
       memcpy(&buf[1], &keyarr[i], sizeof(int));
       ret = write(fd, buf, sizeof(int) + 1);
       if (ret < 0){
           printf("ERROR in write GET, key:%d, int:%d, i:%d\n error: %s\n",
                    keyarr[i], intarr[i], i, strerror(errno));
           continue;
       }
       ret = read(fd, &testint, sizeof(int));        // Read the response from the LKM
       if (ret < 0){
           printf("ERROR in read, key:%d, int:%d, i:%d\n error: %s\n",
                    keyarr[i], intarr[i], i, strerror(errno));
           continue;
       }

       if(testint != intarr[i]){
           printf("Fail for key %d.Expected val: %d, got val: %d\n",keyarr[i],intarr[i], testint);
       }/*else {
           printf("i: %d:success for key %d.Expected val: %d, got val: %d\n",i, keyarr[i],intarr[i], testint);
       }*/
       buf[0] = VSJ_DEL;
       ret = write(fd, buf, sizeof(int) + 1);
       if (ret < 0){
           printf("ERROR in write DEL, key:%d, int:%d, i:%d\n error: %s\n",
                    keyarr[i], intarr[i], i, strerror(errno));
           continue;
       }
       ret = read(fd, &testint, sizeof(int));        // Read the response from the LKM
       if (ret < 0 && errno == ENOKEY){
           /*printf("DEL success, key:%d, int:%d, i:%d\n error: %s\n",
                    keyarr[i], intarr[i], i, strerror(errno));*/
       }else{
           printf("DEL failed  key:%d, int:%d, i:%d\n ",
                    keyarr[i], intarr[i], i);
       }

   }
   for(int i = 0; i < 20; ++i){
       buf[0] = VSJ_GET;
       if(ignorearr[i]){
           continue;
       }
       memcpy(&buf[1], &keyarr[i + 20], sizeof(int));
       ret = write(fd, buf, sizeof(int) + 1);
       if (ret < 0){
           printf("ERROR in write GET, key:%d, str:%s, i:%d\n error: %s\n",
                    keyarr[i + 20], strarr[i], i, strerror(errno));
           continue;
       }
       ret = read(fd, retbuf, sizearr[i]);        // Read the response from the LKM
       if (ret < 0){
           printf("ERROR in read, key:%d, str:%s, i:%d\n error: %s\n",
                    keyarr[i + 20], strarr[i], i, strerror(errno));
           continue;
       }

       if(strncmp(strarr[i],retbuf,sizearr[i])){
           printf("Fail for key %d.Expected str: %s, got str: %s\n",keyarr[i + 20],strarr[i], retbuf);
       } /*else {
           printf("success for key %d.Expected str: %s, got str: %s\n",keyarr[i + 20],strarr[i], retbuf);
       }*/
       buf[0] = VSJ_DEL;
       ret = write(fd, buf, sizeof(int) + 1);
       if (ret < 0){
           printf("ERROR in write DEL, key:%d, str:%s, i:%d\n error: %s\n",
                    keyarr[i + 20], strarr[i], i, strerror(errno));
           continue;
       }
       ret = read(fd, &testint, sizeof(int));        // Read the response from the LKM
       if (ret < 0 && errno == ENOKEY){
           /*printf("DEL success, key:%d, str:%d, i:%d\n error: %s\n",
                    keyarr[i + 20], strarr[i], i, strerror(errno));*/
       }else{
           printf("DEL failed  key:%d, str:%s, i:%d\n ",
                    keyarr[i + 20], strarr[i], i);
       }
   }


   for(int i = 0; i < 20; ++i){
     free(strarr[i]);
   }
   wait(NULL);
   return 0;
}
