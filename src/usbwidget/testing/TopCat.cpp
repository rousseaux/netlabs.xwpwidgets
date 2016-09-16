///////////////////////////////////////////////////////////////////////////////
// TopCat.cpp :: This is the third level Animal derived class implementation
///////////////////////////////////////////////////////////////////////////////

/* Include Class Definition */
#include    "TopCat.hpp"

/* Constructor */
TopCat::TopCat() {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    this->legs = 4;
}

/* Destructor */
TopCat::~TopCat() {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
}
