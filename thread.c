#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<pthread.h>
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
struct teskInput{
    struct Clusters clusters;
    int dotNums;
    int clusterNum;
    int alNum;
    long long int clusteringTime ;
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
            cl.means[i].x = 100000.0;
            cl.means[i].y = 100000.0;
        }else{
            float new_x = x_sum / count;
            float new_y = y_sum / count;
            cl.means[i].x = new_x;
            cl.means[i].y = new_y;
        }
    }
};

/**global variable**/
//tesk input for thread
struct teskInput *teskInputs;
// dots
struct Location *locationArray;
// mean
struct Means *meanArray;
// get time spend - total
struct timespec  start_,end_;
//mutex
// pthread_mutex_t lock;

//void function for pthread
void *clusterCase(void * teskinput){
    // pthread_mutex_lock(&lock);
    int alnum = ((struct teskInput*) teskinput)->alNum;
    struct Clusters cl = ((struct teskInput*) teskinput)->clusters;
    int dotNum = ((struct teskInput*) teskinput)->dotNums;
    int clusterNum =((struct teskInput*) teskinput)->clusterNum;
    struct timespec start,end;
    clock_gettime(CLOCK_REALTIME, &start);
    for(int i=0;i<alnum;i++){
        clustering(cl,dotNum,clusterNum);
        newMean(cl,dotNum,clusterNum);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    double result = (end.tv_sec - start.tv_sec) *1000000
                      + (double)(end.tv_nsec - start.tv_nsec) 
                      /1000;
    long long int resultime = (long long int)(result);
    ((struct teskInput*) teskinput)->clusteringTime = resultime;
};


// using input main function
int main(){
    int testCase;
    scanf("%d", &testCase);
    //tesk inputs initial
    teskInputs = (struct teskInput*)malloc(sizeof(struct teskInput)* testCase);
    //make threads
    pthread_t p_thread[testCase];
    //get time
    clock_gettime(CLOCK_REALTIME, &start_);
    //get dot infos
    for(int itr=0;itr<testCase;itr++){
        struct teskInput teskinput;
        struct Clusters cluster;

        

        //get infos
        int alNum;
        scanf("%d", &alNum);
        int clusterNum;
        scanf("%d", &clusterNum);
        int dotNums;
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

        //make teskInput
        // output.dotNum = dotNums;
        // output.locations = cluster.locations;
        // teskinput.op = output;
        teskinput.clusters = cluster;
        teskinput.alNum = alNum;
        teskinput.dotNums = dotNums;
        teskinput.clusterNum = clusterNum;
        teskInputs[itr] = teskinput;
        

        
    }//end task cases
    //threads
    for(int i=0;i<testCase;i++){
        int thr;
        struct timespec start,end;
        clock_gettime(CLOCK_REALTIME, &start);
        thr = pthread_create(&p_thread[i], NULL, clusterCase, &teskInputs[i]);
        clock_gettime(CLOCK_REALTIME, &end);
        double result = (end.tv_sec - start.tv_sec) *1000000
                        + (double)(end.tv_nsec - start.tv_nsec) 
                        /1000;
        long long int resultime = (long long int)(result);
        printf("thread 생성시간: %lld \n", resultime);
    }
    for(int i=0;i<testCase;i++){
        pthread_join(p_thread[i], NULL);
    }
    // pthread_mutex_destroy(&lock);
    for(int i=0;i<testCase;i++){
        printf("Test Case #%d \n", i);
        printf("%lld microseconds \n", teskInputs[i].clusteringTime);
        int dots = teskInputs[i].dotNums;
        struct Clusters cls = teskInputs[i].clusters;
        for(int i = 0; i<dots;i++){
            printf("%d \n",cls.locations[i].cluster);
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
    free(teskInputs);
    return 0;
}