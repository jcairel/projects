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
* PageTable holds the number of levels in page tree, the offset mask,
* pointer to first page table Level struct, and arrays of each levels
* bitmasks, bitshifts, and entry count values
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