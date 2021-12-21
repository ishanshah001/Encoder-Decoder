#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HeapDT.h"
#include "packman_utils.h"


//-------------------------------------------------------Heap helper function--------------------------------------------------------------------

/// less-than-or-equal compares entries as pointers to int values.
///
/// @param item1 pointer to the left hand side of the comparison
/// @param item2 pointer to the right hand side of the comparison
/// @return non-zero if *item1->freq < *item2->freq
/// @pre values are pointers to integer values.
static int cmpintmin( const void * item1, const void * item2 ) {
    struct Tree_node_s* one = (struct Tree_node_s*)item1;
    struct Tree_node_s* two = (struct Tree_node_s*)item2;
    return one->freq<two->freq;
}


/// prints entry as an int value.
///
/// @param item1 pointer to the value to print
/// @param outfp pointer to output file stream
/// @pre values are pointers to integer values.
static void print_int( const void * item1, FILE * outfp ) {
    if ( ! item1 ) {
        fprintf( outfp, "\n\tWARNING: null entry! This should not happen.\n" );
        return;
    }
    int v1 = *(int*)item1;
    fprintf( outfp, "%d\n", v1 );
    return;
}


//----------------------------------------------------Encoding Hepler functions--------------------------------------------------------------------------

// Function to check if given node is the leaf node
// @param root node to be checked
// returns 1 if leaf node
int isLeaf(struct Tree_node_s* root){
   
   return !(root->left) && !(root->right);
   
}


// Function to find unsigned int for remainer while encoding
// @param b the entire code
// @param rem the number of remainder
// returns unsigned bit of remainder
unsigned int binary_to_uint_remainder( char *b,int rem){

  int start = strlen(b)-rem;
  int end = strlen(b);
  char copy[3];
  
  for(int i=start;i<end;i++){
      copy[i-start]=b[i];
  }
  
  unsigned int val = 0;
  int i = 0;

  if(!b){
    return 0;
  }
    
    while (i<rem)
  {  
    val <<= 1;
    val += copy[i]-'0';
    i++;
  }
  
  while(rem<32){
    val <<= 1;
    rem++;
  }
  
  return val;
}



// Function to find unsigned int for code while encoding
// @param b the entire code
// @param count count of the 32 bit group
// returns unsigned bit of given group
unsigned int binary_to_uint( char *b,int count){
  
  unsigned int val = 0;
  int i = 0;

  if (b == NULL)
    return 0;
    
  while (i<32){  
    val <<= 1;
    val += b[i+(count*32)]-'0';
    i++;
  }

  
  return val;
}