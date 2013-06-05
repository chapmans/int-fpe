#include "fpe.h"

int
/*
rank( char *str, int q0, int length, int (* ord) (char), int ** table,
      int ** delta ){
*/
rank( DFA * dfa, int * ord, int ** table, char *message ){

  int q = dfa->start;
  int c = 0;
  int i, j;
  int length = strlen( message );

  /* Ranking algorithm */
  for( i=0; i<length; i++ ){
    for( j=0; j<=ord[message[i]]-1; j++ ){
      c += table[dfa->delta[q][j]][length-(i+1)];
    }
    q = dfa->delta[q][ord[message[i]]];
  }
 
  return c;
}

