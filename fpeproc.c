#include "nfa.h"
#include "fpe.h"

char* do_fpe(char* regex, char* message, void* key) {

  /* Handle regex to DFA conversion */
  DFA *comp = (DFA*) malloc(sizeof(DFA));
  comp->start = 1;

  NODE* root = build_nfa(regex);
  ALPHA* alphabet = build_alphabet();
  comp->alphabet = (char*) malloc(sizeof(char) * alphabet->alen);
  strncpy(comp->alphabet, alphabet->out, alphabet->alen);

  DNODE* droot = build_dfa(root);
  comp->delta = build_transition_table(droot);
  comp->numAccept = accepting;
  comp->finalStates = get_accepting(droot);
  comp->numStates = dsize;

  finish(droot);
  freeNode(root); 

  int tweak = 0x12345678;

  char key0[9];
  char key1[9];
  char key2[9];
  char key3[9];
  strncpy(key0, (char*) key, 8);
  strncpy(key1, (char*) key+8, 8);
  strncpy(key2, (char*) key+16, 8);
  strncpy(key3, (char*) key+24, 8);
  key0[8] = '\0';
  key1[8] = '\0';
  key2[8] = '\0';
  key3[8] = '\0';

  int* xkey = (int*) malloc(4*sizeof(int));
  xkey[0] = strtol(key0, NULL, 16);
  xkey[1] = strtol(key1, NULL, 16);
  xkey[2] = strtol(key2, NULL, 16);
  xkey[3] = strtol(key3, NULL, 16);
  printf("%x %x %x %x", xkey[0], xkey[1], xkey[2], xkey[3]);
  char* cipher = (char*) fpe(xkey, tweak, comp, message);
  printf("Encrypted string: %s\n", cipher);

  free(comp->alphabet);
  free(alphabet);
  
  int k;
  for (k = 0; k < comp->numStates; k++) {
    free(comp->delta[k]);
  }
  free(comp->delta);
  free(comp->finalStates);
  free(comp);
  
  dsize = 1;
  accepting = 0;
  
  return cipher;
}

int freeNode(NODE* r) {
  if (r == NULL) return 0;
  freeNode(r->l);
  free(r);
  return 0;
}
