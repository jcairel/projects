#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string> 
#define BITMASKS 0         // Define int for each output
#define VIRTUAL2PHYSICAL 1 // Easier than working with strings
#define V2P_TLB_PT 2
#define VPN2PFN 3
#define OFFSET 4
#define SUMMARY 5 
#include "tracereader.h"
#include "output_mode_helpers.h"
using namespace std;


/*
* Map struct holds virtual page and physical frame numbers, along with 
* the mappings index in tlb and its last access time
*/
struct Map{
    unsigned int virt;
    unsigned int physical;
    unsigned int tlbIndex;
    unsigned int time;
    /*
    * Map constructor with arguments to set virtual page and physical frame,
    * also sets time accessed to default 0
    */
    Map(unsigned int page, unsigned int frame){
        virt = page;
        physical = frame;
        time = 0;
    }
    Map(); 
};

/*
* LRU struct holds its current size and array of 10 pointers to
* mapping structs last used by TLB
*/
struct LRU{
    int currentSize;
    struct Map *lruMap[10];
    /*
    * LRU constructor sets currentSize to 0 and sets all map pointers
    * in lruMap to nullptr
    */
    LRU()
    {
        currentSize = 0;
        for(int i=0; i<10; i++)
        {
            lruMap[i] = nullptr;
        }
    }
};

/*
* TLB struct holds its current size, max size, and array of pointers to 
* map structs
*/
struct TLB
{
    int currSize;
    int maxSize;
    struct Map **tlbMap;
    /*
    * TLB constructor that takes argument of its max size, sets
    * maxSize and instansiates tlbMap to correct size before 
    * setting all pointers to nullptr and setting default currSize of 0
    */
    TLB(int tlbSize)
    {
        maxSize = tlbSize;
        tlbMap = new struct Map*[tlbSize];
        currSize = 0;
        for(int i=0; i<maxSize; i++)
        {
            tlbMap[i] = nullptr;
        }
    }
};
/*
* Level holds its depth in the page table, pointer to parent PageTree struct,
* and array of pointers to next levels or mappings
*/
struct Level{
    int depth;
    struct PageTable *tableptr;
    struct Level **nextLevelptr;
    struct Map **mapping;
    /*
    * Level constructor, takes arguments of pageTable pointer and int entryCount
    * Allocates correct size of nextLevelptr and mapping arrays, then sets
    * them to nullptrs
    */
    Level(struct PageTable *table, int levelDepth);
    Level();
};
/*
* PageTable holds the number of levels in page tree, number of bytes
* used by the table, the offset mask, pointer to first page table Level struct,
* and arrays of each levels bitmasks, bitshifts, and entry count values
*/
struct PageTable{
    int levelCount;
    unsigned int *bitmask;
    int *bitshift;
    int *entryCount;
    unsigned int *currentEntryCount; // keeping track of index within level of pagetable
    unsigned int offsetMask;
    int bytesUsed;
    struct Level *levelptr;
    /*
    * PageTable constructor sets the number of levels in the page table,
    * sets the first level pointer to nullptr, and allocates the correct 
    * size of its bitshift, bitmask, entryCount arrays before setting their values to 0
    */
    PageTable(int levels);
};


Map* pageLookup(PageTable *pageTable, unsigned int virtualAddress);
Map* pageInsert(PageTable *pageTable, unsigned int virtualAddress, unsigned int frame);
void tlbInsert(TLB *tlb, LRU *lru, Map *mapping, int time);
Map* tlbCheck(TLB *tlb, LRU *lru, unsigned int virtualAddress, int time);
void printTLB(TLB *tlb);
void printLRU(LRU *tlb);