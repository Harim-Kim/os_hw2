#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/stat.h> 
#include <errno.h>
#include <string.h>
struct Location{
    float x;
    float y;
    int cluster;
};
struct Means{
    float x;
    float y;
};
struct Clusters{
    struct Location* locations;
    struct Means* means;
};
//output for process ipc
struct Output{
    long msgtype;
    long long int resultTime;
    int testCase;
    struct Location oneDot;
};
struct DotOutput{
    long msgtype;
    int cluster;
};

// k means clustering using mean
void clustering(struct Clusters cl, int dotNums, int clusterNum){
    float x;
    float y;
    float x_m;
    float y_m;
    
    for(int i=0;i<dotNums;i++){
        x = cl.locations[i].x;
        y = cl.locations[i].y;
        cl.locations[i].cluster = 0;
        float least_distance = pow(cl.means[0].x -x,2) + pow(cl.means[0].y -y ,2);
        for(int j=0;j<clusterNum;j++){
            x_m = cl.means[j].x;
            y_m = cl.means[j].y;
            float distance = pow(x_m - x, 2) + pow(y_m - y, 2);
            if(distance < least_distance){
                cl.locations[i].cluster = j;
                least_distance = distance;
            }
        }
    }
};


void newMean(struct Clusters cl, int dotNums, int clusterNum){
    for(int i=0;i<clusterNum;i++){
        float x_sum = 0;
        float y_sum = 0;
        int count = 0;
        if(cl.means[i].x == 100000.0){
            continue;
        }
        for(int j=0;j<dotNums;j++){
            if(cl.locations[j].cluster == i){
                count++;
                x_sum = x_sum + cl.locations[j].x;
                y_sum = y_sum + cl.locations[j].y;
            }
        }
        if(count == 0){
            cl.means[i].x = 100001.0;
            cl.means[i].y = 100001.0;
        }else{
            float new_x = x_sum / count;
            float new_y = y_sum / count;
            cl.means[i].x = new_x;
            cl.means[i].y = new_y;
        }
    }
};

/**global variable**/
// dots
struct Location *locationArray;
// mean
struct Means *meanArray;
// get time spend - total
struct timespec  start_,end_;

// using input main function
int main(){
    int dotNums;
    int testCase;
    scanf("%d", &testCase);
    //get time
    clock_gettime(CLOCK_REALTIME, &start_);
    int pid;
    key_t key_id;
    //get dot infos
    for(int itr=0;itr<testCase;itr++){
        struct Clusters cluster;
        //get infos
        int alNum;
        scanf("%d", &alNum);
        int clusterNum;
        scanf("%d", &clusterNum);
        
        scanf("%d", &dotNums);

        // dot's array
        locationArray = (struct Location*)malloc(sizeof(struct Location)*dotNums);
        meanArray = (struct Means*)malloc(sizeof(struct Means)*clusterNum);

        for(int loc=0;loc<dotNums;loc++){
            struct Location location;
            struct Means means;
            float x;
            float y;
            scanf("%f %f",&x, &y );
            if(loc<clusterNum){
                means.x = x;
                means.y = y;
                meanArray[loc] = means;
            }
            location.x = x;
            location.y = y;
            location.cluster = -1;
            locationArray[loc] = location;
            

        }

        cluster.locations = locationArray;
        cluster.means = meanArray;
        //make child process
        struct timespec start,end;
        clock_gettime(CLOCK_REALTIME, &start);
        pid = fork();
        
        if(pid == 0){
            clock_gettime(CLOCK_REALTIME, &end);
            double resultp = (end.tv_sec - start.tv_sec) *1000000
                            + (double)(end.tv_nsec - start.tv_nsec) 
                            /1000;
            long long int resultimep = (long long int)(resultp);
            printf("process%d 생성시간: %lld \n",itr, resultimep);
            fflush(stdout);
            key_id = msgget((key_t)itr+1988,IPC_CREAT|0666);
            if(key_id == -1){
                perror("msgget error");
                exit(0);
            }
                struct timespec start,end;
            clock_gettime(CLOCK_REALTIME, &start);
            for(int i=0; i<alNum; i++){
                clustering(cluster,dotNums, clusterNum);
                newMean(cluster, dotNums, clusterNum);
            }
            clock_gettime(CLOCK_REALTIME, &end);
            double result = (end.tv_sec - start.tv_sec) *1000000
                            + (double)(end.tv_nsec - start.tv_nsec) 
                            /1000;
            long long int resultime = (long long int)(result);
            //for output
            struct Output mymsg;
            mymsg.msgtype = 1;
            mymsg.resultTime = resultime;
            mymsg.testCase = itr;
            if(msgsnd(key_id, (void *)&mymsg,sizeof(struct Output), 0)==-1){
                perror("msg send error\n");
                printf("errorrrrrr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%d\n", errno);
                exit(0);
            }
            // for(int i=0; i< dotNums; i++){
            //     struct DotOutput dots;
            //     memset(&dots,0x00,sizeof(dots));
            //     dots.msgtype =2;
            //     dots.cluster = locationArray[i].cluster;
            //     if(msgsnd(key_id, (void *)&dots, sizeof(struct DotOutput), 0)==-1){
            //         perror("msg send error");
            //         printf("errorrrrrr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%d\n", errno);
            //         exit(0);
            //     }
            // }
            break;
        }else if(pid==1){
            perror("fork error:");
            exit(0);
        }
    }//end task cases
    if(pid>0){
        for(int i=0; i< testCase; i++){
            struct Output getmymsg;
            key_id = msgget(i+1988,IPC_CREAT|0666);
            if(key_id < 0){
                perror("msgget error");
                exit(0);
            }
            if(msgrcv(key_id, (void *)&getmymsg, sizeof(struct Output), 1, 0)==-1){
                perror("msg get error");
                exit(0);
            }
            printf("Test Case #%d \n",getmymsg.testCase);
            fflush(stdout);
            printf("%lld microseconds\n",getmymsg.resultTime);
            fflush(stdout);
            // for(int j=0;j<dotNums;j++){
            //     struct DotOutput getDots;
            //     if(msgrcv(key_id,(void *)&getDots, sizeof(struct DotOutput), 2, 0)==-1){
            //         perror("msg get error");
            //         exit(0);
            //     }
            //     printf("%d \n",getDots.cluster);
            //     fflush(stdout);
            // }


            if(msgctl(key_id, IPC_RMID,NULL)== -1){
                perror("msgctl error");
                exit(1);
            } 
        }
    clock_gettime(CLOCK_REALTIME, &end_);
    double result = (end_.tv_sec - start_.tv_sec) *1000000
                      + (double)(end_.tv_nsec - start_.tv_nsec) 
                      /1000;
    long long int resultime = (long long int)(result);
    printf("total timespend: %lld microsecond \n",(long long int)(result));
    
    // free memory
    free(locationArray);
    free(meanArray);    
    }
    
    
    return 0;
}