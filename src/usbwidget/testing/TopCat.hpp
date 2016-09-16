///////////////////////////////////////////////////////////////////////////////
// TopCat.hpp :: This is the third level Animal derived class definition
///////////////////////////////////////////////////////////////////////////////

#ifndef     __TOPCAT_HPP__
#define     __TOPCAT_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

/* Standard Includes */
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

/* Include Class Definition for Super Class */
#include    "StrayCat.hpp"

/* Class Definition */
class   TopCat : public StrayCat {

    public:
    /* Constructors and Destructors */
    TopCat();
    virtual ~TopCat();

    protected:

    private:

};

#ifdef      __cplusplus
    }
#endif
#endif // __TOPCAT_HPP__
