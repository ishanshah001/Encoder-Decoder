
// Project 2 - Packman, the Encoder-Decoder
//
// file: packman.c
//
// File takes two command line arguments and either encodes the content of the first, un-encoded file into a second, encoded file, 
// or decodes the content of the first, encoded file into a second, decoded file.
// 
// The program decides whether to encode or decode based on the presence or absence of a magic number at the start of the file content.
//
// Author: Ishan Shah is4761


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HeapDT.h"
#include "packman_utils.h"
#include "packman_helper.h"


// stores the code
char lut[1000][1000];

// stores the symbol
uchar *lut_data;

// used for size of lut
int count = 0;


//----------------------------------------------------Encoding Hepler functions--------------------------------------------------------------------------



// Loops through lut and returns the code to encode
// @param val the value for which code is required
static char *get_code(uchar val){

  for(int i = 0;i<count;i++){
  
    if(lut_data[i]==val){
        return lut[i];
    }
  }
  return NULL;
}


// Converts tree into binary and creates look up table
// Traverses through all nodes and adds 0 if on the left and 1 if on the right
// @param root root of the tree to be parsed
// @param arr stores binary code
// @param top checks how many digits to be stored
static void Binary(struct Tree_node_s* root, char arr[], int top){

    if (root->left) {
 
        arr[top] = '0';
        Binary(root->left, arr, top + 1);
    }
 
    
    if (root->right) {
 
        arr[top] = '1';
        Binary(root->right, arr, top + 1);
    }
 
    if (isLeaf(root)) {
          char c[1000] = "";
          char check[1000] = "";
         lut_data[count]=root->sym;         
        
        
        for (int i = 0; i < top; ++i){
        
            // storing required codes in a single array of char
            check[i]=arr[i];
        
        }
        strcat((char *)c,check); // converting array of char to string
        
        strncpy((char *)lut[count],(char *)c,strlen(c));
        
        count++;

    }
}


// Function to create new node of struct Tree_node_s type
// @param sym symbol 
// @param freq frequency of the symbol
// @returns new node
static struct Tree_node_s* newNode(char sym, int freq)
{
	struct Tree_node_s* temp = (struct Tree_node_s*)malloc(sizeof(struct Tree_node_s));
  
  temp->left = NULL;
  temp->right = NULL;
	temp->sym = sym;
	temp->freq = freq;

	return temp;
}


// The function enocdes the data from first file and stores in second
// @param first pointer to first file
// @param second pointer to second file
static void encode(FILE *first, FILE *second){
  
  
  unsigned int freq[BUFSIZE];
  
  uchar input;
  
  // initialize all 256 as 0
  for(int i=0;i<BUFSIZE;i++){
    freq[i] = 0;
  }
  
  while(fread(&input,1,sizeof(char),first)==1){
    freq[input]++;
  }
  
  
   int size = sizeof(freq)/sizeof(freq[0]); 
   Heap theheap = NULL;
   Heap newheap = NULL;
   theheap = hdt_create( size, cmpintmin, print_int );
   newheap = hdt_create( size, cmpintmin, print_int );
   
   for ( size_t i = 0; i < size; i++ ) {
     if(freq[i]>0){
        hdt_insert_item( theheap, newNode(i, freq[i]));
     }     
    }
   
   
   struct Tree_node_s *l, *r, *top;
    
    //loops till root is reached
  	while (hdt_size(theheap)>1) {
    
      //lowest frequency is added to the left
  		l = (struct Tree_node_s*)hdt_remove_top(theheap);
      //second lowest frequency is added to the right
  		r = (struct Tree_node_s*)hdt_remove_top(theheap);
      
      //cumulative frequencies of left and right are added to top with * as symbol
      top = newNode("0", l->freq + r->freq);
  
  		top->left = l;
  		top->right = r;
      
      // top is readded to the heap
  		hdt_insert_item(theheap, top);
     
      // left of top and right of top are added to a new heap
      hdt_insert_item(newheap, top->left);
      hdt_insert_item(newheap, top->right);      
     
     }
     
     //stores the root
     struct Tree_node_s * root = (struct Tree_node_s*)hdt_top(theheap);
     
     hdt_insert_item(newheap, (struct Tree_node_s*)hdt_remove_top(theheap));
     
     
     char arr[size*2];
     int t = 0;
     
     lut_data = (uchar*)malloc(size*sizeof(uchar));
     
     
     //generates the lookup table
     Binary(root, arr, t);     
     
     //writes tree to the file
     write_tree(second,root);
     
     fseek(first, 0, SEEK_SET);
     char convert[1000][1000]={};
     char * code=NULL;
     while(fread(&input,1,sizeof(char),first) == 1){
       code = get_code(input);
       strcat((char *)convert,code);
     }
     
     unsigned int numbits = strlen(convert);
     int num = strlen(convert)/32;
     int rem = strlen(convert)%32;
     unsigned int number[num+1];
     
     //gets the unisgned ints in groups of 32 bits
     for(int i=0;i<num;i++){
       number[i] = binary_to_uint(convert,i);
     }
     
     //gets unsigned bits of the remaining bits
     number[num] = binary_to_uint_remainder(convert,rem);
      
     //writes numints
     fwrite(&numbits, sizeof(unsigned int), 1, second);
     
     //writes encoded data
     for(int i=0;i<num+1;i++){
       
       fwrite(&(number[i]), sizeof(unsigned int), 1, second);
     
     }
     
     
     free_tree_node(root);
     hdt_destroy( theheap );
     hdt_destroy( newheap );
     free(lut_data);
     
     fclose(first);
     fclose(second);
     
  

}


