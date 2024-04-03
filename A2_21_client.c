#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

#define SHM_SIZE sizeof(msg)


int current=0;
int randomn=0;
typedef struct {
    int written;
    int index;
    char name[50];
    int operation;
    int op1;
    int op2;
} msg;

typedef struct {
    int written;
    int n;
    int type;
    char name[50];
    int operation;
    int op1;
    int op2;
    int key;
    int ans;
} reg;

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
void f(reg *x){
    int result;
    switch (x->operation) {
                case 1:
                    result = x->op1 + x->op2;
                    printf("=> Result of request of client %s is: %d\n", x->name,result);
                    x->ans=result;
                    break;
                case 2:
                    result = x->op1 - x->op2;
                    printf("=> Result of request of client %s is: %d\n", x->name,result);
                    x->ans=result;
                    break;
                case 3:
                    result = x->op1 * x->op2;
                    printf("=> Result of request of client %s is: %d\n", x->name,result);
                    x->ans=result;
                    break;
                case 4:
                    float answer;
                    answer=(float) x->op1/x->op2;
                    printf("=> Result of request of client %s is: %.2f\n", x->name,answer);
                    x->ans=answer;
                    break;
                case 5:
                    //int ans;
                    if(x->op1%2==0){
                        printf("=> Result of request of client %s is: EVEN\n", x->name);
                        x->ans=1;
                    }
                    else{
                        printf("=> Result of request of client %s is: ODD\n",x->name);
                        x->ans=0;
                    }
                    break;
                case 6:
                    int ans;
                    ans=is_prime(x->op1);
                    if(ans==1){
                        printf("=> Result of request of client %s is: YES it is a Prime\n", x->name);
                        x->ans=1;
                    }
                    else{
                        printf("=> Result of request of client %s is: NO it is not a Prime\n", x->name);
                        x->ans=0;
                    }
                    break;
                default:
                    //printf("g\n");
                    printf("Invalid operation\n");
                    x->ans=-1;
                   break;
            }
    
    return;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    // Shared memory key
    key_t key = ftok("random", 100);
    int shmid = shmget(key, SHM_SIZE, 0666);
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

    int choice = 0;
    while (choice != -1) {
        
        printf("---------------------------------------------------------------------\n");
        printf("Select an option:\n");
        printf("1. Register\n");
        printf("2. Request\n");
        printf("3. Unregister\n");
        printf("-1. Shutdown\n");
        scanf("%d", &choice);

        if (choice == 2) {
            // Acquire the mutex lock
            pthread_mutex_lock(&mutex);
            int x;
            printf("Enter name: ");
            scanf("%49s", reg_details->name);
            printf("Enter the key: ");
            scanf("%d", &reg_details->key);
            reg_details->type=1;
            printf("Enter request type (1-arithametic(+,-,*,/), 2-EvenOrOdd, 3-IsPrime): ");
            scanf("%d", &x);

            if(x==1){
                printf("Enter operation number (1-addition || 2-subtraction || 3-multiplication || 4-division): ");
                scanf("%d", &reg_details->operation);
                printf("Enter operand 1: ");
                scanf("%d", &reg_details->op1);
                printf("Enter operand 2: ");
                scanf("%d", &reg_details->op2);
            }
            else if(x==2){
                reg_details->operation=5;
                printf("Enter number: ");
                scanf("%d", &reg_details->op1);
                reg_details->op2=0;
            }
            // else{
            //     reg_details->operation=6;
            //     printf("Enter number: ");
            //     scanf("%d", &reg_details->op1);
            //     reg_details->op2=0;
            // }
            else if(x==3){
                reg_details->operation=6;
                printf("Enter number: ");
                scanf("%d", &reg_details->op1);
                reg_details->op2=0;
            }
            else{
                reg_details->operation=7;
            }
            
            reg_details->written=1;

            // Release the mutex lock
            pthread_mutex_unlock(&mutex);

            printf("Request sent......\n");
            f(reg_details);
            
        } else if (choice == 1) {
            //Acquire the mutex lock
            pthread_mutex_lock(&mutex);

            printf("Enter name: ");
            scanf("%s", reg_details->name);
            reg_details->operation = 0;
            reg_details->op1 = 0;
            reg_details->op2 = 0;
            reg_details->type=1;
            reg_details->n=0;
            reg_details->written=1;
            pthread_mutex_unlock(&mutex);
            //printf("%d",reg_details->n);
            // while (reg_details->n == 0) {
            //     pthread_mutex_unlock(&mutex);
            //     printf("**");
            //     usleep(100000);
            //     pthread_mutex_lock(&mutex);
            // }
            pthread_mutex_lock(&mutex);
            if(randomn==0)printf("Key for client %s is %d\n", reg_details->name,1000);
            //else printf("Key for client %s is %d\n", reg_details->name,reg_details->key+1);
            else printf("Key for client %s is %d\n", reg_details->name,1000+randomn);
            reg_details->n=0;
            randomn++;
            pthread_mutex_unlock(&mutex);

        } else if (choice == 3) {
            //Acquire the mutex lock
            pthread_mutex_lock(&mutex);

            printf("Enter name: ");
            scanf("%s", reg_details->name);
            printf("Enter the key: ");
            scanf("%d", &reg_details->key);
            reg_details->operation = -1;
            reg_details->op1 = 0;
            reg_details->op2 = 0;
            reg_details->type=-1;
            reg_details->written=1;
            // Release the mutex lock
            pthread_mutex_unlock(&mutex);
        }
        else{
            printf("Treminating...\n");
            break;
        }
        
    }

    // Detach from shared memory
    shmdt(reg_details);

    return 0;
}
