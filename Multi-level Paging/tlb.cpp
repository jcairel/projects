#include "tlb.h"

/**
 * @brief Checks TLB for existing virtual address
 * 
 * Input of a virtual address will be compared with the virtual addresses stores in the TLB
 * struct instantiated in main. The LRU will be used to quicly find mappings acting as a cahce.
 * If LRU value is not found in cache but it found in the TLB, the LRU is updated. A nullptr is
 * returned if the virtual value does not correspond to any of the TLB entries.
 * 
 * @param  tlb: Pointer to TLB structure instantiated in main
 * @param  lru: Pointer to LRU structure instantiated in main
 * @param  virtualAddress: Virtual address being used to search the LRU and TLB
 * @param  time: Number of virtual addresses accessed correesponds to time and is used to update LRU
 * 
 * @return Map: Pointer to a map struct
 */

Map* tlbCheck(TLB *tlb, LRU *lru, unsigned int virtualAddress, int time)
{
    // iterating over current tlb entries
    for(int i = 0; i < tlb->currSize; i++)
    {
        // iterating over maps that are not null in TLB to prevent segmentation faults
        if(tlb->tlbMap[i] != nullptr)
        {
            // storing found TLB value in a Map struct
            struct Map *mapptr = tlb->tlbMap[i];
            // checking to see if virtual address exists in tlb
            if(mapptr->virt == virtualAddress)
            {
                bool foundInLru = false;
                // checking LRU to see if TLB Map is in cache
                for(int i = 0; i<lru->currentSize; i++)
                {
                    if(lru->lruMap[i]->virt == virtualAddress)
                    {
                        foundInLru = true;
                    }
                }
                // if virtual address found in TLB but not LRU, add value to LRU
                if(!foundInLru)
                {
                    // iterating through LRU to find least recently accessed value to replace
                    int lruReplaceIndex = 0;
                    int smallestTime = lru->lruMap[0]->time;
                    for(int i = 0; i<lru->currentSize; i++)
                    {
                        if(lru->lruMap[i]->time < smallestTime)
                        {
                            smallestTime = lru->lruMap[i]->time;
                            lruReplaceIndex = i;
                        }
                    }
                    // Map replaces correct index in LRU
                    lru->lruMap[lruReplaceIndex] = mapptr;
                }

                // updating access time of map found in TLB/LRU
                mapptr->time = time;
                //returns the map pointer if found in TLB
                return mapptr;
            }
        }
    }
    // nullptr is returned if virtual address is not found in TLB
    return nullptr;
}

/**
 * @brief Inserts virtual address into LRU and TLB
 * 
 * Inserts map into LRU and TLB when map does not already exist in LRU and TLB.
 * Uses LRU to replace 10th least recently used value in TLB if TLB has is greater than
 * 10 and has reached it's full capacity: otherwise map is inserted into TLB sequentially
 * while LRU updates
 * Updates LRU and TLB accordinly after map has been inserted.
 * 
 * @param  tlb: Pointer to TLB structure instantiated in main
 * @param  lru: Pointer to LRU structure instantiated in main
 * @param  mapping: Map holds data that is inserted into TLB and LRU
 * @param  time: Number of virtual addresses accessed correesponds to time and is used to update LRU
 */

void tlbInsert(TLB *tlb, LRU *lru, Map *mapping, int time)
{
    int index = 0;

    // Set mapping time to current access time (number of addresses accessed)
    mapping->time = time;

    // if TLB hasn't reached max size, entry is added to next open spot in TLB, LRU updated
    if(tlb->currSize < tlb->maxSize)
    {
        // setting next location in TLB to mapping
        tlb->tlbMap[tlb->currSize] = mapping;
        // Set mapping index to currSize of TLB, then increment currSize of TLB
        mapping->tlbIndex = tlb->currSize;
        tlb->currSize = tlb->currSize + 1;
        // fill LRU with inserted values, replacing LRU if ltb inserts exceed 10
        // fill next spot in LRU with mapping if LRU size is less than 10
        if(lru->currentSize<10)
        {
            lru->lruMap[lru->currentSize] = mapping;
            lru->currentSize++;
        }
        // finding the least reecently used value in LRU to replace
        else
        {
            int smallestTime = lru->lruMap[0]->time;
            int lruReplaceIndex = 0;
            for(int i = 0; i<lru->currentSize;i++)
            {
                if(lru->lruMap[i]->time < smallestTime)
                {
                    smallestTime = lru->lruMap[i]->time;
                    lruReplaceIndex = i;
                }
            }
            lru->lruMap[lruReplaceIndex] = mapping;
        }
    }

    // tlb is full so tlb value will replaced 10th least recently used value in TLB
    else
    {
        // checking for LRU entry with smallest access time
        int smallestTime = lru->lruMap[0]->time;
        // keeping track of what index to replace in LRU and TLB
        int replaceLruIndex = 0;
        int replaceTlbIndex = 0;

        // finding the smallest access time in LRU
        for(int i = 0; i < lru->currentSize; i++)
        {
            if(lru->lruMap[i]->time <= smallestTime)
            {
                smallestTime = lru->lruMap[i]->time;
                replaceLruIndex = i;
                replaceTlbIndex = lru->lruMap[i]->tlbIndex;
            }
        }
        // update TLB with correct index, set replaced index in mapping
        tlb->tlbMap[replaceTlbIndex] = mapping;
        mapping->tlbIndex = replaceTlbIndex;
        
        // update LRU with correct least recently used index
        lru->lruMap[replaceLruIndex] = mapping;

        // update access time of map
        mapping->time = time;

    }

}


// commented out print functions used for testing
/*
void printTLB(TLB *tlb)
{
    cout<< "TLB Table:"<<endl;
    for(int i = 0; i <tlb->currSize; i++)
    {
        cout << i << ": " << hex << tlb->tlbMap[i]->virt << endl;
    }
    cout<<endl;

}

void printLRU(LRU *lru)
{
    cout << endl;
    cout << "LRU Table:" << endl;

    for(int i = 0; i <lru->currentSize; i++)
    {
        cout << i << ": " << hex << lru->lruMap[i]->virt << " time: " << dec << lru->lruMap[i]->time <<endl;
    }

    cout << endl;

}
*/