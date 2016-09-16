///////////////////////////////////////////////////////////////////////////////
// Animal.cpp :: This is the top-level class implementation
///////////////////////////////////////////////////////////////////////////////

/* Include Class Definition */
#include    "Animal.hpp"

/* Constructor */
Animal::Animal() {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    this->legs = 0;
    this->tail = 0;
}

/* Destructor */
Animal::~Animal() {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
}

int     Animal::testAttAccess() {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    return 0;
}

int     Animal::testAttAccess(Animal* animal) {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    return 0;
}

