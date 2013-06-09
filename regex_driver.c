#include "nfa.h"
#include "fpe.h"

int main(int argc, char *argv[]) {

  //printf("args: %d\n", argc);
  
  DFA *comp = (DFA*) malloc(sizeof(DFA));
  comp->alphabet;
  comp->delta;
  comp->finalStates;
  comp->numAccept = 0;
  comp->numStates = 0;
  comp->start = 1;
  
  NODE* root;

  if (argc >= 2) {
    root = build_nfa(argv[1]);
    ALPHA* alphabet = build_alphabet();
    comp->alphabet = (char*) malloc(sizeof(char)*alphabet->alen);
    strncpy(comp->alphabet, alphabet->out, alphabet->alen);
    //printf("%s\n", comp->alphabet);
    //traverse(root);
    DNODE* droot = build_dfa(root);
    comp->delta = build_transition_table(droot);
    comp->numAccept = accepting;
    comp->finalStates = get_accepting(droot);
    comp->numStates = dsize;
    printf("accepting states: %d\n", accepting);
    printf("number of states: %d\n", dsize);  
  }

  if (argc == 3) {
    int k = checkmatch(root, argv[2]);
    if (k == 0) {
      printf("match: false\n");
      return 0;
    }
    else {
      printf("match: true\n");
    }
  
    int* key = (int*) malloc(4*4);
    key[0] = 0x01020304;
    key[1] = 0xcafebabe;
    key[2] = 0xdeadbeef;
    key[3] = 0xdeafbabe;

    int tweak = 0x12345678;
    char* message = argv[2];
    char* cipher = (char*) fpe(key, tweak, comp, message);
    printf("The encrypted string is %s\n", cipher);

  }

}
