
/*
 * License:
 *
 * This file has been released under "unlicense" license
 * (https://unlicense.org).
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute
 * this software, either in source code form or as a compiled binary, for any
 * purpose, commercial or non-commercial, and by any means.
 *
 * For more information about this license, please visit - https://unlicense.org
 */

/*
 * Author: Amit Choudhary
 * Email: amitchoudhary0523 AT gmail DOT com
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#include <errno.h>
#include <pthread.h>

#define NUM_ITEMS 10

#define PTHREAD_MUTEX_LOCK pthread_mutex_lock
#define PTHREAD_MUTEX_UNLOCK pthread_mutex_unlock

#define PTHREAD_COND_WAIT pthread_cond_wait
#define PTHREAD_COND_BROADCAST pthread_cond_broadcast

struct common_vars {
    pthread_mutex_t *mutex;
    pthread_cond_t *cond_var;
    int items_queue[NUM_ITEMS];
    int current_num_items_in_queue;
};

static void check_retval_and_exit_on_error(int retval, const char *msg);
static void *producer(void *arg);
static void *consumer(void *arg);

static void check_retval_and_exit_on_error(int retval, const char *msg)
{

    if (retval != 0) {
        printf("\nError: %s. The error number is: %d. Now, there is no point in"
               " running this program. This program is now exiting..\n\n",
               ((msg != NULL) ? msg : ""), retval);
        exit(1);
    }

    return;

} // end of function check_retval_and_exit_on_error()

static void *producer(void *arg)
{

    struct common_vars *cv_ptr = arg;
    int current_index_in_queue = 0;
    int num = 0;
    int to_yield = 0;
    int retval = -1;

    if (arg == NULL) {
        printf("\nError: 'arg' argument is NULL in producer thread. Now, there"
               " is no point in running this program. This program is now"
               " exiting..\n\n");
        exit(1);
    }

    srand((unsigned int)(time(NULL)));

    while (1) {

        num = rand();

        retval = PTHREAD_MUTEX_LOCK(cv_ptr->mutex);

        check_retval_and_exit_on_error(retval, "pthread_mutex_lock() failed in"
                                               " producer thread");

        while (cv_ptr->current_num_items_in_queue == NUM_ITEMS) {

            printf("\nProducer is going to sleep because the queue is full.."
                   "\n\n");
            retval = PTHREAD_COND_WAIT(cv_ptr->cond_var, cv_ptr->mutex);
            check_retval_and_exit_on_error(retval, "pthread_cond_wait() failed"
                                                   " in producer thread");
        }

        cv_ptr->items_queue[current_index_in_queue] = num;
        current_index_in_queue = (current_index_in_queue + 1) % NUM_ITEMS;
        cv_ptr->current_num_items_in_queue++;

        printf("Producer added number %d to the queue. Current number of items"
               " in the queue = %d\n", num, cv_ptr->current_num_items_in_queue);

        retval = PTHREAD_MUTEX_UNLOCK(cv_ptr->mutex);

        check_retval_and_exit_on_error(retval, "pthread_mutex_unlock() failed"
                                               " in producer thread");

        retval = PTHREAD_COND_BROADCAST(cv_ptr->cond_var);

        check_retval_and_exit_on_error(retval, "pthread_cond_broadcast() failed"
                                               " in producer thread");

        // To introduce some randomness, we will do sched_yield() here in some
        // cases.
        to_yield = rand();
        to_yield = to_yield % 100;
        if (to_yield > 25) {
            retval = sched_yield();
        }
        if (retval != 0) {
            printf("\nError: sched_yield() failed in producer thread. The error"
                   " number is: %d\n\n", errno);
        }

    } // end of while (1) loop

    return NULL;

} // end of function producer()

static void *consumer(void *arg)
{

    struct common_vars *cv_ptr = arg;
    int current_index_in_queue = 0;
    int num = 0;
    int to_yield = 0;
    int retval = -1;

    if (arg == NULL) {
        printf("\nError: 'arg' argument is NULL in consumer thread. Now, there"
               " is no point in running this program. This program is now"
               " exiting..\n\n");
        exit(1);
    }

    while (1) {

        retval = PTHREAD_MUTEX_LOCK(cv_ptr->mutex);

        check_retval_and_exit_on_error(retval, "pthread_mutex_lock() failed in"
                                               " consumer thread");

        while (cv_ptr->current_num_items_in_queue == 0) {

            printf("\nConsumer is going to sleep because the queue is empty.."
                   "\n\n");
            retval = PTHREAD_COND_WAIT(cv_ptr->cond_var, cv_ptr->mutex);
            check_retval_and_exit_on_error(retval, "pthread_cond_wait() failed"
                                                   " in consumer thread");
        }

        num = cv_ptr->items_queue[current_index_in_queue];
        current_index_in_queue = (current_index_in_queue + 1) % NUM_ITEMS;
        cv_ptr->current_num_items_in_queue--;

        printf("Consumer removed number %d from the queue. Number of items"
               " remaining in the queue = %d\n", num,
               cv_ptr->current_num_items_in_queue);

        retval = PTHREAD_MUTEX_UNLOCK(cv_ptr->mutex);

        check_retval_and_exit_on_error(retval, "pthread_mutex_unlock() failed"
                                               " in consumer thread");

        retval = PTHREAD_COND_BROADCAST(cv_ptr->cond_var);

        check_retval_and_exit_on_error(retval, "pthread_cond_broadcast() failed"
                                               " in consumer thread");

        // To introduce some randomness, we will do sched_yield() here in some
        // cases.
        to_yield = rand();
        to_yield = to_yield % 100;
        if (to_yield > 25) {
            retval = sched_yield();
        }
        if (retval != 0) {
            printf("\nError: sched_yield() failed in consumer thread. The error"
                   " number is: %d\n\n", errno);
        }

    } // end of while (1) loop

    return NULL;

} // end of function consumer()

int main(void)
{

    struct common_vars cv;
    pthread_t ptid = 0; // producer thread id
    pthread_t ctid = 0; // consumer thread id
    int retval = -1;

    memset(&cv, 0, sizeof(cv));

    cv.mutex = calloc(1, sizeof(*(cv.mutex)));

    if (cv.mutex == NULL) {
        printf("\nError: No memory available. Exiting..\n\n");
        exit(1);
    }

    cv.cond_var = calloc(1, sizeof(*(cv.cond_var)));

    if (cv.cond_var == NULL) {
        free(cv.mutex);
        printf("\nError: No memory available. Exiting..\n\n");
        exit(1);
    }

    retval = pthread_create(&ptid, NULL, producer, &cv);

    if (retval != 0) {
        free(cv.mutex);
        free(cv.cond_var);
        printf("\nError: Could not create producer thread. Exiting..\n\n");
        exit(1);
    }

    retval = pthread_create(&ctid, NULL, consumer, &cv);

    if (retval != 0) {
        free(cv.mutex);
        free(cv.cond_var);
        printf("\nError: Could not create consumer thread. Exiting..\n\n");
        exit(1);
    }

    retval = pthread_join(ptid, NULL);

    if (retval != 0) {
        printf("\nError: Joining on producer thread returned error. Error"
               " number is: %d\n\n", retval);
    }

    retval = pthread_join(ctid, NULL);

    if (retval != 0) {
        printf("\nError: Joining on consumer thread returned error. Error"
               " number is: %d\n\n", retval);
    }

    free(cv.mutex);
    free(cv.cond_var);

    return 0;

} // end of function main()

