///////////////////////////////////////////////////////////////////////////////
// Cat.hpp :: This is the first level Animal derived class definition
///////////////////////////////////////////////////////////////////////////////

#ifndef     __CAT_HPP__
#define     __CAT_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

/* Standard Includes */
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

/* Include Class Definition for Super Class */
#include    "Animal.hpp"

/* Class Definition */
class   Cat : public Animal {

    public:
    /* Constructors and Destructors */
    Cat();
    virtual ~Cat();
    virtual int testAttAccess();
    virtual int testAttAccess(Animal* animal);

    protected:
    Animal* an;

    private:

};

#ifdef      __cplusplus
    }
#endif
#endif // __CAT_HPP__
