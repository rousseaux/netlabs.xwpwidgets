///////////////////////////////////////////////////////////////////////////////
// StrayCat.hpp :: This is the second level Animal derived class definition
///////////////////////////////////////////////////////////////////////////////

#ifndef     __STRAYCAT_HPP__
#define     __STRAYCAT_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

/* Standard Includes */
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

/* Include Class Definition for Super Class */
#include    "Cat.hpp"

/* Class Definition */
class   StrayCat : public Cat {

    public:
    /* Constructors and Destructors */
    StrayCat();
    virtual ~StrayCat();

    protected:

    public:

};

#ifdef      __cplusplus
    }
#endif
#endif // __STRAYCAT_HPP__
