#include "producer.h"
// Generic function for producers, takes argument of Driver struct
void *producer(void* producer){
    struct Driver *driver = (struct Driver*)producer;
    struct Buffer *buffer = driver->buffer;
    // Constantly trying to produce
    while(true){
        // Lock if already hit max production
        sem_wait(&buffer->maxProduce);
        // If human driver, locked until open spot for human
        if(driver->producerType == HUMANDRIVER){
            sem_wait(&buffer->humanDriverOpen);
        }
        // Add ride, locked until room in queue
        sem_wait(&buffer->bufferOpen);
        // Get access to queue
        sem_wait(&buffer->mutex);
        // Push type of ride to queue, signal ride in buffer
        buffer->myqueue.push(driver->producerType);
        sem_post(&buffer->bufferSet);
        // Update records
        buffer->inRequestQueue[driver->producerType] +=1;
        buffer->produced[driver->producerType] +=1;
        // Print production output
        io_add_type((RequestType)driver->producerType, buffer->inRequestQueue, buffer->produced);
        // Release access to queue
        sem_post(&buffer->mutex);
        // Sleep after completion
        usleep(driver->napTime);
    }

}
