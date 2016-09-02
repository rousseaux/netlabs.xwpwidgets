/*****************************************************************************\
* AllocMem.cpp -- NeoWPS * USB Widget                                         *
*                                                                             *
* Copyright (c) 2014 RDP Engineering                                          *
*                                                                             *
* Author: Ben Rietbroek <rousseau.ecsdev@gmail.com>                           *
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
 * This module implements the overrides for the C malloc and free functions
 * as well as the C++ new and delete overloads.
 * This is needed to allocate from named shared memory using the standard
 * C/C++ methods.
 */

#include    "AllocMem.hpp"

/*
// This is the all important pointer to the dynamically allocated memory pool
// to hold the Widget State and service other dynamic memory allocations.
// It is either obtained from the DAEMON, allocated using DosAllocMem or
// from the C runtime using malloc.
*/
void*       masterpool = NULL;



/*
// This allocates the memory pool in case the named shared memory could not
// be found. It allocates SHMEM_SIZE either directly from OS/2 using
// DosAllocMem() or from the C runtime using native malloc().
// The pooltype flag indicates which to use.
*/
void*       allocate_master_pool(POOLTYPE pooltype) {
    APIRET  ulrc            = -1;
    void*   ptr             = NULL;
    char    buf[256]        = "\0";
    PMASTERMEMPOOL  pmmp    = NULL;

    switch (pooltype) {
        case    PT_USE_OS2:
            ulrc = DosAllocMem(&ptr, SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL), PAG_COMMIT | PAG_READ | PAG_WRITE);
            sprintf(buf,"DosAllocMem: ptr: %08lX, ulrc:%d", ptr, ulrc);
            __debug("allocate_master_pool(PT_USE_OS2)", buf, DBG_POPUP);
            break;
        case    PT_USE_CRT:
            ptr = crt_malloc(SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL));
            sprintf(buf,"crt_malloc: ptr: %08lX", ptr);
            __debug("allocate_master_pool(PT_USE_CRT)", buf, DBG_POPUP);
            break;
        default:
            break;
    }

    /*
    // Allocation of the master pool succeeded, so we initialize it
    // for sub allocation.
    */
    if (ptr) {
        pmmp = (PMASTERMEMPOOL) ptr;            // Cast pointer to access master structure
        pmmp->signature     = SHMEM_SIGNATURE;  // Set signature
        pmmp->poolsize      = SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL);
        pmmp->pooltype      = pooltype;         // Set the pooltype
        pmmp->submempool    = &pmmp[1];         // Sub Memory Pool starts here
        pmmp->pwidgetstate  = NULL;             // Pointer to the Widget State

        ulrc = DosSubSetMem(pmmp->submempool, DOSSUB_INIT, SHMEM_SIZE+sizeof(SUBMEMPOOL));
        sprintf(buf,"DosSubSetMem: pooltype:%d, ulrc:%d", pooltype, ulrc);
        __debug("allocate_master_pool", buf, DBG_POPUP);


        /*
        // If the creation of the sub-allocation system failed,
        //  we free the pool.
        */
        if (ulrc) {
            ulrc = DosSubUnsetMem(pmmp->submempool);
            switch (pooltype) {
                case    PT_USE_OS2:
                    ulrc = DosFreeMem(ptr);
                    break;
                case    PT_USE_CRT:
                    crt_free(ptr);
                    break;
            }
            ptr = NULL;
        }
    }

    sprintf(buf,"ptr:%08lX", ptr);
    __debug("allocate_master_pool", buf, DBG_POPUP);

    return ptr;
}

void        free_master_pool(void* pool) {
    APIRET  ulrc            = -1;
    void*   ptr             = NULL;
    PMASTERMEMPOOL  pmmp    = NULL;

    if (pool) {
        pmmp = (PMASTERMEMPOOL) pool;
        ulrc = DosSubUnsetMem(pmmp->submempool);
        switch (pmmp->pooltype) {
            case    PT_USE_OS2:
                ulrc = DosFreeMem(pool);
                break;
            case    PT_USE_CRT:
                crt_free(pool);
                break;
        }
    }

    return;
}



/*
// Encapsulate the standard C runtime malloc function so we don't have
// to #undef/#define the custom malloc every time we need the standard one.
*/
void*   crt_malloc(size_t size) {
#undef      malloc
    return malloc(size);
#define     malloc  custom_malloc
}

/*
// Encapsulate the standard C suntime free function so we don't have
// to #undef/#define the custom free every time we need the standard one.
*/
void    crt_free(void* ptr) {
#undef      free
    free(ptr);
#define     free   custom_free
}



/*
// Override the standard C malloc function.
*/
void*   custom_malloc(size_t size) {
    APIRET          ulrc        = -1;
    PMASTERMEMPOOL  pmmp        = NULL;
    PSUBMEMPOOL     psmp        = NULL;
    void*           ptr         = NULL;
    char            buf[256]    = "\0";

    if (masterpool) {
        pmmp = (PMASTERMEMPOOL) masterpool;
        psmp = (PSUBMEMPOOL) pmmp->submempool;
        ulrc = DosSubAllocMemEx(psmp, &ptr, size);
        sprintf(buf, "custom_malloc/DosSubAllocMemEx: ptr: %08lX, ulrc: %d", ptr, ulrc);
        //~ MessageBox("custom_malloc/DosSubAllocMemEx", buf);
        if (ulrc) {
            ptr = NULL;
        }
    }
    else {
        ptr = NULL;
    }

    custom_malloc_debug(ptr, size);

    return ptr;
}


/*
// Override the standard C free function.
*/
void    custom_free(void* ptr) {
    APIRET          ulrc    = -1;
    PMASTERMEMPOOL  pmmp    = NULL;
    PSUBMEMPOOL     psmp    = NULL;

    if (masterpool) {
        pmmp = (PMASTERMEMPOOL) masterpool;
        psmp = (PSUBMEMPOOL) pmmp->submempool;
        ulrc = DosSubFreeMemEx(ptr);
    }

    custom_free_debug(ptr);

    return;
}



/*
// Overload the C++ new operator.
*/
void*   operator new(size_t size) {
    void*   ptr = NULL;
    ptr = malloc(size);
    custom_new_debug(ptr, size);
    return ptr;
}

/*
// Overload the C++ delete operator.
*/
void    operator delete(void* ptr) {
    free(ptr);
    custom_delete_debug(ptr);
}

//// DEBUG STUFF ////

/*
// Debug function for custom_malloc.
*/
void    custom_malloc_debug(void* ptr, size_t size) {
    char    buf[256] = "\0";
    sprintf(buf, "custom_malloc: size: %d, ptr: %08lX", size, ptr);
    //~ __debug(NULL, buf, DBG_LBOX);

    return;
}

/*
// Debug function for custom_free.
*/
void    custom_free_debug(void* ptr) {
    char    buf[256] = "\0";
    sprintf(buf, "custom_free: ptr: %08lX", ptr);
    //~ __debug(NULL, buf, DBG_LBOX);

    return;
}

/*
// Debug function for custom new operator.
*/
void    custom_new_debug(void* ptr, size_t size) {
    char    buf[256] = "\0";
    sprintf(buf, "custom operator new: size: %d, ptr: %08lX", size, ptr);
    //~ __debug(NULL, buf, DBG_LBOX);

    return;
}

/*
// Debug function for custom free operator.
*/
void    custom_delete_debug(void* ptr) {
    char    buf[256] = "\0";
    sprintf(buf, "custom operator delete: ptr: %08lX", ptr);
    //~ __debug(NULL, buf, DBG_LBOX);

    return;
}
