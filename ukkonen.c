/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

//#define NDEBUG1
#include "debug.h"
#include "ukkonen.h"
#include <stdlib.h>

#define DEFAULT_EDGE_END instrlen

///////GENERAL UTILITY FUNCTIONS
size_t char2rank(char c, size_t UNQ_CHARS){
  return c-'a';//TODO this needs to be adapted so that everything fits in UNQ_CHARS
};

///////Printing Functions
void printUpTo(const char *instr,const size_t len){
  for(size_t i=0;i<len;i++)printf(" %c,",instr[i]);
  printf("\n");
}

void printEdge(const edge *e,const char *instr){
  printf("Edge start: %zd   end: %zd\n",e->start,e->end);
  printUpTo(instr+e->start,e->end-e->start);
  printf("\n");
}

void printPoint(const point *ap, const char *instr,size_t UNQ_CHARS){
  printf("\n/-------\nPRINTING activePoint\n");
  if(ap->node==NULL){
    printf("ap node NULL\n");
  }
  if(ap->node->edges==NULL){
    printf("ap node EDGES NULL\n");
  }
  printf("nodeDepth: %zd\n",ap->node->nodeDepth);
  printf("printing edges of ap node\n");
  for(size_t i = 0; i<UNQ_CHARS;i++){
    edge *e = ap->node->edges[i];
    if(e==NULL)continue;
    printEdge(e,instr);
  }
  printf("ap depth: %zd\n",ap->depth);
  printf("ap edge: %p\n",ap->edge);
  if(ap->edge!=NULL){
    printf("printing ap edge\n");
    printEdge(ap->edge,instr);
  }
  printf("done PRINTING activePoint\n-------/\n");
}

void printTree(const node *n,const size_t UNQ_CHARS,const char *instr){
  printf("PRINTING NODE\n");
  if(n==NULL){
    printf("NULL\n");
    printf("DONE PRINTING NODE\n");
    return;
  }
  if(n->edges==NULL){
    printf("EDGES NULL\n");
    printf("DONE PRINTING NODE\n");
    return;
  }
  printf("link: %p\n",n->link);
  printf("nodeDepth: %zd\n",n->nodeDepth);
  for(size_t i = 0; i<UNQ_CHARS;i++){
    edge *e = n->edges[i];
    if(e==NULL)continue;
    printEdge(e,instr);
    printTree(e->leaf,UNQ_CHARS,instr);
  }
  printf("DONE PRINTING NODE\n");
}

/////////VALIDATION FUNCTIONS
int isAPexceptOverflow(point *ap){
  return ap!=NULL && ap->node!=NULL && 
        (ap->edge == NULL && ap->depth >= ap->node->nodeDepth
        ||  ap->edge != NULL && ap->depth > ap->node->nodeDepth && ap->edge->start < ap->edge->end);//this because in an implicit tree, there's no dollar sign
}

int isAP(point *ap){
  return isAPexceptOverflow(ap) && //now check if ap->depth overflows currEdge
        (ap->edge == NULL ||
        ap->depth < (ap->node->nodeDepth + ap->edge->end - ap->edge->start));//if on edge, strictly less than length
}

int isEdge(const node *parent, const char *instr, const edge *e,
          const size_t UNQ_CHARS, const size_t instrlen){
  const node *n = parent;
  if(n->edges[char2rank(instr[e->start],UNQ_CHARS)]!=e)return 0;
  if(e->end <= e->start)return 0;
  if(n->nodeDepth > e->start)return 0;
  if(e->leaf==NULL){
    if(instrlen!=e->end)return 0;
    DEBUG_PRINT(("DONE CHECKING if valid edge\n"));
    return 1;
  }
  node *eleaf = e->leaf;
  if(eleaf->nodeDepth != n->nodeDepth + e->end - e->start) return 0;
  DEBUG_PRINT(("DONE CHECKING if valid edge\n"));
  return 1;
}

