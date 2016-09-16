///////////////////////////////////////////////////////////////////////////////
// Cat.cpp :: This is the first level Animal derived class implementation
///////////////////////////////////////////////////////////////////////////////

/* Include Class Definition */
#include    "Cat.hpp"

/* Constructor */
Cat::Cat() {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    this->legs = 4;
    this->tail = 1;
    this->an = new Animal();
}

/* Destructor */
Cat::~Cat() {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    if (this->an) delete this->an;
    this->an = NULL;
}

int     Cat::testAttAccess() {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    printf("-------------------------------------------------------------------------------\n");
    printf("Legs: %d\n", this->legs);
    printf("this->tail: %d\n", this->tail);

    //! Cannot access this member this way
    //~ printf("this->an->tail: %d\n", this->an->tail);

    //! This one works
    printf("this->an->tail: %d\n", ((Cat*)this->an)->tail);

    printf("-------------------------------------------------------------------------------\n");
    return 0;
}

int     Cat::testAttAccess(Animal* animal) {
    printf("[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    printf("-------------------------------------------------------------------------------\n");
    printf("Legs: %d\n", animal->legs);

    //! Cannot access parameter this way
    //~ printf("this->tail: %d\n", animal->tail);

    //! This one works
    printf("this->tail: %d\n", ((Cat*)animal)->tail);

    printf("-------------------------------------------------------------------------------\n");
    return 0;
}
