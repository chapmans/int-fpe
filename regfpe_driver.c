#include "nfa.h"
#include "fpe.h"

int main(int argc, char *argv[]) {
  
  int* key = (int*) malloc(4*4);
  key[0] = 0x01020304;
  key[1] = 0xcafebabe;
  key[2] = 0xdeadbeef;
  key[3] = 0xdeafbabe;

  if (argc > 2) {
    do_fpe(argv[1], argv[2], key);
  }

  return 0;
}
