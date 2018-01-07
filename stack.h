/*
Stack implementation
*/

#ifndef _LLSTACK_H_
#define _LLSTACK_H_

#include <stdlib.h>
#include <stdio.h>

typedef void *elem;
typedef struct stack_header llstack;
typedef struct llnode_header llnode;

typedef void stack_elem_free_fn(elem e);
typedef void stack_e_print_fn(elem e);

struct llnode_header{
  elem data;
  llnode *next;
};

struct stack_header{
  //size_t nobj;
  llnode *top;
};

int stack_empty(llstack *s);
llstack *newstack();
void push(llstack *s,elem e);
elem peek(llstack *s);
elem pop(llstack *s);
void free_stack(llstack *s,stack_elem_free_fn *efree);
void stack_print(llstack *s, stack_e_print_fn *p);

#endif