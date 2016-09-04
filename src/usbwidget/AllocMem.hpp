/*****************************************************************************\
* AllocMem.hpp -- NeoWPS * USB Widget                                         *
*                                                                             *
* Copyright (c) RDP Engineering                                               *
*                                                                             *
* Author: Ben Rietbroek <rousseau.os2dev@gmx.com>                             *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,                *
*   MA 02110-1301, USA.                                                       *
*                                                                             *
\*****************************************************************************/


/**
 * This is the public header for it's corresponding source.
 */


#ifndef     __ALLOCMEM_HPP__
#define     __ALLOCMEM_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "ModuleGlobals.hpp"
//#include  "USBHelpers.hpp"

/* Reroute malloc() to our custom_malloc() */
#define     malloc  custom_malloc

/* Reroute free() to our custom_free() */
#define     free    custom_free

/* Signature for the named shared memory */
#define     SHMEM_SIGNATURE 0x0524EFFA      // (os2-for-effa -- OS24EVER :-)

/* Signature for the Widget State */
#define     WGTST_SIGNATURE 0xCAFEDADA


typedef enum    _POOLTYPE {
    PT_USE_OS2,
    PT_USE_CRT,
    PT_USE_NAMED
} POOLTYPE;
typedef POOLTYPE*   PPOOLTYPE;


/*
// This is the all important pointer to the dynamically allocated memory
// to hold the Widget State. It is either obtained from the DAEMON or allocated
// locally.
*/
extern  void*       masterpool;




/*
// Memory for the widget is either allocated from named shared memory, if that
// was found, or from the local heap if it was not. Either way, DosSubAllocMem
// is used to allocate memory objects from the pool.
// The pool has the folowing layout:
// -----> BASE OF POOL
//          MASTERMEMPOOL structure used for custom housekeeping (64 bytes)
// -----> BASE OF SUB-POOL (64 bytes)
//          SUBMEMPOOL structure by DosSubSetMem for housekeeping
// -----> FIRST AVAILABLE ADDRESS IN SUB-POOL
//          SUBMEMBLOCK #0 (8 bytes)
// ----->       USER POINTER #0 (n bytes of DosSubAllocMem allocated space)
//          SUBMEMBLOCK #1 (8 bytes)
// ----->       USER POINTER #1 (n bytes of DosSubAllocMem allocated space)
//          ....
//          ....
//          ....
//          SUBMEMBLOCK #n (8 bytes)
// ----->       USER POINTER #n (n bytes of DosSubAllocMem allocated space)
// ----->   END OF SUB-POOL *AND* OF MASTER POOL
//
// This is how is works:
// The Master Pool is allocated either by the DAEMON and shared or by the
// if no named shared memory could be found.
// The MASTERMEMPOOL structure is then written at the start, occupying 64 bytes.
//~ // If the DAEMON allocated the master-pool then the is_shared flag is set.
//~ // If the Widget allocated the master-pool then the is_shared flag is cleared.
//~ // This enables the Widget to find out what kind of memory it is using.
// The base pointer is set to the area initialized by DosSubSetMem.
// Note that the first 64-bytes of the submempool are used for housekeeping.
//
// Following the MASTERMEMPOOL is the area used by DosSubAllocMem and it is
// initialized by DosSubSetMem. The DAEMON will always use DOSSUB_INIT but the
// widget only uses DOSSUB_INIT if the named shared memory could not be found.
// Otherwise it will use DosSubSetMem to attach to the named shared memory.
//
// The overridden malloc function will used DosSubAllocMem to allocate from the
// sub-pool. Each allocation is initialized with a SUBMEMBLOCK structure so
// DosSubFreeMem (used by the overridden free function) knows how many
// bytes to free up.
*/
//~: MASTERMEMPOOL structure, 64 bytes in size
typedef struct  _MASTERMEMPOOL {
    unsigned long   signature;      // Signature to indicate a valid allocation
    unsigned long   poolsize;       // Size of the master pool
    POOLTYPE        pooltype;       // Type of pool allocated
    void*           submempool;     // Base of the block to sub-allocate
    void*           pwidgetstate;   // Pointer to the widget state structure
    unsigned long   filler[11];     // Reserved
} MASTERMEMPOOL, *PMASTERMEMPOOL;

/*
// This structure is a placeholder for the DosSubSetMem housekeeping.
// It is unknown what is store here, but most probably it will be pointers
// to lists of free blocks.
*/
//~: SUBMEMPOOL structure, 64 bytes in size
typedef struct  _SUBMEMPOOL {
    void*   unknown[16];
} SUBMEMPOOL, *PSUBMEMPOOL;


/*
// DosSubFreeMem needs a size to free a sub allocated block.
// If this size is bigger that the size originally used
// to allocate the block, things can go wrong.
// The size freed is stored in the free block and a subsequent
// allocation might get it assigned while the true free space
// is actually smaller; the size used in the first allocation.
// So we need to assure that a block is always freed by the size
// it was allocated with. Therefore we use a structure that is at the
// beginning of each sub allocated block. Since sub allocations are
// always a multiple of 8 we might as well use the other 4 bytes to store
// the base address of the block that holds all sub allocations.
// At offset 0 we store the address of the base and at offset 4 we store
// the size of the sub allocated block.
// So we wrap DosSubAllocMem and DosSubFreeMem to handle this behavior.
*/
//~: SUBMEMBLOCK structure, 8 bytes in size
typedef struct  _SUBMEMBLOCK {
    void*           base;       // Address of block allocated with DosAllocMem that is sub allocated.
    unsigned long   size;       // Size of the block allocated with DosSubAlloc.
} SUBMEMBLOCK, *PSUBMEMBLOCK;




/* Prototypes */
// Pool to sub allocate
void*       allocate_master_pool(POOLTYPE pooltype);    // Allocation of master pool
/// init_master_pool must also be created
void        free_master_pool(void* pool);               // Deallocation of master pool

// Standard C runtime functions encapsulated
void*   crt_malloc(size_t size);        // The standard malloc()
void    crt_free(void* ptr);            // The standard free()

// Overrides
void*   custom_malloc(size_t size);     // Our malloc() override
void    custom_free(void* ptr);         // Our free() override

// Debug funtions
void    custom_malloc_debug(void* ptr, size_t size);// Debug function for malloc()
void    custom_free_debug(void* ptr);               // Debug function for free()
void    custom_new_debug(void* ptr, size_t size);   // Debug function for operator new
void    custom_delete_debug(void* ptr);             // Debug function for operator delete




#ifdef      __cplusplus
    }
#endif

#endif // __ALLOCMEM_HPP__
