typedef struct {
  char * alphabet;
  int ** delta;
  int * finalStates;
  int numAccept;
  int numStates;
  int start;
} DFA;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
