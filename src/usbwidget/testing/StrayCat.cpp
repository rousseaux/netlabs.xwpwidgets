///////////////////////////////////////////////////////////////////////////////
// StrayCat.cpp :: This is the second level Animal derived class implementation
///////////////////////////////////////////////////////////////////////////////

/* Include Class Definition */
#include    "StrayCat.hpp"

/* Constructor */
StrayCat::StrayCat() {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    this->legs = 4;
}

/* Destructor */
StrayCat::~StrayCat() {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
}