int isNode(const node *n,const size_t UNQ_CHARS, const char *instr,
  const size_t CURR_END, const size_t instrlen){
  //checks if this and all children are good node representation of instr
  //requires 0 < CURR_END && CURR_END <= actual instrlen
  assert(0 < CURR_END && CURR_END <= instrlen);
  if(n==NULL)return 0;
  if(n->edges==NULL)return 0;
  if(n->nodeDepth>=CURR_END)return 0;//CURR_END > 0
  //TODO CHECK IF LINK IS GOOD
  if(n->link==n)return 0;
  for(size_t i = 0; i<UNQ_CHARS;i++){
    edge *e = n->edges[i];
    if(e==NULL)continue;
    if(!isEdge(n,instr,e,UNQ_CHARS,instrlen))return 0;
    if(e->leaf==NULL)continue;
    if(!isNode(e->leaf,UNQ_CHARS,instr,CURR_END,instrlen)) return 0;
  }
  DEBUG_PRINT(("DONE CHECKING if valid node\n"));
  return 1;
}

int isImplicitSuffixTree(const node *root,const size_t UNQ_CHARS, 
  const char *instr, const size_t CURR_END, const size_t instrlen){
  //checks if root is an implicit sTree up to CURR_END
  //TODO DOES NOT COUNT LEAF EDGES
  //instrlen excludes terminating char
  //requires 0 < CURR_END && CURR_END <= instrlen
  assert(0 < CURR_END && CURR_END <= instrlen);
  DEBUG_PRINT(("CHECKING structure of tree\n"));
  if(!isNode(root,UNQ_CHARS,instr,CURR_END,instrlen))return 0;
  DEBUG_PRINT(("CHECKING if each suffix is in tree\n"));
  for(size_t suffix = 0; suffix<CURR_END; suffix++){
    DEBUG_PRINT(("CHECKING if suffix %zd is in tree\n",suffix));
    size_t currCharIndex = suffix;
    const node *currNode = root;
    assert(currNode->nodeDepth == currCharIndex-suffix);//internal loop invariant
    while(currCharIndex<CURR_END){
      char currChar = instr[currCharIndex];
      //currChar should be a branch of currNode
      edge *e = currNode->edges[char2rank(currChar,UNQ_CHARS)];
      if(e==NULL) return 0;
      //compare each char on branch with currCharIndex onwards
      size_t eLength = e->end - e->start;//does not matter if e->leaf==NULL
      size_t eTraversed = 0;
      while(currCharIndex<CURR_END && eTraversed<eLength){
        if(instr[e->start+eTraversed]!=instr[currCharIndex])return 0;
        eTraversed++;
        currCharIndex++;
      }
      if(currCharIndex==CURR_END)break;
      
      //then update currNode
      currNode = e->leaf;
      if(currNode==NULL) return 0;
      assert(currCharIndex-suffix==currNode->nodeDepth);//internal loop invariant
    }
  }
  return 1;
}

