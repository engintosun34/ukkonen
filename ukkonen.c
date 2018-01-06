/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "ukkonen.h"
#include <assert.h>
//#define NDEBUG //if included, removes assert statements
#include <stdlib.h>

size_t char2rank(char c, size_t UNQ_CHARS){
  return c;//TODO this needs to be adapted so that everything fits in UNQ_CHARS
};

size_t ukkonen_implicit(const char *instr, const size_t instrlen, const size_t UNQ_CHARS){
  //instrlen excludes terminating char
  //returns the implicit suffix tree for instr
  node *root = calloc(sizeof(node));
  root->edges = calloc(UNQ_CHARS,sizeof(edge *));

  point *ap=malloc(sizeof(point));
  ap->node = root;
  ap->depth=0;
  ap->edge=NULL;//will always be NULL if depth == nodeDepth
  
  size_t currSuffix = 0;
  
  node *prevNode = NULL;
  
#define DEFAULT_EDGE_END instrlen
  
  for(size_t END = 1; END <= instrlen; END++){
    currSuffix_loop://loops until currSuffix==END or currChar matches
    assert(currSuffix + ap->depth == END-1);
    char currChar = instr[END-1];
    size_t currCharRank = char2rank(currChar,UNQ_CHARS);
    //first decide if ap is at a node or on a branch, then check if currChar matches
    if(ap->edge==NULL?
      (ap->node->edges)[currCharRank]==NULL :
      instr[ap->edge->start + (ap->depth - ap->node->nodeDepth)]!=currChar ){
        //char does not match, so we branch
        assert( currSuffix < END );
        ////////INSERTING
        edge *newEdge = malloc(sizeof(edge));
        newEdge->start = END-1;//index of the currChar
        newEdge->end = DEFAULT_EDGE_END;//default end
        newEdge->leaf = NULL;
        if(ap->edge==NULL){
          //ap is at a node, we only add the new branch
          ap->node->edges[currCharRank]=newEdge;
        }else{
          //ap is on a branch, so we add newEdge and branchedFrom
          node *newNode = malloc(sizeof(node));
          newNode->edges = calloc(UNQ_CHARS,sizeof(edge *));
          newNode->edges[currCharRank]=newEdge;
          newNode->nodeDepth = ap->depth;
          
          edge *branchedFrom = malloc(sizeof(edge));
          size_t edgeDepth = ap->depth - ap->node->nodeDepth;
          branchedFrom->start = ap->edge->start + edgeDepth;
          ap->edge->end = branchedFrom->start;
          branchedFrom->end = DEFAULT_EDGE_END;//default end
          branchedFrom->leaf = ap->edge->leaf;
          ap->edge->leaf = newNode;
          char branchedFromChar = instr[branchedFrom->start];
          newNode->edges[char2rank(branchedFromChar,UNQ_CHARS)]=branchedFrom;
          
          if(prevNode!=NULL){
            assert(prevNode!=root);
            prevNode->link=newNode;
          }
          prevNode = ap->node;
        }
        ////////DONE INSERTING
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
          ap->edge = ap->node->edges[char2rank(nextchar,UNQ_CHARS)];
          size_t depthToGo = ap->depth - ap->node->nodeDepth;
          assert(ap->edge!=NULL);//this because depthToGo > 0
          //do we need to hop over this guy?
          assert(ap->edge->leaf!=NULL || depthToGo < END - ap->edge->start);
          if(depthToGo > ap->edge->end - ap->edge->start){
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
        assert(prevNode==root);
        prevNode = NULL;//because it is root
        assert( currSuffix==END && ap->node == root && prevNode == NULL);
    }else{
      //advance activePoint along matching char
      ap->depth++;
      if(ap->edge==NULL){
        //follow the matching branch
        ap->edge = (ap->node->edges)[currCharRank];
      }else{
        //follow the current branch
        size_t edgeDepth =  ap->depth - ap->node->nodeDepth;
        if(ap->edge->leaf!=NULL){ 
          if (edgeDepth == ap->edge->end - ap->edge->start){
            //TODO is this right?
            ap->node = ap->edge->leaf;
            ap->edge = NULL;
          }
        }else assert(edgeDepth < END - ap->edge->start);//its a leaf edge
      }
    }
    //assert root is now an implicit suffix tree up to currChar
  }
  assert( currSuffix == END && END == instrlen + 1);
  free(ap);
  //do we have any cleaning up to do?
  return root;
}