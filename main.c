/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Xinyu
 *
 * Created on January 4, 2018, 1:18 PM
 */

#include <stdlib.h>
//#define NDEBUG
#include "debug.h"
#include "ukkonen.h"

/*
 * 
 */
int main(int argc, char** argv) {
  char *test="qqqqqqqqqqzrzrrzrzrrzrrzrzrrzrzrrzttttttttttttttttttttttttttttttttttttttttttttttttttttttqncpqzcxpbwa";
  //char *test="aabcwervdszxcupasfaaaaabaaaavavafqwfre";
  size_t testlen = 100;
  //size_t start = 0;
  //size_t end = 9;
  //printf("%zd\n",substrings_ukkonen_implicit(test+start,end-start,26));
  //size_t start = 0;
  //size_t end = 39;
  //substrings_ukkonen_implicit(test+start,end-start,26);
  /*
  for(size_t end=start+1;end<testlen;end++){
    substrings_ukkonen_implicit(test+start,end-start,26);
    printf("end = %zd\n",end);
  }
  /**/
  /**/
  for(size_t start = 0; start <testlen;start++){
    for(size_t end=start+1;end<testlen;end++)substrings_ukkonen_implicit(test+start,end-start,26);
    printf("start = %zd\n",start);
  }
  /**/
  return (EXIT_SUCCESS);
}

