#include "headers.h"
#include <signal.h>
#include <stdio.h>
//#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
//#define PROJECT_ID 'M'

void RunAlgo(int , int );
void RunRR(int);
void RunSRTN();
void RunSJF();

void pause_process(struct PCB*);
void run_process(struct PCB*);
void handler_sigchild(int);

void MakeOutputFile();
void fetchToPQ(struct PCB*);
int recieveProcess();

heap_t *PQ_PCBs; 
int currentalgo;
int time_slice;
int p_gen_qid;
int something_running = 0;

//variables needed for output file
//int totalTA = 0;
//int totalRunningtime = 0;
int num_process = 0;
FILE *fileptr_log;

struct PCB *current_pro_ptr;

void handler_end(int);
void handlersiguser(int);

int main(int argc, char * argv[])
{
    initClk();
    printf("from scheduler \n");
    signal(SIGCHLD  , handler_sigchild);
     signal(SIGINT , handler_end);
    signal(SIGUSR1 , handlersiguser);
    PQ_PCBs = (heap_t *)calloc(1, sizeof (heap_t));
    //TODO implement the scheduler :)
    //upon termination release the clock resources

    fileptr_log = fopen("scheduler.log" , "w");
    fileptr_stats = fopen("scheduler.perf" , "w");
    fprintf(fileptr_log , "At \t time \t x \t process \t y \t state \t arr \t w \t total \t z \t remain \t y \t wait \t k \n");
    //setting up stuff here
    //setting up connection 
    key_t process_gen_key , scheduler_process_key;
    // , sched_qid;
    //process gen scheduer ipc
    if ((process_gen_key = ftok("firstFTOK", 65)) == -1) {
        perror ("ftok");
        exit (1);
    }

    //printf("the ftok at scheduler %d \n" , process_gen_key);

    if ((p_gen_qid = msgget (process_gen_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }
    

    int currentTime = getClk();

    
    currentalgo = *argv[1];
    time_slice = *argv[2];
    printf("here is the algorithm numeber you %d \n" , currentalgo);
    printf("here is the time slice  you enterd %d \n" , time_slice);



    RunAlgo(currentalgo,time_slice);
   
}

void RunAlgo(int num_of_algo ,int time_slice ){
    if(currentalgo == 1){
        RunSJF();
    }else if(currentalgo == 2){
        RunSRTN();
    }else if(currentalgo == 3){
        RunRR(time_slice);
    }
}

int recieveProcess(){
    //0 for reciveing nothing 1 for reciveing something
    struct message_to_sched incoming_msg;

    if (msgrcv (p_gen_qid, &incoming_msg, sizeof (struct PCB), 0, IPC_NOWAIT) == -1) {

            if(errno != ENOMSG){
                perror("reciving message failure");
                exit(1);
            }
            return 0;
        }else{
            printf("%d",incoming_msg.proceess_info.runningtime);
            fetchToPQ(&incoming_msg.proceess_info);
            num_process = num_process+1;
            return 1;
        }
}

void fetchToPQ(struct PCB *p){
    if(currentalgo == 1){
        //fetch according to shortest running time 
        push(PQ_PCBs , p->runningtime , *p);
        printf("pushing to the oq done sucesffuly \n");
    }else if(currentalgo == 2){
        //fetch according to shortest remainng time
        push(PQ_PCBs , p->remainingtime , *p);
    }else if(currentalgo == 3){
        push(PQ_PCBs , p->arrivaltime , *p);
    }
    
}

void RunSRTN(){
    struct PCB currentPCB;
    struct PCB comparingPCB;
    current_pro_ptr = &currentPCB;
    while(true){
        printf("start of the while loop \n");
        int recieved = recieveProcess();
        if(something_running == 0)
        {
            currentPCB = pop(PQ_PCBs);
            if(currentPCB.specialid == 0){
                printf("there is no process at the current time \n");
            }
            else
            {
                run_process(&currentPCB);
                something_running = 1;
            }
        }
        if(recieved == 1 && something_running == 1)
        {
            //printf("after the recieved condition \n");
            comparingPCB= pop(PQ_PCBs);
            printf("comparing PCB adata %d\n" , comparingPCB.specialid);
            if(comparingPCB.specialid != 0){
                if(comparingPCB.remainingtime < currentPCB.remainingtime)
                {
                    pause_process(&currentPCB);
                    printf("the process about to be pusehd %d" , currentPCB.specialid);
                    fetchToPQ(&currentPCB);
                    currentPCB = comparingPCB;
                    run_process(&currentPCB);
                    something_running = 1;
                }
                else{
                    fetchToPQ(&comparingPCB);
                }
            }
        }
        if(something_running == 1){
            currentPCB.remainingtime = current_pro_ptr->remainingtime - (getClk() - current_pro_ptr->lastStartTime);
        if(currentPCB.remainingtime < 0){
            raise(SIGUSR1);
            continue;
        }
        }
        sleep(1);
    }
}


//write here the 2 remaining algorithms 
void RunRR(int time_sl){
    struct PCB currentPCB;
    current_pro_ptr = &currentPCB;
    int last_switch_time = getClk(); // To track when we last switched processes

    while(true){
        int received = recieveProcess();
        
        // Check if it's time to switch to the next process in the queue
        if(something_running && (getClk() - last_switch_time >= time_slice)){
            // Preempt current process if it hasn't finished
            if(current_pro_ptr->remainingtime > 0){
                pause_process(current_pro_ptr);
                fetchToPQ(current_pro_ptr); // Re-add the current process to the queue
            }
            something_running = 0; // Clear the flag as we are going to switch processes
        }
        
        // If no process is currently running, get the next one from the queue
        if(!something_running){
            // Try to pop the next process from the queue
            currentPCB = pop(PQ_PCBs);
            if(currentPCB.specialid == 0){
                // No process to run
                printf("No process available to run at the current time.\n");
            } else {
                // Run the next process
                run_process(&currentPCB);
                something_running = 1; // Mark that a process is now running
                last_switch_time = getClk(); // Update the switch time
            }
        }
        
        // Additional logic to handle newly arrived processes or other scheduling criteria
        // could be placed here. This could involve checking if new processes have arrived
        // and should be immediately scheduled, or handling any other scheduling decisions.
        
        sleep(1); // Wait a bit before the next scheduling decision
    }
}


void RunSJF(){
    struct PCB currentPCB;
    current_pro_ptr = &currentPCB;
    while(true){
        
        int recivedsometing = recieveProcess();
        if(something_running == 0){
            //here you should add that something happened in the output file
            currentPCB = pop(PQ_PCBs);
            if(currentPCB.specialid == 0){
                printf("no process at the current time \n");
            }else{
                run_process(&currentPCB);
                something_running = 1;
            }
        }
        sleep(1);
    }
}

//funcitons of stopping , pausing and running a process ---------------
void run_process(struct PCB *currentprocess){
    if(currentprocess->FirstTime == true){
        int pid_process;
        char rem_time_char = (char) currentprocess->remainingtime;
        char* p_rem_time_char = &rem_time_char;
        pid_process = fork();
        if(pid_process == -1){
            perror("error in fork");
            exit(1);
        }else if(pid_process == 0){
            char* filepath = "./process.out";
            //printf("i hace reached this part");
            if(execlp(filepath , filepath, p_rem_time_char, NULL) == -1){
                perror("error in exec");
                exit(1);
            }
            
            
            //wrtie here that we started a process LOG
        }else{
            currentprocess->pid = pid_process;
            printf("the process id is %d \n" , pid_process);
            currentprocess->FirstRunTime = getClk();
            
            currentprocess->FirstTime = false;
            currentprocess->runningstatus = 2;
            printf("the process id is this time from currentprocess%d \n" , currentprocess->pid);
            fprintf(fileptr_log , "At \t time \t %d \t process %d \t started \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , 0);
            printf("At \t time \t %d \t process %d \t started \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , 0);
            currentprocess->lastStartTime = getClk();
        }
        
    }else{
        
        currentprocess->runningstatus = 2;
        fprintf(fileptr_log , "At \t time \t %d \t process %d \t resumed \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , getClk()-currentprocess->lastStartTime);
        printf("At \t time \t %d \t process %d \t resumed \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , getClk()-currentprocess->lastStartTime);
        currentprocess->lastStartTime = getClk();
        if(kill(currentprocess->pid , SIGCONT) == -1){
            perror("error in sending the signal of resuming");
        }
        return;
    }
}

void pause_process(struct PCB *currentprocess){

    printf("the process about to  be stopped special is %d \n"  , currentprocess->specialid);
    printf("I am here at the puase start of function \n");
    printf("the process about to  be stopped %d \n" , currentprocess->pid);
    if(kill(currentprocess->pid , SIGSTOP) == -1){
        perror("error in sending the signal sigstp ");
    }
    currentprocess->remainingtime = currentprocess->remainingtime - (getClk() - currentprocess->lastStartTime);
    //write here that we paused a process
    fprintf(fileptr_log , "At \t time \t %d \t process %d \t paused \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , 0);
    printf("At \t time \t %d \t process %d \t paused \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , 0);
    printf("I am here at the puase \n");
}

void handler_sigchild(int signum){
    
}

void handler_end(int signum1){
    fclose(fileptr_log);
    fclose(fileptr_stats);
    measure_state(fileptr_stats);
    exit(1);
}
//------------------------------------------------

void handlersiguser(int signum3){
    struct PCB currentprocess = *current_pro_ptr;
    int TA_for_current = getClk() - currentprocess.arrivaltime;
    int WTA_for_current = TA_for_current/currentprocess.runningtime;
    current_pro_ptr->remainingtime = current_pro_ptr->remainingtime - (getClk() - current_pro_ptr->lastStartTime);
    printf("the remain time is %d\n" , current_pro_ptr->remainingtime);
    if(current_pro_ptr->remainingtime <= 0){
    fprintf(fileptr_log , "At \t time \t %d \t process %d \t finished \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \t TA \t %d \t WTA \t %d \n" , getClk() , currentprocess.specialid , currentprocess.arrivaltime , currentprocess.runningtime , 0 , 0 , TA_for_current , WTA_for_current);
    printf("At \t time \t %d \t process %d \t finished \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \t TA \t %d \t WTA \t %d \n" , getClk() , currentprocess.specialid , currentprocess.arrivaltime , currentprocess.runningtime , current_pro_ptr->remainingtime , 0 , TA_for_current , WTA_for_current);

    current_pro_ptr->runningstatus = 6;
    something_running = 0;
    Total_WTA +=  WTA_for_current;
    totalTA += (currentprocess.FirstRunTime - currentprocess.arrivaltime );
    totalRunningtime += currentprocess.runningtime;
    each_WA_time[out_c] = WTA_for_current;
    out_c += 1 ;
    num_process = num_process -1;
    sleep(1);
    }
    
}