// Jack Cairel RED ID: 823197473
// Noah Bland RED ID: 821420932

#include "main.h"

/**
 * @brief Simulation of demand paging using a multi-level page tree/table with 
 * address translation caching (Translation lookaside buffer - TLB).
 */

int main(int argc, char *argv[]) {
    // Set optional args to default values, summary, max value, 0
    int output = SUMMARY;
    unsigned int numAddressToProcess = 0xffffffff;
    int tlbSize = 0;
    // Check for minimum arguments
    if (argc < 2){
        cerr << "Unable to open <<trace.tr>> " << endl;
        exit(1);
    }
    // Loop through optional arguments using getopt
    int Option;
    while ( (Option = getopt(argc, argv, "n:c:o:")) != -1) {
        switch (Option) { 
            case 'n': /* Number of addresses to process */  
                numAddressToProcess = atoi(optarg);
                break; 
            case 'c': /* TLB cache capacity */
                tlbSize = atoi(optarg);
                if (tlbSize <0){
                    cerr << "Cache capacity must be a number, greater than or equal to 0 " << endl;
                    exit(1);
                }
                break;
            case 'o': /* Produce map of pages */ 
                // Assign output to the corresponding output function
                if (string(optarg).compare("bitmasks") == 0){
                    output = BITMASKS;
                } else if (string(optarg).compare("virtual2physical") == 0){
                    output = VIRTUAL2PHYSICAL;
                } else if (string(optarg).compare("v2p_tlb_pt") == 0){
                    output = V2P_TLB_PT;
                } else if (string(optarg).compare("vpn2pfn") == 0){
                    output = VPN2PFN;
                } else if (string(optarg).compare("offset") == 0){
                    output = OFFSET;
                }
                // Summary is default 
                else {
                    output = SUMMARY;
                }
                break; 
            default: 
                // print something about the usage and die... 
                cerr << "Bad Flag" << endl;
                exit(0); 
                break; 
        } 
    } 

    /* first mandatory argument, optind is defined by getopt */ 
    int idx = optind;
    /* argv[idx] now has the first mandatory argument: the trace file  
    path*/
    FILE *tracef_h;
    tracef_h = fopen(argv[idx], "r");
    // Exit if trace file can not be opened
    if (!tracef_h){
        cerr << "unable to open <<" << argv[idx] << ">>" << endl;
        exit(1);
    }
    // Increment idx to get to page level arguments
    idx++;
    
    // Initialize pageTable struct (argc - idx) is the number of levels
    struct PageTable *pageTable = new PageTable(argc - idx);

    
    /* argv[idx+1] through argv[argc-1] would be the arguments for  
    specifying the number of bits for each page table level, one number  
    per each level */
    int count = 0;
    int bits = 0;
    int shift = 0;
    int entryCount = 0;
    unsigned int mask;
    // iterating through levels
    for(int depth = 0; idx + depth < argc; depth++){
        mask = 0xFFFFFFFF;
        //number of bits for each level
        bits = atoi(argv[idx + depth]);
        // Exit if bits is not a valid number
        if (bits < 1){
            cerr << "Level " << depth << " page table must be at least 1 bit" << endl;
            exit(1);
        }

        // Find the mask for this level 
        // If first level, no trailing bits, no change to mask
        if (!(count == 0)){
            // Remove any trailing bits
            mask = mask & ((1<<(32 - count)) - 1);
        }
        // Remove leading bits and add to pageTable
        mask = mask & ~((1<<(32 - count - bits)) - 1);
        pageTable->bitmask[depth] = mask;

        // Find  bit shift for this level and add to pageTable
        shift = 32 - count - bits;
        pageTable->bitshift[depth] = shift;
        // Find number of possible entries for this level and add to pageTable
        entryCount = 1<<bits;
        pageTable->entryCount[depth] = entryCount;
        // Add this levels bits to the total count
        count += bits;
    }
    // If pages too large, exit
    if (count > 28){
        cerr << "Too many bits used in page tables" << endl;
        exit(1);
    }

    // Get the mask to remove offset bits and add to pageTable
    unsigned int offsetMask = ~((1<<(32 - count)) - 1);
    pageTable->offsetMask = offsetMask;

    // Create first level of depth 0 in PageTable
    pageTable->levelptr = new Level(pageTable, 0);

    // If output == bitmasks, print bitmasks and exit
    if (output == 0){
        report_bitmasks(pageTable->levelCount, pageTable->bitmask);
        exit(0);
    }

    struct Map *mapping = nullptr;
    p2AddrTr mtrace; 
    unsigned int vAddr;
    unsigned int frame_idx = 0;
    int tlbHit = 0;
    int pageHit = 0;
    bool tlbHitCheck;
    bool pageHitCheck;
    uint32_t pa;
    struct TLB *tlb = new TLB(tlbSize);
    struct LRU *lru = new LRU();
    int iterations;

    // Get the first n memory accesses from trace file
    for (iterations = 0; iterations < numAddressToProcess; ++iterations){
        mapping = nullptr;
        tlbHitCheck = false;
        pageHitCheck = false;

        // Check if end of trace file
        if (!NextAddress(tracef_h, &mtrace)){
            break;
        }
        vAddr = mtrace.addr;
        // Check for virtual page mapping in tlb
        mapping = tlbCheck(tlb, lru, (vAddr & offsetMask), iterations);
        if(mapping)
        {
            tlbHitCheck = true;
            tlbHit+=1;
        }
        // If mapping not in tlb, check in page table
        if(!tlbHitCheck)
        {
            mapping = pageLookup(pageTable, vAddr);
            // Create mapping if not found, else increment pageHit and mark pageHitCheck as true
            if (!mapping){
                mapping = pageInsert(pageTable, vAddr, frame_idx);
                // Increment frame number 
                frame_idx++;
            } else {
                pageHit++;
                pageHitCheck = true;
            }
            // Check if there is a tlb
            if (tlbSize > 0){
                // Add mapping to tlb
                tlbInsert(tlb, lru, mapping, iterations);
            }
        } 

        // Take physical frame number, bitshift and add the offset to get physical address
        pa = (mapping->physical << (32-count)) + (vAddr & ~offsetMask);
        // Check which output is needed
        switch(output){
            case VIRTUAL2PHYSICAL:
                // Virtual address to physical mapping
                report_virtual2physical(vAddr, pa);
                break;
            case V2P_TLB_PT:
                // Virtual address to physical address, plus pageTable/tlb hit/misses
                report_v2pUsingTLB_PTwalk(vAddr, pa, tlbHitCheck, pageHitCheck);
                break;
            case VPN2PFN:
                // Virtual page number to physical frame number
                report_pagemap(pageTable->levelCount, pageTable->currentEntryCount, mapping->physical);
                break;
            case OFFSET:
                // Used to get the offset from each address
                hexnum(vAddr & ~pageTable->offsetMask);
                break;
            default:
                // Default case, do nothing
                break;
        }
    }
    if (output == SUMMARY){
        // page_size 2^offset bits
        int page_size = 1 << (32 - count);
        // Summary statistics
        report_summary(page_size, tlbHit, pageHit, iterations,
        frame_idx, pageTable->bytesUsed);
    }
}