#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include "io.h"
// Set ints for the different producer/consumer types
#define HUMANDRIVER 0
#define AUTODRIVER 1
#define COSTMATCH 0
#define FASTMATCH 1
// Define the number of different types for producers/consumers
#define NUMTYPES 2
using namespace std;

void *consumer(void* queue);
void *producer(void* queue);


// Shared struct between threads, holds semaphores and other
// shared data needed by different threads
struct Buffer{
    sem_t bufferOpen;
    sem_t bufferSet;
    sem_t humanDriverOpen;
    sem_t mutex;
    sem_t allConsumed;
    sem_t maxProduce;
    sem_t maxConsume;
    int numRequests;
    // Queue to hold current available rides
    queue<int> myqueue;
    // Arrays to hold report info for each type of producers/consumers
    int consumed[NUMTYPES] = {};
    int *consumedTypes[NUMTYPES];
    int produced[NUMTYPES] = {};
    int inRequestQueue[NUMTYPES] = {};
    // Constructor for buffer, takes argument of number of requests to complete
    // Initializes all values to their defaults
    Buffer(int maxRequests){
        numRequests = maxRequests;
        // Tracks open space in request queue, max size of 12
        sem_init(&bufferOpen, 0, 12);
        // Tracks number of rides available to consume
        sem_init(&bufferSet, 0, 0);
        // Tracks open space for human drivers in request queue, max size of 4
        sem_init(&humanDriverOpen, 0, 4);
        // Allows exclusive access to buffer data
        sem_init(&mutex, 0, 1);
        // Locks main() until threads finish
        sem_init(&allConsumed, 0, 0);
        // Locks threads when max number of requests have been produced
        sem_init(&maxProduce, 0, maxRequests);
        // Locks threads when max number of requests have been consumed
        sem_init(&maxConsume, 0, maxRequests);
        // Initializes array to zeros
        for(int i = 0; i < NUMTYPES; i++){
            consumedTypes[i] = new int[NUMTYPES];
        }
    }
};

// Generic struct for producers, holds pointer to Buffer struct,
// type of producer and time for producer to sleep
struct Driver{
    struct Buffer *buffer;
    int producerType;
    int napTime;
    // Constructor for Driver, takes arguments of shared Buffer struct pointer,
    // producer type int, and time to sleep int
    Driver(struct Buffer *shared, int type, int sleepTime){
        buffer = shared;
        producerType = type;
        // Convert microseconds to miliseconds
        napTime = sleepTime * 1000;
    }
};

// Generic struct for consumers, holds pointer to shared Buffer struct,
// type of consumer, and time for producer to sleep
struct Matcher{
    struct Buffer *buffer;
    int requestType;
    int napTime;
    // Constructor for Matcher, takes arguments of Buffer struct pointer,
    // consumer type int, and time for consumer to sleep int
    Matcher(struct Buffer *shared, int type, int sleepTime){
        buffer = shared;
        requestType = type;
        // Convert microseconds to miliseconds
        napTime = sleepTime * 1000;
    }
};

