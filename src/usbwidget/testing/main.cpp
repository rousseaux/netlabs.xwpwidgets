///////////////////////////////////////////////////////////////////////////////
// main.cpp :: Simple program to test classes
// ----------------------------------------------------------------------------
// C++ compilers are complex beasts.
// This program is used to test some IBM VisualAge v3 compiler behavior.
///////////////////////////////////////////////////////////////////////////////

/* Standard Includes */
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

/* Include Class Definition(s) */
#include    "Animal.hpp"
#include    "Cat.hpp"
#include    "StrayCat.hpp"
#include    "TopCat.hpp"

/* Include Module Header */
#include    "main.hpp"

/* Main EntryPoint */
int     main(int argc, char* argv[]) {
    printf("\n");
    printf("Hello from a C++ source !\n");
    printf("\n");

    /* Do some testing on classes */
    do {
        Animal* a = new StrayCat();
        a->testAttAccess();     // Test protected attribute access using pointer
        a->testAttAccess(a);    // Test protected attribute access using parameter
        if (a) delete a;
        a = NULL;
    } while (0);

    return 0;
}
