#include "stack.h"
int stack_empty(llstack *s){
  return s->top==NULL;
}

llstack *newstack(){
  llstack *s = (llstack *) malloc(sizeof(llstack));
  //s->nobj = 0;
  s->top = NULL;
  return s;
}

void push(llstack *s,elem e){
  //requires s!=NULL
  llnode *n = (llnode *) malloc(sizeof(llnode));
  n->data = e;
  n->next=s->top;
  s->top=n;
  //s->nobj++;
}

elem peek(llstack *s){
  //requires s is not empty
  return s->top->data;
}

elem pop(llstack *s){
  //requires s is not empty
  llnode *t=s->top;
  s->top=t->next;
  //s->nobj--;

  elem e = t->data;
  free(t);
  return e;
}

void free_stack(llstack *s,stack_elem_free_fn *efree){
  while(!stack_empty(s)){
    elem e =pop(s);
    if(efree!=NULL)efree(e);
  }
  free(s);
}

void stack_print(llstack *s, stack_e_print_fn *p){
  if(stack_empty(s)) {printf("stack empty\n"); return;}
  llnode *curr = s->top;
  while(curr!=NULL){
    p(curr->data);
    curr=curr->next;
  }
}