//----------------------------------------------------Decoding-------------------------------------------------------------------------


// Takes the code and decodes and either saves in another file or prints on screen
// @param root root of the huffman tree
// @param s contains the code to be decoded
// @param f2 pointer: prints on screen if stdout else saves decoded output in a file
static void decode_file(struct Tree_node_s* root, char s[], FILE * fp2){

    char ans[1000][1000]={};
    int size=0;
    
    struct Tree_node_s* curr = root;
    
    for (int i=0;i<strlen(s);i++)
    {   
        if (s[i] == '0'){
           curr = curr->left;
        }
        else{
           curr = curr->right;
        }
  
        
        if (isLeaf(curr))
        {
            char * data = curr->sym;
            
            strcat((char *)ans,&data);
            size++;
            curr = root;
        }
    }
    strcat((char *)ans,"\0");
    if(fp2==stdout){
      printf("%s",ans);
    }
    else{
      fwrite(ans, 1, size, fp2);
    }
    free_tree_node(curr);
    fclose(fp2);
    
}



// Reads data from encoded file and converts to unsigned int to be printed or saved to a file
// @param first pointer to encoded file
// @param second: points to file where decoded data needs to be saved or to stdout

static int decode(FILE *first, FILE *second){
  
  fseek(first, 0, SEEK_SET);
  
  
  // gets the root of the tree
  struct Tree_node_s* root = read_tree(first);

  unsigned numbits;
  if(!fread(&numbits,1,sizeof(unsigned int),first)){
    return EXIT_FAILURE;
  }
  
  int num = numbits/32;
  int rem = numbits%32;
  
  unsigned int number[num+1];
  
  for(int i = 0;i<num;i++){
    
    if(!fread(&number[i],1,sizeof(unsigned int),first)){
      return EXIT_FAILURE;
    }
  }
  
  fread(&number[num],1,sizeof(unsigned int),first);
  
  // brings the remainder back to normal form
  while(rem<32){
    number[num] >>= 1;
    rem++;
  }
  
  // stores the remainder in binary form in reverse order
  int arr_rem[numbits];
  int k =0, arr_rem_size=0;
  while(number[num]>0)    
    {  
      arr_rem[k]=number[num]%2;
      number[num]/=2;
      arr_rem_size = ++k;    
    }  
  
  
  
  int arr_size=0;
  int a[numbits];
  //stores bits in reverse order
  for(int i =0;i<num;i++){
    int j =0;
    while(number[i]>0)    
    {  
      a[j]=number[i]%2;
      number[i]/=2;
      arr_size = ++j;    
    }    
  }
  
  int reverse_arr[arr_size];
  //to reverse the array
  for(int i =0;i<arr_size;i++){
    reverse_arr[i]=a[arr_size-i-1];
  }
  
  //to add the remainder bits
  for(int i =0;i<arr_rem_size;i++){
    reverse_arr[arr_size+i]=arr_rem[arr_rem_size-i-1];
  }
  
  char code[1000][1000]={};
  
  for(int i =0;i<arr_size+arr_rem_size;i++){
    
    if(reverse_arr[i]==0){
      strcat((char *)code,"0");
    }
    else{
      strcat((char *)code,"1");
    }
    
  }
  
  decode_file(root,code,second);
  
  free(lut_data);
 
  fclose(first);
  return 1;

}


//-------------------------------------------------------input/output----------------------------------------------------------------------


// Takes two command line arguments and either encodes the content of the first, un-encoded file into a second, encoded file, 
// or decodes the content of the first, encoded file into a second, decoded file.
int main(int argc, char* argv[]){
  
  
  if(argc!=3){
    
    fprintf(stderr, "usage: packman firstfile secondfile\n");
        return EXIT_FAILURE;
    
  }
  
  FILE* fp1 = fopen( argv[1], "rb");
  
  if ( !fp1) {
        fprintf(stderr, "Failed to open file: No such file or directory\n");
        return EXIT_FAILURE;
    }
    
  //read magic no
  
  unsigned short magic;
  
  if(!(fread(&magic, sizeof(unsigned short),1,fp1))){
    return EXIT_FAILURE;
  }
  else if(magic==get_magic()){
    if(strcmp(argv[2],"-")==0){
      //decode to stdout
      FILE *fp2 = stdout;
      decode(fp1,fp2);
    }
    else{
      
      FILE *fp2 = fopen(argv[2],"w");
      
      if ( !fp2) {
        fprintf(stderr, "Failed to open file: No such file or directory\n");
        return EXIT_FAILURE;
      }
      
      //decode to fp2
      decode(fp1,fp2);
      
    }
    
  }
  else{
      
      fclose(fp1);
      fp1 = fopen(argv[1],"r");
      FILE *fp2 = fopen(argv[2],"wb");
      
      if ( !fp1 ) {
        fprintf(stderr, "Failed to open file: No such file or directory\n");
        return EXIT_FAILURE;
      }
      if ( !fp2 ) {
        fprintf(stderr, "Failed to open file: No such file or directory\n");
        return EXIT_FAILURE;
      }
      //encode to fp2
      encode(fp1,fp2);
      
      
  }
  
  return EXIT_SUCCESS;
  
  
}

