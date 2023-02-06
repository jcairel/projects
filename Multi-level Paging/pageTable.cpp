#include "pageTable.h"
// Level constructor, takes arguments of pageTable pointer and int entryCount
// Allocates correct size of nextLevelptr and mapping arrays, then sets
// them to nullptrs
Level::Level(struct PageTable *table, int levelDepth){
    tableptr = table;
    depth = levelDepth;
    // Build mapping array if leaf node, else build level array
    if (depth == (tableptr->levelCount - 1)){
        mapping = new struct Map*[tableptr->entryCount[depth]];
        // Initialize arrays to nullptr
        for (int i = 0; i < tableptr->entryCount[depth]; i++){
            mapping[i] = nullptr;
        }
        // Add size of array to pageTable
        tableptr->bytesUsed += (sizeof(mapping) * tableptr->entryCount[depth]);
    } else{
        nextLevelptr = new Level*[tableptr->entryCount[depth]];
        // Initialize arrays to nullptr
        for (int i = 0; i < tableptr->entryCount[depth]; i++){
            nextLevelptr[i] = nullptr;
        }
        // Add size of array to pageTable
        tableptr->bytesUsed += (sizeof(mapping) * tableptr->entryCount[depth]);
    }
}

// PageTable constructor sets the number of levels in the page table,
// sets the first level pointer to nullptr, and allocates the correct 
//size of its bitshift, bitmask, entryCount arrays before setting their values to 0
PageTable::PageTable(int levels){
    levelCount = levels;
    bitmask = new unsigned int[levels];
    bitshift = new int[levels];
    entryCount = new int[levels];
    currentEntryCount = new unsigned int[levels];
    levelptr = nullptr;
    bytesUsed = 0;
    // Initialize arrays to 0
    for (int i = 0; i < levels; i++){
        bitmask[i] = 0;
        bitshift[i] = 0;
        entryCount[i] = 0;

    }
}

/**
 * @brief Searching for virtual address in PageTable
 * 
 * Return mapping of virtual address to physical frame
 * Takes arguments of a PageTable pointer and a virtual address
 * 
 * @param pageTable: 
 * @param virtualAddress: 
 * 
 * @return Map: Pointer to a map struct found in PageTable
 */

Map* pageLookup(PageTable *pageTable, unsigned int virtualAddress){
    // Get the first level of PageTable
    struct Level *levelptr = pageTable->levelptr;
    unsigned int temp;
    // Walk through the levels of PageTable
    for (int depth = 0; depth < pageTable->levelCount; depth++){
        // Apply the bitmask and bitshift for the PageTable level
        temp = (virtualAddress & pageTable->bitmask[depth]) >> pageTable->bitshift[depth];
        pageTable->currentEntryCount[depth] = temp;
        // Check if current level is a leaf Node
        if (depth == (pageTable->levelCount - 1)){
            // Check if correct mapping exists, else return nullptr
            if (levelptr->mapping[temp]){
                // Return the correct mapping to physical frame
                return levelptr->mapping[temp];
            } else return nullptr;
        } 
        // Check if next page level exists
        if (levelptr->nextLevelptr[temp]){
            // Assign levelptr to the correct next page level
            levelptr = levelptr->nextLevelptr[temp];
        } else {
            // Page mapping does not exist, return nullptr
            return nullptr;
        }
    }
    // Page mapping not found, return nullptr
    return nullptr;

};

/**
 * @brief Inserting virtualAdress into PageTable
 * 
 * Adds a page mapping when pageLookup fails
 * Takes arguments of a PageTable pointer, virtual address,
 * and frame number to map the page to
 * 
 * @param pageTable: PageTable struct instantated in main holding info for the structure of the page table
 * @param virtualAddress: virtual address informatino used to make pageTable
 * @param frame: int that keeps track of the number of maps that have been created
 * 
 * @return Map: Pointer to a map struct insertef in PageTable
 */

Map* pageInsert(PageTable *pageTable, unsigned int virtualAddress, unsigned int frame){
    // Get the first level of PageTable
    Level *levelptr = pageTable->levelptr;
    unsigned int temp;
    for (int depth = 0; depth < pageTable->levelCount; depth++){
        // Apply the bitmask and bitshift for the PageTable level
        temp = (virtualAddress & pageTable->bitmask[depth]) >> pageTable->bitshift[depth];
        pageTable->currentEntryCount[depth] = temp;
        // Check if level is a leaf Node
        if (depth == (pageTable->levelCount - 1)){
            // Create proper Mapping for page
            levelptr->mapping[temp] = new Map(virtualAddress & pageTable->offsetMask, frame); // Add unsigned int frame
            // Return created mappping
            return levelptr->mapping[temp];
        }
        // Check to see if next level exists with temp value
        if(!levelptr->nextLevelptr[temp]){
            // Instatiate next level
            levelptr->nextLevelptr[temp] = new Level(pageTable, depth + 1);
        }
        // Move to next level pointer
        levelptr = levelptr->nextLevelptr[temp];
    }
}