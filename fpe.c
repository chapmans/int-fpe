#include "fpe.h"
#include "aes.h"

int * ord( char * message );
char * unord( char * message );

char *
fpe( void * key, int tweak, DFA * dfa, char * message ){

  int i, j;

  int max;
  int * order = (int *) ord( dfa->alphabet );
  char * unorder = (char *) unord( dfa->alphabet );
  int radix = strlen( dfa->alphabet );
  int length = strlen( message );

  char * unranked;

  int error;
  for( i=0; i<strlen(message); i++ ){
    error = 1;
    for( j=0; j<strlen(dfa->alphabet); j++ ){
      if( message[i] == dfa->alphabet[j] )
        error = 0;
    }
    if( error )
      return "";
  }

  int state = dfa->start;
  for( i=0; i<strlen(message); i++ ){
    state = dfa->delta[state][order[message[i]]];
  }
  error = 1;
  for( i=0; i<dfa->numAccept; i++ )
    if( state == dfa->finalStates[i] )
      error = 0;
  if( error )
    return "";

  /* Build the table */
  int ** table;
  table = (int **) build_table( dfa, radix, length );
  max = table[dfa->start][length];

  /* Can remove this section */
  printf("Built table is:\n");
  for( i=0; i<dfa->numStates; i++ ){
    for( j=0; j<length+1; j++ ){
      printf("%d ", table[i][j] );
    }
    printf("\n");
  }
  printf("And the number of strings of length %d is %d\n", length, max );

  unsigned int * rankNum = (unsigned int *) malloc( 4*sizeof(unsigned int) );
  rankNum[1] = rankNum[2] = rankNum[0] = 0;
  rankNum[3] = rank( dfa, order, table, message );
  
  /* Can remove this section as well */
  printf("\nWhen you rank %s you get %d\n", message, rankNum[3] );

  int opt_out_counter = 0;
  aes_encrypt_ctx ctx[1];
  aes_encrypt_key128( key, ctx );
  unsigned int * encNum = (unsigned int *) malloc( 4*sizeof(unsigned int) );
  do{
    aes_encrypt( rankNum, encNum, ctx );
    for( i=0; i<4; i++ ){
      rankNum[i] = encNum[i];
    }
    if(opt_out_counter > 100000)
      break;
    opt_out_counter++;
  } while( encNum[3] > max || encNum[3] < 0 );

  encNum[3] %= max;
  printf("Encrypted number is %d\n", encNum[3] );

  /* Do the unranking */
  unranked = (char *) unrank( dfa, encNum[3], length, table, unorder );

  return unranked;
}

int * ord( char * alphabet ){
  int * order = (int *) malloc( 256 * sizeof(int) );

  int i;
  for( i = 0; i < 256; i++ ){
    order[i] = 0;
  }

  for( i = 0; i < strlen( alphabet ); i++ ){
    order[alphabet[i]] = i;
  }

  return order;
}

char * unord( char * alphabet ){
  char * unorder = (char *) malloc( strlen( alphabet ) );
  int i;

  for( i=0; i<strlen( alphabet ); i++ ){
    unorder[i] = alphabet[i];
  }

  return unorder;
}
