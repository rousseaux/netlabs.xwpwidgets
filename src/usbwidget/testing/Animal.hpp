///////////////////////////////////////////////////////////////////////////////
// Animal.hpp :: This is the top-level class definition
///////////////////////////////////////////////////////////////////////////////

#ifndef     __ANIMAL_HPP__
#define     __ANIMAL_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

/* Standard Includes */
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

/* Class Definition */
class   Animal {

    public:
    /* Constructors and Destructors */
    Animal();
    virtual ~Animal();

    /* Public Methods */
    virtual int     testAttAccess();
    virtual int     testAttAccess(Animal* animal);

    /* Public Attributes */
    int     legs;

    protected:
    /* Protected Attributes */
    int     tail;

    private:

};

#ifdef      __cplusplus
    }
#endif
#endif // __ANIMAL_HPP__
