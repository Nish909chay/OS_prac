/*
Multithreading Library
You're expected to build your own threading library (like pthread) and implement three threading models:
1:1         Each user thread maps to one kernel thread. You use clone() or pthread_create() and let the OS handle scheduling.
Many:1      Many user threads map to one kernel thread. You implement your own scheduler — if one thread blocks, all threads block.
Many:Many   Many user threads map to many kernel threads. Your library manages which user thread runs on which kernel thread.
*/

#ifndef MYTHREAD_H
#define MYTHREAD_H
#define STACK_SIZE 4096

#include <setjmp.h>   // think of it as placing a bookmark
#include <stdio.h>
#include <stdlib.h>

jmp_buf main_context;  // Saves main's context to return later

// Step 1: Define thread state enum for clarity
typedef enum {
    READY,
    RUNNING,
    DEAD
} thread_state;

// Step 2: Define the thread structure
typedef struct my_thread {
    int id;
    void *stack;                                 // Allocated stack memory
    void *arg;                                   // Argument to pass to the thread function
    void (*start_routine)(void *);               // Function pointer to the thread's entry point
    jmp_buf context;                             // Execution context (registers, stack pointer)
    thread_state state;                          // Thread lifecycle state
    struct my_thread *next;                      // For the ready queue (linked list)
} my_thread;

my_thread *current_thread = NULL;  // Currently running thread
my_thread *thread_head = NULL;     // Head of the ready queue


void my_thread_create(my_thread **head, void (*function)(void *), void *arg)
{
    /*
    - Allocates and initializes a new thread
    - Sets its stack, function, and state
    - Adds it to the end of the ready queue
    - Saves its context with setjmp (not restored immediately)
    */
    my_thread *new = (my_thread*)malloc(sizeof(struct my_thread));
    if (!new) {
        perror("Thread memory allocation failed");
        exit(1);
    }

    new->stack = malloc(STACK_SIZE);
    if (!new->stack) {
        perror("Failed to allocate stack");
        free(new);
        exit(1);
    }

    static int id_counter = 1;
    new->id = id_counter++;
    new->start_routine = function;
    new->arg = arg;
    new->state = READY;
    new->next = NULL;

    if (setjmp(new->context) == 0) {
        // Initial save; will return 0
    }

    if (*head == NULL) {
        *head = new;
    } else {
        my_thread *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new;
    }
}


void my_thread_yield()
{
    /*
    - Saves current thread's context using setjmp
    - Moves it to the end of the queue
    - Picks the next thread in the ready queue
    - Transfers control to it using longjmp
    */
    if (setjmp(current_thread->context) == 0)
    {
        my_thread *temp = thread_head;
        while (temp->next != NULL)
            temp = temp->next;

        temp->next = current_thread;        // Move to end of queue
        current_thread->state = READY;

        thread_head = thread_head->next;    // Remove from front
        current_thread->next = NULL;

        current_thread = thread_head;       // Switch to next thread
        current_thread->state = RUNNING;
        longjmp(current_thread->context, 1);
    }
}


void my_thread_run(my_thread **head)
{
    /*
    - Runs the thread at the head of the queue
    - If it's DEAD, frees it and moves on
    - Otherwise, sets it to RUNNING
    - Saves main context using setjmp
    - Transfers control to thread using longjmp
    */
    while (*head && (*head)->state == DEAD) {
        my_thread *dead = *head;
        *head = dead->next;
        free(dead->stack);
        free(dead);
    }

    if (*head == NULL) {
        printf("No threads to run.\n");
        return;
    }

    current_thread = *head;
    current_thread->state = RUNNING;

    if (setjmp(main_context) == 0) {
        longjmp(current_thread->context, 1);  // Switch to thread
    }
}

void my_thread_exit()
{
    /*
    - Marks current thread as DEAD
    - Returns control back to main using longjmp
    */
    current_thread->state = DEAD;
    longjmp(main_context, 1);  // Return to main thread runner
}

void print_numbers(void *arg) {
    int id = *((int*)arg);
    for (int i = 0; i < 5; i++) {
        printf("Thread %d: %d\n", id, i);
        my_thread_yield();
    }
    my_thread_exit();
}

int main() {
    int arg1 = 1, arg2 = 2;

    my_thread_create(&thread_head, print_numbers, &arg1);
    my_thread_create(&thread_head, print_numbers, &arg2);

    while (thread_head != NULL) {
        my_thread_run(&thread_head);
    }

    return 0;
}

#endif
