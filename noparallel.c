#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
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
// k means clustering make new mean
void newMean(struct Clusters cl, int dotNums, int clusterNum){
    for(int i=0;i<clusterNum;i++){
        float x_sum = 0;
        float y_sum = 0;
        int count = 0;

        //skip if there is no dot in cluster
        if(cl.means[i].x == 100001.0){
            continue;
        }
        for(int j=0;j<dotNums;j++){
            if(cl.locations[j].cluster == i){
                count++;
                x_sum = x_sum + cl.locations[j].x;
                y_sum = y_sum + cl.locations[j].y;
            }
        }
        //if there is any dot in cluster make it unsual else calculate new cluster
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

// using input 
int main(){
    int testCase;
    scanf("%d", &testCase);
    //total time spend
    struct timespec start_;
    clock_gettime(CLOCK_REALTIME, &start_);
    //testcase time spend
    struct timespec start, end;
    
    int sum = 0;
    for(int itr=0;itr<testCase;itr++){
        
        struct Clusters cluster;
        struct Location *locationArray;
        struct Means *meanArray;
        int alNum;
        scanf("%d", &alNum);
        int clusterNum;
        scanf("%d", &clusterNum);
        int dotNums;
        scanf("%d", &dotNums);
        int loc;
        locationArray = (struct Location*)malloc(sizeof(struct Location)*dotNums);
        meanArray = (struct Means*)malloc(sizeof(struct Means)*clusterNum);
        for(loc=0;loc<dotNums;loc++){
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

        //start time check
        
        clock_gettime(CLOCK_REALTIME, &start);
        for(int i = 0; i< alNum; i++){
            clustering(cluster,dotNums,clusterNum);
            newMean(cluster,dotNums,clusterNum);
        }
        //end time check
        clock_gettime(CLOCK_REALTIME, &end);
        double result = (end.tv_sec - start.tv_sec) * 1000000
                      + (double)(end.tv_nsec - start.tv_nsec) 
                      /1000;
        printf("Test Case #%d \n", itr);
        printf("%lld microseconds\n", (long long int)(result));
        sum += result;
        for(int i = 0; i<dotNums;i++){
            printf("%d \n",cluster.locations[i].cluster);
        }
        free(locationArray);
        free(meanArray);
        
    }
    
    double result = (end.tv_sec - start_.tv_sec) * 1000000
                    + (double)(end.tv_nsec - start.tv_nsec) 
                    /1000;
    printf("total timespend: %lld microsecond \n",(long long int)(result));
    printf("total algorithm time spend: %d \n", sum);
    return 0;
}