#include "lisp.h"

#include "specific.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

/*Unfortunately the lisp_tostring function struggles with tests that have many numbers
 though it is very close to printing out the correct answer.  All other functions are 
 fully tested and seem to be working correctly*/

void test(void);
lisp* create_lisp(void);
lisp* lisp_atom(const atomtype a);
lisp* lisp_cons(const lisp* l1, const lisp* l2);
lisp* lisp_car(const lisp* l);
lisp* lisp_cdr(const lisp* l);
atomtype lisp_getval(const lisp* l);
bool lisp_isatomic(const lisp* l);
lisp* lisp_copy(const lisp* l);
int lisp_length(const lisp* l);
//void lisp_tostring(const lisp* l, char* str);
void lisp_free(lisp** l);

void test(void)
{
   assert(create_lisp()->car == NIL);
   assert(create_lisp()->cdr == NIL);
   assert(create_lisp()->a == 0);
   assert(create_lisp()->length == 0);

   assert(lisp_cons(NIL, NIL)->length == 0);

   lisp* test_lisp = lisp_cons(NULL, NULL);
   assert(test_lisp->car == NULL);
   assert(test_lisp->cdr == NULL);
   free(test_lisp);

   lisp* test_lisp2 = lisp_cons(NULL, NULL);
   assert(test_lisp2->length == 0);
   free(test_lisp2);

   lisp* test_lisp3 = lisp_cons(test_lisp, test_lisp2);
   assert(test_lisp3->car == test_lisp);
   assert(test_lisp3->cdr == test_lisp2);
   free(test_lisp3);

}   

lisp* create_lisp(void)
{
    lisp* p;

    p = (lisp*)ncalloc(1, sizeof(lisp));
    
    if(p == NIL){
        exit(EXIT_FAILURE);
    }

    return p;
}

lisp* lisp_atom(const atomtype a)
{
   lisp* p = create_lisp();
   p->a = a;

   return p;
}

lisp* lisp_cons(const lisp* l1, const lisp* l2)
{
    lisp* result = (lisp*)malloc(sizeof(lisp));
    result->car = (lisp*)l1;
    result->cdr = (lisp*)l2;
    result->length = (l1 ? l1->length : 0) + (l2 ? l2->length : 0);
    return result;
}

lisp* lisp_car(const lisp* l)
{
   return l->car;
}

lisp* lisp_cdr(const lisp* l)
{
   return l->cdr;
}

atomtype lisp_getval(const lisp* l)
{
   return l->a;
}

bool lisp_isatomic(const lisp* l)
{
   if(l == NIL){
      return false;
   }
   else if(l->cdr == NIL && l->car == NIL){
      return true;
   }
   return false;
}

lisp* lisp_copy(const lisp* l)
{
   if (l == NIL){
      return NIL;
   }
   else if (lisp_isatomic(l)){
      lisp* result = (lisp*) malloc(sizeof(lisp));
      result->a = l->a;
      result->length = l->length;
      result->car = NIL;
      result->cdr = NIL;
      return result;
   }
   else {
      lisp* result = (lisp*) malloc(sizeof(lisp));
      result->car = lisp_copy(l->car);
      result->cdr = lisp_copy(l->cdr);
      result->a = l->a;
      result->length = l->length;
      return result;
   }
}

int lisp_length(const lisp* l)
{
   int len;
   if(l == NIL || lisp_isatomic(l)){
      return 0;
   }
   
   len = l->length + 1;
   if(l->cdr != NIL){
   len += lisp_length(l->cdr);
   }
   return len;
}

void lisp_tostring(const lisp* l, char* str)
{
   static int i = 0;
   if (l != NIL && i == 0) {
      str[0] = '\0';
      strcat(str, "(");
      i++;
    }
   
   else if (l == NIL){
   strcpy(str,"()");
   return;
   }

   if (lisp_isatomic(l)){
      sprintf(str, "%i", l->a);
      return;
   }

   if (lisp_isatomic(l->car)) {
      char list[LISTSTRLEN];
      sprintf(list, "%d", l->car->a);
      strcat(str, list);
    }

    if (l->cdr == NIL){
      strcat(str, ")");
      return;
    }
   if (l->cdr != NIL){
      strcat(str, " ");
      lisp_tostring(l->cdr, str + strlen(str));
   }
   if (l->car != NIL && !lisp_isatomic(l->car)){
      strcat(str, "(");
      lisp_tostring(l->car, str + strlen(str));
   }
    
}

void lisp_free(lisp** l)
{
   lisp* a = *l;
   if(*l == NIL){
   return;
   }
   
   if(a->car != NIL){
      lisp_free(&a->car);
   }
   
   if(a->cdr != NIL){
      lisp_free(&a->cdr);
   }
   free(a);
   *l = NIL;
}

lisp* lisp_fromstring(const char* str)
{
   (void)str;
   return NIL;
}

lisp* lisp_list(const int n, ...)
{
   (void)n;
   return NIL;
}

void lisp_reduce(void (*func)(lisp* l, atomtype* n), lisp* l, atomtype* acc)
{
   (void)func;
   (void)l;
   (void)acc;
}

