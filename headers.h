#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'
#define Scheduler_key_pathname "/tmp"


///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================



int getClk()
{
    return *shmaddr;
}


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}


int schedulingalgo;
int quantum;

struct PCB{
    int arrivaltime;
    int priority;
    int time_slice; // only For RR
    int runningtime;
    int pid; // Real Process Id
    int remainingtime;
    int runningstatus;
    // code for the runningstatus 
    //  1 for just arrived
    //  2 for running
    //  3 for blocked
    //  4 for resumed
    //  5 for stopped
    //  6 for finished 
    int specialid; // id in input file like 1 , 2 , 3
    bool FirstTime;
    int FirstRunTime;
    int lastStartTime;
};

struct message_to_sched{
    long message_type;
    struct PCB proceess_info;
};

//starintg from here is the priority queue implementiaion
typedef struct {
    int priority;
    struct PCB data;
} node_t;

typedef struct {
    node_t *nodes;
    int len;
    int size;
} heap_t;

void push (heap_t *h, int priority, struct PCB pid_n) {
    if (h->len + 1 >= h->size) {
        h->size = h->size ? h->size * 2 : 4;
        h->nodes = (node_t *)realloc(h->nodes, h->size * sizeof (node_t));
    }
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > priority) {
        h->nodes[i] = h->nodes[j];
        i = j;
        j = j / 2;
    }
    h->nodes[i].priority = priority;
    h->nodes[i].data = pid_n;
    h->len++;
}

struct PCB pop (heap_t *h) {
    int i, j, k;
    if (!h->len) {
        struct PCB teststruct;
        teststruct.specialid = 0;
        return teststruct;
    }
    struct PCB data = h->nodes[1].data;
    
    h->nodes[1] = h->nodes[h->len];
    
    h->len--;
    
    i = 1;
    while (i!=h->len+1) {
        k = h->len+1;
        j = 2 * i;
        if (j <= h->len && h->nodes[j].priority < h->nodes[k].priority) {
            k = j;
        }
        if (j + 1 <= h->len && h->nodes[j + 1].priority < h->nodes[k].priority) {
            k = j + 1;
        }
        h->nodes[i] = h->nodes[k];
        i = k;
    }
    return data;
}
//-----------------------
//it ends here

//stuff for the output perf -------------
int Total_WTA;
int out_c =0;
int each_WA_time[100];
int totalTA = 0;
int totalRunningtime = 0;


float calc_sum(int m, int n)
{
    float sum = 0;
    for (int i = 0 ; i < n; i++)
    {
        sum += pow((each_WA_time[i]-m),2);
    }
    return sum;
}
float calc_std(int m, int n)
{
    float std = pow(calc_sum(m,n)/(n-1),0.5);
    return std;
}
float calc_avg_WTA()
{
    return  ((float)Total_WTA/out_c);
}
float calc_avg_waiting()
{
    return ((float)totalTA/out_c);
}
float calc_CPU()
{
    float cpu = (totalRunningtime / (float)getClk())*100 ;
    printf("total running time %d \n" , totalRunningtime);
    printf("clock at this instant is %d \n" , getClk());
    return cpu;
}
FILE *fileptr_stats;
void measure_state(FILE* fileName)
{
    float std =             calc_std(Total_WTA, out_c);
    float avg_WTA =         calc_avg_WTA();
    float avg_waiting  =    calc_avg_waiting();
    float cpu =             calc_CPU();
    printf("the numbers are in orderd %f %f %f %f \n" , cpu , avg_WTA , avg_waiting ,std );
    fprintf(fileName , "CPU utilization =  %f \n Avg WTA =  %f \n Avg Waiting =  %f \n Std WTA = %f \n" , cpu, avg_WTA, avg_waiting, std);
}
//-----------------------------------------