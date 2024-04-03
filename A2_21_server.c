#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

#define SHM_SIZE sizeof(msg)
#define SHM_SIZE2 sizeof(reg)
#define Max_Clients 100000

int globe=1000;
int total_clients=0;
int total_active_clients=0;
int randomn = 0;
char registered_clients[Max_Clients][50];

typedef struct {
    int written;
    int type;
    char name[50];
    int operation;
    int op1;
    int op2;
    float ans;
} msg;

typedef struct {
    int written;
    int type;
    int n;
    char name[50];
    int operation;
    int op1;
    int op2;
    int ans;
    int key;
} reg;

//map<string, pthread_t> registered_clients;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int is_prime(int num) {
    if (num < 2) {
        return 0;
    }
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}

void *client_handler(void *arg) {
    int key = *(int *)arg;
    
    int shmid = shmget(key, SHM_SIZE, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("Failed to create shared memory segment");
        exit(EXIT_FAILURE);
    }

    msg * message = (msg*) shmat(shmid, NULL, 0);
        //printf("at start %s && %s\n",registered_clients[0],registered_clients[1]);
        
    if (message == (msg*) -1) {
        perror("Failed to attach shared memory segment");
        exit(EXIT_FAILURE);
    }
    shmdt(message);
    shmctl(shmid, IPC_RMID, NULL);
    return NULL;
    while (message->written == 0) {
        pthread_mutex_unlock(&mutex);
        usleep(100000);
        pthread_mutex_lock(&mutex);
    }

    //int result = 0;
    while(message->type!=-1){
        if(message->written==1){
            pthread_mutex_lock(&mutex);
            int result=0;
            //printf("here\n");
            switch (message->operation) {
                case 1:
                    result = message->op1 + message->op2;
                    printf("=> Result of request of client %s is: %d\n", message->name,result);
                    message->ans=result;
                    return NULL;
                case 2:
                    result = message->op1 - message->op2;
                    printf("=> Result of request of client %s is: %d\n", message->name,result);
                    message->ans=result;
                    return NULL;
                case 3:
                    result = message->op1 * message->op2;
                    printf("=> Result of request of client %s is: %d\n", message->name,result);
                    message->ans=result;
                    return NULL;
                case 4:
                    float answer;
                    answer=(float) message->op1/message->op2;
                    printf("=> Result of request of client %s is: %.2f\n", message->name,answer);
                    message->ans=answer;
                    return NULL;
                case 5:
                    //int ans;
                    if(message->op1%2==0){
                        printf("=> Result of request of client %s is: EVEN\n", message->name);
                        message->ans=1;
                    }
                    else{
                        printf("=> Result of request of client %s is: ODD\n",message->name);
                        message->ans=0;
                    }
                    return NULL;
                case 6:
                    int ans;
                    ans=is_prime(message->op1);
                    if(ans==1){
                        printf("=> Result of request of client %s is: YES it is a Prime\n", message->name);
                        message->ans=1;
                    }
                    else{
                        printf("=> Result of request of client %s is: NO it is not a Prime\n", message->name);
                        message->ans=0;
                    }
                    return NULL;
                default:
                    //printf("g\n");
                    printf("Invalid operation\n");
                    message->ans=-1;
                   return NULL;
            }
            message->written=0;
            pthread_mutex_unlock(&mutex);
        }
        else {
            pthread_mutex_unlock(&mutex);
            usleep(100000);
            pthread_mutex_lock(&mutex);
        }
    }
    int i=0;
    for(i=0;i<randomn-1;i++){
        if(strcmp(registered_clients[i], message->name)==0){
            //printf("client %s here matched with %s at %d\n",shared_message->name,registered_clients[i],i);
            break;
        }
    }
    strcpy(registered_clients[i], "***");
    printf(" Client %s Succesfully unregistered\n", message->name);
    shmdt(message);
    shmctl(shmid, IPC_RMID, NULL);
    return NULL;
}

