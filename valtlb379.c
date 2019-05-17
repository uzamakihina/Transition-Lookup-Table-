#include<math.h>
#include<time.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<stdint.h>
#define MAX_INPUT 1024
#define MOD 256

struct page {
  unsigned long memory;
  struct page * next;
  struct page * prev;
  struct page * nextHash;
  struct page * prevHash;
};

int getArguments(int argc, char * argv[], int * flushperiod,  int * pageSize, int * TLBsize, bool * isLRU, bool * isIgnoringI);
struct page * searchHash(struct page * searchNode, unsigned long address);
struct page * insertMain(struct page * nodeToInsert, struct page * mainHead);
struct page * insertHash(struct page * nodeToInsert, struct page * hashHead);
struct page * removeTail(struct page * tail);

int main(int argc, char * argv[]) {
    // Initialize all the counters and command line arguments, and scan them in
    bool isLRU, isIgnoringI = false;
    int flushperiod = 0, pageCount = 0, TLBmisses = 0, memoryAccesses = 0, pageSize, TLBsize;
    int exponent = getArguments(argc, argv, &flushperiod,  &pageSize, &TLBsize, &isLRU, &isIgnoringI);

    // Initialize linked list and hash map
    struct page * mainHead = NULL, * mainTail = NULL;
    struct page ** map = calloc(MOD, sizeof(*map));
    // Skip initial lines
    char line[MAX_INPUT] = {'='};
    while (line[0] == '=') fgets(line, MAX_INPUT, stdin);
    // Main loop
    do {
      // Ignore when -i is specified
      if (line[0] == 'I' && isIgnoringI) continue;
      // Signals end of input
      if (line[0] == '=') break;
      // Scan each line for its memory reference and its size
      unsigned long hex;
      int i = 2, accessSize;
      sscanf(line + i, "%lx", &hex);
      while (line[i++] != ',');
      accessSize = atoi(line + i);
      // Determine whether the 2nd address is within the bounds of the 1st page
      unsigned long pageAddress  =  hex               >> exponent;
      unsigned long pageAddress2 = (hex + accessSize) >> exponent;
      checkSecondPage:

      // Logic for clearing the TLB when -f is specified (0=no-flush is the default)
      ++memoryAccesses;
      if (flushperiod != 0 && (memoryAccesses % flushperiod == 0) && pageCount != 0) {
          // Reset the page count
          pageCount = 0;
          // Free the entire list
          while (mainHead->next != NULL) {
              mainHead = mainHead->next;
              free(mainHead->prev);
          }
          free(mainHead);
          // Make all available pointers null
          mainHead = NULL;
          mainTail = NULL;
          for (int i = 0; i < MOD; i++) map[i] = NULL;
      }

      // See if address in its hash bucket, NULL if not found
      struct page * node = searchHash(map[pageAddress % MOD], pageAddress);
      // Found an entry, do nothing for FIFO
      if      (node != NULL && !isLRU);
      // Found an entry, if it's not the head, make it the head
      else if (node != NULL &&  isLRU) {
          if (node != mainHead) {
              if (node == mainTail) mainTail = removeTail(mainTail);
              mainHead = insertMain(node, mainHead);
              // If it's not the head of the hash bucket, make it the head
              if (node != map[pageAddress % MOD]) {
                map[pageAddress % MOD] = insertHash(node, map[pageAddress % MOD]);
              }
          }
      }
      // Did not find an entry, record a miss, allocate a new page, and make it the head
      else if (node == NULL) {
          TLBmisses += 1;
          node = calloc(1, sizeof(*node));
          node->memory = pageAddress;
          mainHead               = insertMain(node, mainHead);
          map[pageAddress % MOD] = insertHash(node, map[pageAddress % MOD]);
          // Logic for maintaining the tail and the size of the TLB
          if (pageCount < TLBsize) ++pageCount;
          else mainTail = removeTail(mainTail);
          if (pageCount == 1) mainTail = node;
      }
      // If the memory access spanned two pages, do another memory access before scanning a new line
      if (pageAddress != pageAddress2) {
          pageAddress = pageAddress2;
          goto checkSecondPage;
      }
    } while (fgets(line, MAX_INPUT, stdin) != NULL);
    printf("%d\n%d\n%d\n", memoryAccesses, TLBmisses, memoryAccesses - TLBmisses);
    return 0;
}

// Get all the command line arguments in one function to clean up main function
int getArguments(int argc, char * argv[], int * flushperiod,  int * pageSize, int * TLBsize, bool * isLRU, bool * isIgnoringI) {
  char c;
  while ((c = getopt(argc-2,argv,"if:p:"))!= -1 ) {
    switch(c){
      case 'i':
        *isIgnoringI = true;
        break;
      case 'p':
        if      (strcmp(optarg, "LRU" ) == 0) *isLRU = true;
        else if (strcmp(optarg, "FIFO") == 0) *isLRU = false;
        else exit(-1);
        break;
      case 'f':
        *flushperiod = atoi(optarg);
        break;
      case '?':
        break;
    }
  }
  *pageSize = atoi(argv[argc - 2]);
  *TLBsize = atoi(argv[argc - 1]);
  for (int i = 0; i < 17; i++)
        if (pow(2,i) == *pageSize)
                return i;
  exit(-1);
}

// Iterate through hash links until you find a match or reach the end
struct page * searchHash(struct page * searchNode, unsigned long address) {
    while (searchNode != NULL) {
      if (searchNode->memory == address) return searchNode;
      searchNode = searchNode->nextHash;
    }
    return NULL;
}

// Insert a node at the head of the main linked list
struct page * insertMain(struct page * nodeToInsert, struct page * mainHead) {
    // extract node from main linked list
    if (nodeToInsert->prev    != NULL) nodeToInsert->prev   ->next    = nodeToInsert->next;
    if (nodeToInsert->next    != NULL) nodeToInsert->next   ->prev    = nodeToInsert->prev;
    // replace head of main linked list
    if (mainHead != NULL) mainHead->prev = nodeToInsert;
    nodeToInsert->prev = NULL;
    nodeToInsert->next = mainHead;
    return nodeToInsert;
}

// Insert a node at the head of the appropriate hash linked list
struct page * insertHash(struct page * nodeToInsert, struct page * hashHead) {
    // extract node from hash linked list
    if (nodeToInsert->prevHash != NULL) nodeToInsert->prevHash->nextHash = nodeToInsert->nextHash;
    if (nodeToInsert->nextHash != NULL) nodeToInsert->nextHash->prevHash = nodeToInsert->prevHash;
    // replace head of hash linked list
    if (hashHead != NULL) hashHead->prevHash = nodeToInsert;
    nodeToInsert->prevHash = NULL;
    nodeToInsert->nextHash = hashHead;
    return nodeToInsert;
}

// Logic for dereferencing the appropriate pointers when evicting an element
struct page * removeTail(struct page * tail) {
    if (tail->prevHash != NULL) tail->prevHash->nextHash = NULL;
    if (tail->prev != NULL) {
        tail->prev->next = NULL;
        tail->prev->nextHash = NULL;
    }
    struct page * newTail = tail->prev;
    tail->prev = NULL;
    tail->prevHash = NULL;
    return newTail;
}
