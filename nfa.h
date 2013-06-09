#define BRANCH 127
#define EP 128
#define DIGITS 129
#define UPPERS 130
#define LOWERS 131

typedef struct ALPHA ALPHA;
typedef struct NODE NODE;
typedef struct NFA NFA;
typedef struct SET SET;
typedef struct DNODE DNODE;
//typedef struct DFA DFA;

struct ALPHA {
  int alen;
  char in[95];
  char out[95];
};

struct NODE {
  int c;            // character. range: 32-126; special: 127+
  NODE *n;          // next state to go to
  NODE *n1;         // next state to go to (for branching)
  int id;           // id value for node
  int accept;       // if it's an accept state
  int v;            // used to mark if visited when building DFA
};

struct NFA {
  NODE *start_c;    // starting point for concatenation
  NODE *start_s;    // starting point for star
  NODE *cur;        // current (last) NFA node
  char *back;       // index pointer when parsing regex
};

struct SET {
  NODE **ns;        // list of nodes
  int num;          // amount of nodes in the set
  int block;        // for memory allocation
  int accept;       // if it's an accepting node
};

struct DNODE {
  SET *s;           // set mapping to DFA node
  DNODE **next;     // next DFA node
  int done;         // check if node is begin/has been created
  int id;           // id for DFA
  DNODE *l;         // used for tree traversal to check
  DNODE *r;         //  if the sets are equivalent
};
/*
struct DFA {
  char* alphabet;
  int** delta;      // transition table
  int* finalStates; // accepting states
  int numAccept;    // number of accepting states
  int numStates;    // number of total states
  int start;        // start state
};
*/
extern int dsize;         // size of the DFA
extern int accepting;     // amount of accepting states
extern DNODE* droot;      // DFA node root
extern ALPHA* alphabet;

/* to_nfa methods */
NODE* build_nfa(char* regex);
NFA* to_nfa(NFA *begin, char *regex);
int traverse(NODE *nd);
ALPHA* build_alphabet();
int checkmatch(NODE *nd, char* l);

/* to_dfa methods */
static int valcmp(const void* a, const void* b);
static int setcmp(SET *a, SET *b);
SET* get_closure(SET* nodes, NODE* nd);
DNODE* node2d(NODE* nd, DNODE* place);
DNODE* get_dnode(SET* s);
DNODE* build_dfa(NODE* nd);
int to_dfa(DNODE* lastnode);
int** build_transition_table(DNODE* p);
int** tree_table(DNODE* p, int** delta);
int* get_accepting(DNODE* p);
int find_accepting(int* finstates, DNODE* p, int c);
int finish(DNODE* p);

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
