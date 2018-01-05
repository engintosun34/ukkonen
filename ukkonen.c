/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "ukkonen.h"
#include <assert.h>
//#define NDEBUG //if included, removes assert statements
#include <stdlib.h>

size_t char2index(char c, size_t UNQ_CHARS){
  return c;//TODO this needs to be adapted so that everything fits in UNQ_CHARS
};

inline void insertCurrSuffix(point *ap, size_t currSuffix){
          //insert currSuffix at new branch at activePoint 
        //  if prevNode is not NULL, then set prevNode->link = new node at activePoint
        //  set prevNode = currNode
}

size_t ukkonen(char *instr, size_t instrlen, size_t UNQ_CHARS){//instrlen excludes terminating char
  node *root = calloc(sizeof(node));
  root->edges = calloc(UNQ_CHARS,sizeof(edge *));

  point *ap=malloc(sizeof(point));
  ap->node = root;
  ap->depth=0;
  ap->edge=NULL;//will always be NULL if depth == nodeDepth
  
  size_t currSuffix = 0;
  
  node *prevNewNode = NULL;
  
  for(size_t END = 1; END <= instrlen; END++){
    currSuffix_loop://loops until currSuffix==END or currChar matches
    assert(currSuffix + ap->depth + 1 == END);
    char currChar = instr[END-1];
    //first decide if ap is at a node or on a branch, then check if currChar matches
    if(ap->edge==NULL?
      (ap->node->edges)[char2index(currChar,UNQ_CHARS)]==NULL :
      instr[ap->edge->start + (ap->depth - ap->node->nodeDepth)]!=currChar ){
      //char does not match, so we branch
      assert( currSuffix < END );
      //insertCurrSuffix(ap,currSuffix);
      currSuffix++;
      ap->node = ap->node->link;
      if(ap->node==NULL)ap->node=root;
      ap->depth--;
      //calculate where this depth is
      if(ap->depth==ap->node->nodeDepth){
        //ap will be at the node, so we don't have to do anything
      }else{
        assert(ap->depth > ap->node->nodeDepth);//ap must go deeper than the node
        
        hop_loop://terminates when ap is at the deepest node b4 traversing edge
        //calculate which branch to go
        char nextchar = instr[ap->node->nodeDepth+currSuffix];
        ap->edge = ap->node->edges[char2index(nextchar,UNQ_CHARS)];
        size_t depthToGo = ap->depth - ap->node->nodeDepth;
        assert(ap->edge!=NULL);//this because depthToGo > 0
        //do we need to hop over this guy?
        if(ap->edge->leaf==NULL?
          depthToGo > END - ap->edge->start:
          depthToGo > ap->edge->end - ap->edge->start){
          ap->node = ap->edge->leaf;
          ap->edge = NULL;
          goto hop_loop;
        }
        //we have arrived at the deepest node by node hopping
        assert(depthToGo + ap->node->nodeDepth == ap->depth);
      }
      //activePoint is now positioned where it needs to be
      //  so that currChar is the next to be considered
      if(currSuffix<END) goto currSuffix_loop;
      prevNewNode = NULL;//because it is root
      assert( currSuffix==END && ap->node == root && prevNode == NULL);
    }else{
      //advance activePoint along matching char
    }
  }
  assert( currSuffix == END && END == instrlen + 1);
  //we now need to add the dollar sign
  //then sum up the substrings, and subtract one to exclude the empty string
  
}