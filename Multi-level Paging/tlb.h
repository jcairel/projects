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