void *Client_Handler(void *arg) {
    reg *message = (reg*) arg;
    // printf("%s\n",message->name);
    // printf("%d\n",message->operation);
    // printf("%d\n",message->op1);
    int result = 0;
    switch (message->operation) {
        case 1:
                    result = message->op1 + message->op2;
                    printf("=> Result of request of client %s is: %d\n", message->name,result);
                    message->ans=result;
                    return NULL;
                case 2:
                    result = message->op1 - message->op2;
                    printf("=> Result of request of client %s is: %d\n", message->name,result);
                    message->ans=result;
                    return NULL;
                case 3:
                    result = message->op1 * message->op2;
                    printf("=> Result of request of client %s is: %d\n", message->name,result);
                    message->ans=result;
                    return NULL;
                case 4:
                    float answer;
                    answer=(float) message->op1/message->op2;
                    printf("=> Result of request of client %s is: %.2f\n", message->name,answer);
                    message->ans=answer;
                    return NULL;
                case 5:
                    //int ans;
                    if(message->op1%2==0){
                        printf("=> Result of request of client %s is: EVEN\n", message->name);
                        message->ans=1;
                    }
                    else{
                        printf("=> Result of request of client %s is: ODD\n",message->name);
                        message->ans=0;
                    }
                    return NULL;
                case 6:
                    int ans;
                    ans=is_prime(message->op1);
                    if(ans==1){
                        printf("=> Result of request of client %s is: YES it is a Prime\n", message->name);
                        message->ans=1;
                    }
                    else{
                        printf("=> Result of request of client %s is: NO it is not a Prime\n", message->name);
                        message->ans=0;
                    }
                    return NULL;
                default:
                    //printf("x\n");
                    printf("Invalid operation\n");
                    message->ans=-1;
                   return NULL;
    }
    return NULL;
}

int main() {
    key_t key = ftok("random", 100);
    int shmid = shmget(key, SHM_SIZE, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("Failed to create shared memory segment");
        exit(EXIT_FAILURE);
    }

    reg *reg_details = (reg*) shmat(shmid, NULL, 0);
        //printf("at start %s && %s\n",registered_clients[0],registered_clients[1]);
        
    if (reg_details == (reg*) -1) {
        perror("Failed to attach shared memory segment");
        exit(EXIT_FAILURE);
    }
    
    while (true) {
        
        printf("---------------------------------------------------------------------\n");
        printf("total clients requests = %d && total active client = %d\n",total_clients,total_active_clients);
        printf("Waiting for client request.......\n");
        
        while (reg_details->written == 0) {
            pthread_mutex_unlock(&mutex);
            usleep(1);
            pthread_mutex_lock(&mutex);
        }

        total_clients++;
        printf("Capturing the request\n");
        
        int i=0;
        for(i=0;i<randomn;i++){
            if(strcmp(registered_clients[i], reg_details->name)==0){
                //printf("Client %s already exits, provide your request with proper key\n",reg_details->name);
                break;
            }
        }
        
        if(i==randomn){
            total_active_clients++;
            reg_details->key=globe;
            
            printf("New Client < %s > is connected\n", reg_details->name);
            printf("Key of Client < %s > is %d\n", reg_details->name,reg_details->key);

            pthread_t new_thread;
            pthread_create(&new_thread, NULL, client_handler, (void*)&globe);
            //printf("%d\n",1);
            reg_details->n=1;
            globe++;
            strcpy(registered_clients[randomn], reg_details->name);
            pthread_mutex_unlock(&mutex);
            
        }
        else{
            printf(" Existing Client < %s > has requested\n", reg_details->name);
            //printf("--------------%d", reg_details->type);
            int key=reg_details->key;
            // printf("%d\n",key);
            // int shmidx = shmget(key, SHM_SIZE, 0666);
            // if (shmidx == -1) {
            //     perror("Failed to create shared memory segment");
            //     exit(EXIT_FAILURE);
            // }

            // msg * message = (msg*) shmat(shmidx, NULL, 0);
            // printf("at start %s && %s\n",registered_clients[0],registered_clients[1]);
        
            // if (message == (msg*) -1) {
            // perror("Failed to attach shared memory segment");
            // exit(EXIT_FAILURE);
            // }
            // message->operation=reg_details->operation;
            // message->op1=reg_details->op1;
            // message->op2=reg_details->op2;
            // message->type=reg_details->type;
            // message->written=reg_details->written;
            // usleep(1000);
            // message->type=0;
            if(reg_details->type==-1 || reg_details->operation==-1){
                total_active_clients--;
                total_clients--;
                printf("Client %s is unregistered\n",reg_details->name);
                strcpy(registered_clients[i], "***");
            }
            else{
                pthread_t new_thread;
                pthread_create(&new_thread, NULL, Client_Handler, (void*)reg_details);
                if(i==randomn)strcpy(registered_clients[randomn], reg_details->name);
            }
            // if(i==randomn)strcpy(registered_clients[randomn], reg_details->name);
            // reg_details->written=0;
            // shmdt(message);
            // pthread_mutex_unlock(&mutex);
        }
        reg_details->written=0;
        //printf("&&& %s\n",registered_clients[0]);
        randomn++;
        
        
        //pthread_mutex_unlock(&mutex);
    }
    shmdt(reg_details);
    shmctl(shmid, IPC_RMID, NULL);
    
    return 0;
}
