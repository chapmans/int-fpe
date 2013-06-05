#include <stdlib.h>
#include "aes.h"

int main() {
  unsigned char * key = (unsigned char *) malloc( 16 );

  char * message = (char *) malloc( 128 );
  char * cipher = (char *) malloc( 128 );

  int i;
  for( i=0; i<128; i++ ){
    message[i] = rand() & 0xFF;
    cipher[i] = message[i];
  }

  for( i = 0; i < 16; i++ ){
    key[i] = i;
  }

  aes_encrypt_ctx ctx[1];

  aes_encrypt_key128( key, ctx );

  long * p1 = message;
  long * p2 = cipher;
  for(i=0; i<32; i++){
    aes_encrypt( p1, p2, ctx );
    p1++;
    p2++;
  }

  long * ptr1 = message;
  long * ptr2 = cipher;

  for( i=0; i<32; i++ ){
    printf("%x %x\n", *ptr1, *ptr2 );
    ptr1++;
    ptr2++;
  }

  return 1;

}
