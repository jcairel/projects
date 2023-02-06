#include "consumer.h"
// Generic function for consumers, takes an argument of a Matcher struct
void *consumer(void* consumer){
    struct Matcher *match = (struct Matcher*)consumer;
    struct Buffer *buffer = match->buffer;
    int rideType;
    // Constatly be trying to consume from queue
    while(true){
        // Lock if already hit max consumption
        sem_wait(&buffer->maxConsume);
        // Take ride, locked until open ride available
        sem_wait(&buffer->bufferSet);
        // Get access to queue
        sem_wait(&buffer->mutex);
        // Pop ride from queue, release buffer slot
        rideType = buffer->myqueue.front();
        buffer->myqueue.pop();
        sem_post(&buffer->bufferOpen);
        // If human driver, release human driver slot
        if(rideType == HUMANDRIVER){
            sem_post(&buffer->humanDriverOpen);
        }
        // Update records
        buffer->inRequestQueue[rideType] -=1;
        buffer->consumed[rideType] +=1;
        buffer->consumedTypes[match->requestType][rideType] +=1;
        // Print consumption output
        io_remove_type((Consumers)match->requestType, (RequestType)rideType, buffer->inRequestQueue, buffer->consumed);
        // If last to consume, signal for main to finish, sum consumed array to get total consumed
        if(buffer->myqueue.empty() && (buffer->consumed[0] 
        + buffer->consumed[1] == buffer->numRequests)){
            sem_post(&buffer->allConsumed);
        }
        // Release access to queue
        sem_post(&buffer->mutex);        
        // Sleep after completion
        usleep(match->napTime);
    }

}
