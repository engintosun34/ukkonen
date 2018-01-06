/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ukkonen.h
 * Author: Xinyu
 *
 * Created on January 4, 2018, 1:20 PM
 */

#ifndef UKKONEN_H
#define UKKONEN_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include <stdlib.h>
  //const size_t NUM_CHARS;
  
  typedef struct edge_header edge;
  typedef struct node_header node;
  typedef struct point_header point;
  
  struct edge_header{
    node *leaf;//NULL if does not branch at end
    size_t start;//inclusive
    size_t end;//exclusive
                //starts out as the actual instr end, including terminating char
  };
  
  struct node_header{
    edge **edges;//static array of NUM_CHARS children; NULL if does not have edge at a char
    node *link; //points to node that has same address, except for the first char
                //defaults to NULL, which means go to root after branching from here
    size_t nodeDepth;
    size_t numsubsbelow;
  };
  
  struct point_header{
    node *node;//all chars up to this branch point match currSuffix
    edge *edge;
    size_t depth;//represents total matching chars
      //depth - node->nodeDepth will be the edge depth
  };
  
  size_t ukkonen(char *instr, size_t instrlen, size_t UNQ_CHARS);//instrlen excludes terminating char
  size_t substrings_ukkonen_implicit(const char *instr, const size_t instrlen, const size_t UNQ_CHARS);
  /*
High-level description of Ukkonen's algorithm:
We are basically inserting suffixes into the tree from longest to shortest.
Tree representation:
  Nodes (represent places where branch off), each containing:
    collection of edges
    link points to node that contains same path from root, except first char, defaults to NULL
    number of substrings in sub and below
  Edges (represent branches), each containing:
    parent node?
    child node
    start and end indeces
set activePoint to root, depth 0
set root->link to NULL
set currSuffix to suffix 0
set prevNode to NULL
Set END to 1
Iterate END across the instr//don't include terminating char
  (assert currSuffix < END)
  currSuffix_loop://loops until currSuffix==END or currChar matches
  if currSuffix has next char that does not match activePoint's children:
    (meaning that curr char, which is always one b4 END, does not match)
    insert currSuffix at new branch at activePoint 
      if prevNode is not NULL, then set prevNode->link = new node at activePoint
      set prevNode = activePoint->node
    advance currSuffix 
    follow activePoint->node->link to find next activePoint
      if NULL, then set activePoint to root
      otherwise set activePoint->node = activePoint->node->link
      go to a depth-1, then calculate where that is on active edge 
        may entail jumps
        if currSuffix<END: goto currSuffix_loop
    set prevNode to NULL (because it is root)
    (assert currSuffix==END && activePoint is root && prevNode is NULL)
  else: advance activePoint along sub that matches char
    this will always increase depth
  //assert currSuffix == END || currChar matches
//assert currSuffix == END == actual instr end (without terminating char)
 */
  
#ifdef __cplusplus
}
#endif

#endif /* UKKONEN_H */