///////Actual Implementation
node *ukkonen_implicit(const char *instr, const size_t instrlen, const size_t UNQ_CHARS){
  //instrlen excludes terminating char
  //returns the implicit suffix tree for instr
  node *root = calloc(1,sizeof(node));
  root->edges = calloc(UNQ_CHARS,sizeof(edge *));

  point *ap=malloc(sizeof(point));
  ap->node = root;
  ap->depth=0;
  ap->edge=NULL;//will always be NULL if depth == nodeDepth
  
  size_t currSuffix = 0;
  
  node *prevNode = NULL;
  
#define DEFAULT_EDGE_END instrlen
  
  for(size_t END = 1; END <= instrlen; END++){
    currSuffix_loop: ;//loops until currSuffix==END or currChar matches
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
          prevNode = (ap->node == root? NULL: ap->node);
        }
        ////////DONE INSERTING
        currSuffix++;
        ap->node = ap->node->link;
        if(ap->node==NULL)ap->node=root;
        ap->depth > 0? ap->depth--: 0;//this has a problem is ap->depth was 0
        //calculate where this depth is
        if(ap->depth==ap->node->nodeDepth){
          //ap will be at the node, so we don't have to do anything
        }else{
          assert(ap->depth > ap->node->nodeDepth);//ap must go deeper than the node

          hop_loop: ;//terminates when ap is at the deepest node b4 traversing edge
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
        assert(prevNode==NULL || prevNode==root);
        prevNode = NULL;
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
  //assert root is now an implicit suffix tree of instr
  free(ap);
  //do we have any cleaning up to do?
  return root;
}

size_t substrings_ukkonen_implicit(const char *instr, const size_t instrlen, const size_t UNQ_CHARS){
  //instrlen excludes terminating char
  //returns the number of unique substrings in instr, through counting 
  //  the lengths of the edges on the implicit suffix tree built with Ukkonen's
  node *root = calloc(1,sizeof(node));
  root->edges = calloc(UNQ_CHARS,sizeof(edge *));

  point *ap=malloc(sizeof(point));
  ap->node = root;
  ap->depth=0;
  ap->edge=NULL;//will always be NULL if depth == nodeDepth
  
  size_t currSuffix = 0;
  
  node *prevNode = NULL;

  size_t substrings = 0;
  
  for(size_t END = 1; END <= instrlen; END++){
    DEBUG_PRINT(("\n============\nstarting at END = %zd\n",END));
    currSuffix_loop: ;//loops until currSuffix==END or currChar matches
    assert(currSuffix + ap->depth == END-1);
    char currChar = instr[END-1];
    DEBUG_PRINT(("curr char: %c\n",currChar));
    size_t currCharRank = char2rank(currChar,UNQ_CHARS);
    //first decide if ap is at a node or on a branch, then check if currChar matches
    DEBUG_PRINT(("STARTING currSuffix loop with suffix %zd, and ap\n",currSuffix));
    DEBUG_FUNC_CALL(printPoint(ap,instr,UNQ_CHARS));
    assert(isAP(ap));
    if(ap->edge==NULL?
      (ap->node->edges)[currCharRank]==NULL :
      instr[ap->edge->start + (ap->depth - ap->node->nodeDepth)]!=currChar ){
        //char does not match, so we branch
        assert( currSuffix < END );
        ////////INSERTING
        DEBUG_PRINT(("inserting new edge at active Point\n"));
        edge *newEdge = malloc(sizeof(edge));
        newEdge->start = END-1;//index of the currChar
        newEdge->end = instrlen;//default end
        substrings += (instrlen - END + 1 );//end - start
        newEdge->leaf = NULL;
        
        //is ap on a branch or at a node?
        if(ap->edge==NULL){
          DEBUG_PRINT(("ap is at a node so only add newEdge\n"));
          //ap is at a node, so we add new branch, and set link just in case
          ap->node->edges[currCharRank]=newEdge;
          if(prevNode!=NULL){
            assert(prevNode!=root);
            assert(prevNode->nodeDepth -1 == ap->node->nodeDepth);
            prevNode->link=ap->node;
          }
          prevNode = ap->node;
          assert(isEdge(ap->node,instr,newEdge,UNQ_CHARS,instrlen));
        }else{
          DEBUG_PRINT(("ap is on a branch, so add newNode too\n"));
          //ap is on a branch, so we add newEdge and branchedFrom
          node *newNode = malloc(sizeof(node));
          newNode->edges = calloc(UNQ_CHARS,sizeof(edge *));
          newNode->edges[currCharRank]=newEdge;
          newNode->nodeDepth = ap->depth;
          newNode->link = NULL;
          newNode->mark = 0;
          
          edge *branchedFrom = malloc(sizeof(edge));
          size_t edgeDepth = ap->depth - ap->node->nodeDepth;
          branchedFrom->start = ap->edge->start + edgeDepth;
          branchedFrom->end = ap->edge->end;
          ap->edge->end = branchedFrom->start;
          branchedFrom->leaf = ap->edge->leaf;
          ap->edge->leaf = newNode;
          char branchedFromChar = instr[branchedFrom->start];
          newNode->edges[char2rank(branchedFromChar,UNQ_CHARS)]=branchedFrom;
          
          //TODO MUST INCREASE NODE DEPTH OF ALL newNode
          
          if(prevNode!=NULL){
            assert(prevNode!=root);
            assert(prevNode->nodeDepth -1 == newNode->nodeDepth);
            //TODO make better prevNode check
            prevNode->link=newNode;
          }
          prevNode = newNode;
          DEBUG_FUNC_CALL(printf("/********\nPrinting newNode\n"));
          DEBUG_FUNC_CALL(printTree(newNode,UNQ_CHARS,instr));
          DEBUG_FUNC_CALL(printf("\nDONE Printing newNode\n********/\n"));
          assert(isNode(newNode,UNQ_CHARS,instr,END,instrlen));
        }
        DEBUG_PRINT(("inserted new edge\n"));
        
        ////////DONE INSERTING
        currSuffix++;
        ap->node = ap->node->link;
        if(ap->node==NULL)ap->node=root;
        if(ap->depth > 0) ap->depth--;//this has a problem if ap->depth was 0
        if(ap->depth == ap->node->nodeDepth) ap->edge=NULL;
        DEBUG_FUNC_CALL(printPoint(ap,instr,UNQ_CHARS));
        assert(isAPexceptOverflow(ap));
        
        //calculate where this depth is
        DEBUG_PRINT(("Calculating where this activePoint depth is\n"));
        DEBUG_FUNC_CALL(printPoint(ap,instr,UNQ_CHARS));
        if(ap->depth>ap->node->nodeDepth){
          hop_loop: ;//terminates when ap is at the deepest node b4 traversing edge
          //calculate which branch to go
          char nextchar = instr[ap->node->nodeDepth+currSuffix];
          size_t depthToGo = ap->depth - ap->node->nodeDepth;
          if(depthToGo==0) goto end_node_hopping;
          ap->edge = ap->node->edges[char2rank(nextchar,UNQ_CHARS)];
          assert(ap->edge!=NULL);//this because depthToGo > 0
          //do we need to hop over this guy?
          assert(ap->edge->leaf!=NULL || depthToGo < END - ap->edge->start);
          if(depthToGo >= ap->edge->end - ap->edge->start){
              ap->node = ap->edge->leaf;
              ap->edge = NULL;
              goto hop_loop;
          }
          end_node_hopping: ;
          //we have arrived at the deepest node by node hopping
          assert(depthToGo + ap->node->nodeDepth == ap->depth);
        }
        DEBUG_FUNC_CALL(printPoint(ap,instr,UNQ_CHARS));
        if(ap->node==root){
          prevNode=NULL;
          DEBUG_PRINT(("RESET prevNode because ap is now at root\n"));
        }
        assert(isAP(ap));
        DEBUG_PRINT(("DONE Calculating where this activePoint depth is\n"));
        //activePoint is now positioned where it needs to be
        //  so that currChar is the next to be considered
        if(currSuffix<END) goto currSuffix_loop;
        assert( currSuffix==END && ap->node == root && isAP(ap) && prevNode == NULL);
    }else{
      prevNode = NULL;
      DEBUG_PRINT(("reset prevNode because no longer matching suffix\n"));
      //advance activePoint along matching char
      DEBUG_PRINT(("////////advancing aP along matching char\n"));
      assert(isAP(ap));
      
      ap->depth++;
      if(ap->edge==NULL)ap->edge = (ap->node->edges)[currCharRank];//follow the matching branch

      //follow the current branch
      size_t edgeDepth =  ap->depth - ap->node->nodeDepth;
      assert(edgeDepth>0);
      if(ap->edge->leaf!=NULL){ 
        if (edgeDepth == ap->edge->end - ap->edge->start){
          //TODO is this right?
          ap->node = ap->edge->leaf;
          ap->edge = NULL;
        }
      }else assert(edgeDepth < END - ap->edge->start);//its a leaf edge
      DEBUG_FUNC_CALL(printPoint(ap,instr,UNQ_CHARS));
      assert(isAP(ap));
      DEBUG_PRINT(("////////DONE advancing aP along matching char\n"));
      
    }
    DEBUG_FUNC_CALL(printf("\n/**********\nprinting Tree\n"));
    DEBUG_FUNC_CALL(printTree(root,UNQ_CHARS,instr));
    DEBUG_FUNC_CALL(printf("\nDONE printing Tree\n**********/\n"));
    assert(isImplicitSuffixTree(root,UNQ_CHARS,instr,END,instrlen));
  }
  assert(isImplicitSuffixTree(root,UNQ_CHARS,instr,instrlen,instrlen));
  free(ap);
  //free the tree
  //do we have any cleaning up to do?
  return substrings;
}