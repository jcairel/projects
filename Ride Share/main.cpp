// Jack Cairel
// RedID 823197473
#include "main.h"

int main(int argc, char *argv[]){
    // Initialize optional args to defaults
    int numRequests = 120;
    int costMatchSleepTime = 0;
    int fastMatchSleepTime = 0;
    int humanSleepTime = 0;
    int autoSleepTime = 0;
    // Loop through optional arguments using getopt
    int Option;
    while ( (Option = getopt(argc, argv, "n:c:f:h:a:")) != -1) {
        // Print error msg and exit if invalid arguments
        if (atoi(optarg) < 0){
            cout << "Optional arguments must be a number greater than 0" << endl;
            exit(0);
        }
        switch (Option) { 
            case 'n': // Number of requests 
                numRequests = atoi(optarg);
                break; 
            case 'c': // Time for cost-saving dispatcher to sleep after requests 
                costMatchSleepTime = atoi(optarg);
                break;
            case 'f': // Time for fast-matching dispatcher to sleep
                fastMatchSleepTime = atoi(optarg);
                break;
            case 'h': // Time for human driver producer to sleep
                humanSleepTime = atoi(optarg);
                break; 
            case 'a': // Time for autonomous car producer to sleep
                autoSleepTime = atoi(optarg);
                break;
            default: 
                cout << "invalid argument" << endl;
                exit(0);
                break; 
        } 
    }

    // Initialize all structs
    struct Buffer *buffer = new Buffer(numRequests);
    struct Driver *human = new Driver(buffer, HUMANDRIVER, humanSleepTime);
    struct Driver *autonomous = new Driver(buffer, AUTODRIVER, autoSleepTime);
    struct Matcher *cost = new Matcher(buffer, COSTMATCH, costMatchSleepTime);
    struct Matcher *fast = new Matcher(buffer, FASTMATCH, fastMatchSleepTime);

    // Create all producer and consumer threads
    pthread_t humanDriverThread, autoCarThread, costSavingThread, fastMatchingThread;
    pthread_attr_t pthread_attributes;
    pthread_attr_init(&pthread_attributes);
    pthread_create(&humanDriverThread, &pthread_attributes, &producer,(void *) human);
    pthread_create(&autoCarThread, NULL, &producer, (void*) autonomous);
    pthread_create(&costSavingThread, NULL, &consumer, (void*) cost);
    pthread_create(&fastMatchingThread, NULL, &consumer, (void*) fast);

    // Wait for completion 
    sem_wait(&buffer->allConsumed);
    // Print final report
    io_production_report(buffer->produced, buffer->consumedTypes);
}