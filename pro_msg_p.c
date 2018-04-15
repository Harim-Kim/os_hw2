#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <mqueue.h>
void server_function(){
    struct mq_attr attr;//메시지큐의 속성 지정을 위한 구조체, 소스의 내용은 별 내용없이 recieve되면 출력
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 10;
    char result[10];
    mqd_t mfd;
    mfd = mq_open("/mymq", O_RDWR | O_CREAT, 555, &attr);
    if(mfd == -1){
        perror("mq_open error-server\n:");
        exit(0);
    }
    while(1){
        if((mq_receive(mfd, result, attr.mq_msgsize, NULL))== -1){
            perror("Server Receive error\n");
            exit(-1);
        }
        printf("Read Data %s\n", result);
    }
}
void client_function(){
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 10;
    char buf[10]="";
    mqd_t mfd;
    mfd = mq_open("/mymq", O_WRONLY, 555, &attr);
    if(mfd==-1){
        perror("error mq open\n");
    }
    while(1){
        printf("input data:\n");
        scanf("%s", buf);
        if((mq_send(mfd, buf, attr.mq_msgsize,1))==-1){
            perror("error sending from 자식 프로세스");
            exit(-1);
        }
    }
}
int main(){
    int pid = fork();
    if(pid >0 ){
        printf("부모프로세스: %d, %d \n", getpid(), pid);
        server_function();
    }else if(pid == 0){
        printf("자식 프로세스: %d, %d\n", getpid(), pid);
        client_function();
    }else if(pid==1){
        perror("fork error:");
        exit(0);
    }
}