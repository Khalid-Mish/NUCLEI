#define LISPIMPL "CarCdr"
#define NIL NULL
#include "lisp.h"

#define LISTSTRLEN 1000


struct lisp {
lisp* car;
lisp* cdr;
atomtype a;
int length;
};
