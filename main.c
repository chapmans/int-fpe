#include "fpe.h"

/* Currently this is not much but a shallow husk which runs build table,
 * then rank, then unrank, on a single sample dfa.
 */
int
main( int argc, char *argv[] ){

  int i;

  /* Create variables for building table */
  DFA * dfa;
  int * key;
  int tweak;
  char * message;
  char * cipher;

  /* Build an example DFA for testing */
  dfa = (DFA *) malloc( sizeof( DFA ) );

  /* Set the transition table */
  dfa->delta = (int **) malloc( 9*4 );
  for( i=0; i<9; i++ ){
    dfa->delta[i] = (int *) malloc( 36*4 );
  }
  for( i=0; i<36; i++ ){
    if( i>0 && i<10 )
      dfa->delta[0][i] = 1;
    else
      dfa->delta[0][i] = 8;

    if( i>9 ){
      dfa->delta[1][i] = 2;
      dfa->delta[2][i] = 3;
      dfa->delta[3][i] = 4;
    }
    else{
      dfa->delta[1][i] = dfa->delta[2][i] = dfa->delta[3][i] = 8;
    }

    if( i<10 ){
      dfa->delta[4][i] = 5;
      dfa->delta[5][i] = 6;
      dfa->delta[6][i] = 7;
    }
    else{
      dfa->delta[4][i] = dfa->delta[5][i] = dfa->delta[6][i] = 8;
    }
    
    dfa->delta[7][i] = dfa->delta[8][i] = 8;
  }

  /* Set the accepting states */
  dfa->finalStates = (int *) malloc( 4 );
  dfa->finalStates[0] = 7;
  
  /* Set the alphabet */
  dfa->alphabet = (char *) malloc( 37 );
  strcpy( dfa->alphabet, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" );

  /* Miscellaineous values */
  dfa->numAccept = 1;
  dfa->numStates = 9;
  dfa->start = 0;

  /* A not random at all key */
  key = (int *) malloc( 4*4 );
  key[0] = 0x01020304;
  key[1] = 0xcafebabe;
  key[2] = 0xdeadbeef;
  key[3] = 0xdeafbabe;

  /* A not informed at all tweak */
  tweak = 0x12345678;

  /* The starting message */
  message = (char *) malloc( 8 );
  strcpy( message, "3AHF457" );

  cipher = (char *) fpe( key, tweak, dfa, message );
  printf( "The encrypted string is %s\n", cipher );
  
  /* Build the table */
//  t = (int **) build_table( test, 3, 2, 5, a, 2 );
  
  /* Show the table */
/*
  printf("The built table:\n");
  for( i=0; i<3; i++ ){
    for( j=0; j<6; j++ )
      printf( "%d ", t[i][j] );
    printf("\n");
  }
  printf("\n");
  
  int r;
  r = rank( "aabbb", 0, 5, order , t, test );
  printf("Rank of \"%s\" is %d\n", "aabbb", r );

  char * unranked;
  int ur = 2;
  unranked = (char *) unrank( ur, 5, 0, test, t, unorder );
  printf("\nAnd %d unranks to the string \"%s\"\n", ur, unranked );
*/
}
