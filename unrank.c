#include "fpe.h"

char *
/*
unrank( int rank, int length, int q0, int ** delta, int ** table,
        char (* unord) (int) ){
*/
unrank( DFA * dfa, int rank, int length, int ** table, char * unord ){

  /* Create space for and null terminate the result */
  char * res;
  res = (char *) malloc( length + 1 );
  res[length] = '\0';

  int q = dfa->start;
  int j = 0;
  int i;

/*
  for( i=0; i<length; i++ ){
    while( rank >= table[delta[q][j]][length-(i+1)] ){
      rank -= table[delta[q][j]][length-(i+1)];
      j++;
    }
    res[i] = unord(j);
    q = delta[q][j];
    j = 0;
  }
*/
  for( i=0; i<length; i++ ){
    while( rank >= table[dfa->delta[q][j]][length-(i+1)] ){
      rank -= table[dfa->delta[q][j]][length-(i+1)];
      j++;
    }
    res[i] = unord[j];
    q = dfa->delta[q][j];
    j = 0;
  }

  return res;
}

