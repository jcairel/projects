#include <semaphore.h>
#include <queue>
#include <unistd.h>
#include "io.h"
using namespace std;
// Set ints for the producer types
#define HUMANDRIVER 0
// Define the number of different types for producers/consumers
#define NUMTYPES 2

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

