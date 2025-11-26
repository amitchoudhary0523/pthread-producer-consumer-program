
File "pthread_producer_consumer_program.c" uses pthread library to implement a
producer thread and a consumer thread. The producer thread puts items in the
items queue and the consumer thread consumes items from the items queue.

Since the items queue is used by both the producer thread and the consumer
thread, a mutex (from pthread library) is used for synchronization between these
two threads.

A condition variable (from pthread library) is also used for the following two
purposes:

    * If the items queue is full then the producer thread waits on the condition
      variable for the signal from the consumer thread signifying that the items
      queue is no more full because the consumer thread had consumed at least
      one item from the items queue and now the producer thread can again start
      putting item(s) in the items queue.

    * If the items queue is empty then the consumer thread waits on the
      condition variable for the signal from the producer thread signifying that
      the items queue is no more empty because the producer thread had put at
      least one item in the items queue and now the consumer thread can again
      start consuming item(s) from the items queue.

This program should link with the pthread library, so the option -lpthread
should be passed to gcc for compiling this program.

---- End of README ----
