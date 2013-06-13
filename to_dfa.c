#include "nfa.h"

static int visiting = 0;
int dsize = 1;
int accepting = 0;

DNODE* droot;
int alen;

/* valcmp
 * Comparator for 2 NFA nodes.
 */
static int valcmp(const void *a, const void *b) {
  NODE *s = (NODE*) a;
  NODE *t = (NODE*) b;

  if (s->id > t->id)
    return 1;
  else if (s->id < t->id)
    return -1;
  else
    return 0;

}


/* setcmp
 * Comparator for 2 sets.
 */
static int setcmp(SET *a, SET *b) {
  /* check the sizes first */
  if (a->num > b->num) 
    return 1;
  else if (a->num < b->num) 
    return -1;
  
  NODE** u = a->ns;
  NODE** v = b->ns;

  /* check if the IDs match match */
  int i;
  for(i = 0; i < a->num; i++) {
    if (u[i]->id > v[i]->id) 
      return 1;
    else if (u[i]->id < v[i]->id) 
      return -1;
  }
  return 0;
}


/* get_closure
 * Get the epsilon closure set for each node. 
 */
SET* get_closure(SET* nodes, NODE* nd) {
  if (nd == NULL) return nodes;
  if (nd->v == visiting) return nodes;
  //printf("\t %d", nd->id);
  int c = nd->c;
  if (c == EP) {
    //printf("!");
    nodes->ns[nodes->num++] = nd; // add empty node to set
    if (nd->accept == 1) nodes->accept = 1;
    nd->v = visiting;
    nodes = get_closure(nodes, nd->n);
  } else if (c == BRANCH) { // add branched nodes to set
    //printf("!");
    nodes->ns[nodes->num++] = nd;
    if (nd->accept == 1) nodes->accept = 1;
    nd->v = visiting;
    nodes = get_closure(nodes, nd->n);
    nodes = get_closure(nodes, nd->n1);
  } /*else {
    printf(".");
    nodes->ns[nodes->num++] = nd;
    if (nd->accept == 1) nodes->accept = 1;
    nd->v = visiting;
  }*/

  if (nodes->num >= nodes->block) {
    nodes->block += 10;
    NODE** nts = (NODE**) realloc(nodes->ns, sizeof(NODE*) * nodes->block);
    if (nts != NULL) nodes->ns = nts;
    else printf("pls");
  }

  return nodes;
}


/* node2d
 * Converts each NFA node to a DFA node.
 */
DNODE* node2d(NODE* nd, DNODE* place) {
  if (nd == NULL) return NULL;
  
  int c = nd->c; // get character of node  
  //printf("node char: %c (%d) (%d)\n", c, c, nd->accept);

  if (c >= 32 && c <= 129) { // check if node is a character
    /* create set */
    SET *startset = (SET*) malloc(sizeof(SET));
    startset->num = 1;
    startset->block = 10;
    startset->ns = (NODE**) calloc(startset->block, sizeof(NODE*));
    startset->ns[0] = nd; // add nfa node into set
    startset->accept = nd->accept == 1 ? 1 : 0;
    
    /* get set of nodes */
    visiting++;
    nd->v = visiting;
    //printf("closure: \n[%d] ", nd->id);
    get_closure(startset, nd->n); // get set of nodes
    if (nd->c == BRANCH) get_closure(startset, nd->n1);
    //printf(" [size %d]\n", startset->num);
    /*int q;
    printf("set: ");
    for (q = 0; q < sset->num; q++) {
      printf("%d(%c) ", sset->ns[q]->r, sset->ns[q]->c);
    }
    printf("\n");*/

    /* get/create dfanode corresponding to set */
    DNODE* dn = get_dnode(startset);
    char l;
    if (c == DIGITS)
      for (l = '0'; l <= '9'; l++) place->next[alphabet->in[l-32]] = dn;
    else if (c == LOWERS)
      for (l = 'a'; l <= 'z'; l++) place->next[alphabet->in[l-32]] = dn;
    else if (c == UPPERS)
      for (l = 'A'; l <= 'Z'; l++) place->next[alphabet->in[l-32]] = dn;
    else
      place->next[alphabet->in[c-32]] = dn; // link dfa node to transitions
    return dn;
  }
  return NULL;
}

/* get_dnode
 * Creates or gets a DFA node based on the set.
 */
DNODE* get_dnode(SET* s) {
  qsort(s->ns, s->num, sizeof(NODE*), valcmp); // sort states

  // see if there exists a dfa node that already matches the set
  DNODE* t = droot;
  int last = 0;
  DNODE* prev;
  int y;
  while (t != NULL) {
    prev = t;
    y = setcmp(s, t->s);
    if (y == -1) {
      last = -1;
      t = t->l;
    } else if (y == 1) {
      last = 1;
      t = t->r;
    } else {
      free(s);
      return t;
    }
  }

  // create dfa node if one doesn't exist
  DNODE* newnode = (DNODE*) malloc(sizeof(DNODE));
  newnode->next = (DNODE**) calloc(alphabet->alen,sizeof(DNODE*));
  newnode->id = dsize++;
  newnode->s = s;
  newnode->done = 0;
  newnode->l = NULL;
  newnode->r = NULL;
  if (last == -1) prev->l = newnode; // insert into tree
  else prev->r = newnode;
  return newnode;
}

/* build_dfa
 * Builds the DFA.
 */
DNODE* build_dfa(NODE* nd) {
  
  /* create set */
  SET *startset = (SET*) malloc(sizeof(SET));
  startset->num = 1;
  startset->block = 10;
  /* add current (first) nfa node into set */
  startset->ns = (NODE**) calloc(startset->block,sizeof(NODE*));
  startset->ns[0] = nd;
  startset->accept = nd->accept == 1 ? 1 : 0;

  // create dfa node
  DNODE *dfanode = (DNODE*) malloc(sizeof(DNODE));
  dfanode->next = (DNODE**) calloc(alphabet->alen,sizeof(DNODE*));
  dfanode->s = startset; // set of dfa node
  dfanode->id = dsize++;
  dfanode->done = 0;
  dfanode->l = NULL;
  dfanode->r = NULL;
  visiting = 1;
  nd->v = visiting;
  //printf("closure: \n [%d]", nd->id);
  startset = get_closure(startset, nd->n); // grab ep-closure
  if (nd->c == BRANCH) startset = get_closure(startset, nd->n1);
  //printf(" [size %d]\n", startset->num);
  qsort(startset->ns, startset->num, sizeof(NODE*), valcmp); // sort states
  droot = dfanode;
  
  /* create dfa with this starting node */
  to_dfa(dfanode);
  
  /* free nfas */
  return dfanode;
}

/*  to_dfa
 *  Recursive section in building DFA.
 */
int to_dfa(DNODE* lastnode) {
  if (lastnode == NULL) return 0; 
  if (lastnode->done == 1) return 0; // return if last node was completed
  lastnode->done = 1;
  SET *s = lastnode->s;
  int m;
  //printf("entering node %d (%d)\n",lastnode->id, s->num);
  //printf("iterating\n");

  /* go through nodes of the set */
  for (m = 0; m < s->num; m++) { 
    //printf("%d %d\n", lastnode->at, m);
    //if (lastnode->at > m) return 0;
    //lastnode->at = m;
    NODE* b = s->ns[m];
    if (b != NULL) {
    //printf("set %d @ %d\n", lastnode->id, b->id);
      DNODE* x = NULL;
      DNODE* y = NULL;

      if (b->n != NULL) x = node2d(b->n, lastnode);
      if (b->c == BRANCH && b->n1 != NULL) y = node2d(b->n1, lastnode);
    
      //printf("dfa node %x to node %x\n", &lastnode, &x);
      
      if (x != NULL) {
        //printf("%d to %d\n", lastnode->
        to_dfa(x);
      }
      if (y != NULL) {
        to_dfa(y);
      }
    
    }
    //m = lastnode->at;
  }
  if (s->accept == 1) accepting++;

  return 0;
}

/*DFA* get_dfa() {
  DFA* dfa = (DFA*) malloc(sizeof(DFA));
  dfa->alphabet = outalpha;
  dfa->finalStates = (int*) malloc(sizeof(int)*95);
  dfa->numAccept =
  dfa->numStates =
  dfa->start = 1;

}*/

/* build_transition_table
 * Builds the transition table from DFAs. rows = states, cols = letter
 */
int** build_transition_table(DNODE* p) {
  printf("states: %d\n", dsize);

  int i;
  printf("    ");
  for (i = 0; i < alphabet->alen; i++) {
    printf("%2c ", alphabet->out[i]);
  }
  int** delta = (int**) malloc(sizeof(int*) * dsize);
 
  delta[0] = (int*) malloc(sizeof(int) * alphabet->alen);
  delta[p->id] = (int*) malloc(sizeof(int) * alphabet->alen);
  //printf("id %d\n", p->id);
  for (i = 0; i < alphabet->alen; i++) {
    delta[0][i] = 0;
    if (p->next[i] != NULL) {
      delta[p->id][i] = p->next[i]->id;
      /*int k;
      printf("adding %d: ", p->next[i]->id);
      for (k = 0; k < p->next[i]->s->num; k++) {
        printf("%d ", p->next[i]->s->ns[k]->id);
      }
      printf("\n");*/
    } else
      delta[p->id][i] = 0;
  }
  tree_table(p->l, delta);
  tree_table(p->r, delta);
  int j;
  printf("\n");
  for (i = 0; i < dsize; i++) {
    if (delta[i] != NULL) {
      printf("%2d: ", i);
      for (j = 0; j < alphabet->alen; j++) {
        printf("%2d ", delta[i][j]);
      }
      printf("\n");
    }
  }
  /*printf("\n");
  printf("label %d (%d)\n", p->id, p->s->accept);
  for (u = 0; u < alphabet->alen; u++) {
    if (p->next[u] != NULL) {
      printf("%d ",p->next[u]->id);
    } else {
      printf("0 ");
    }
  }
  printf("\n");
  tree_table(p->l);
  tree_table(p->r);
  printf("\n");*/
  return delta;
}

/* tree_table
 * Recursive version for building a transition table.
 */
int** tree_table(DNODE* p, int** delta) {
  if (p == NULL) return 0;
  
  delta[p->id] = (int*) malloc(sizeof(int) * alphabet->alen);
  int i;
  //printf("id %d\n", p->id);
  for (i = 0; i < alphabet->alen; i++) {
    if (p->next[i] != NULL) {
      delta[p->id][i] = p->next[i]->id;
      /*int k;
      printf("adding %d: ", p->next[i]->id);
      for (k = 0; k < p->next[i]->s->num; k++) {
        printf("%d ", p->next[i]->s->ns[k]->id);
      }
      printf("\n");*/
    } else
      delta[p->id][i] = 0;
  }
  //printf("label %d (%d)\n", p->id, p->s->accept);
  /*for (u = 0; u < alphabet->alen; u++) {
    if (p->next[u] != NULL) {
      printf("%d ", p->next[u]->id);
    } else {
      printf("0 ");
    }
  }
  printf("\n");*/
  tree_table(p->l, delta);
  tree_table(p->r, delta);
  return delta;
}

int* get_accepting(DNODE* p) {
  if (p == NULL) return NULL;

  int* finstates = (int*) malloc(sizeof(int) * accepting);
  int c = 0;
  if(p->s->accept == 1) {
    finstates[c] = p->id;
  }

  c = find_accepting(finstates,p->l,c);
  c = find_accepting(finstates,p->r,c);

  return finstates;
}

int find_accepting(int* finstates, DNODE* p, int c) {
  if (p == NULL) return c;

  if (p->s->accept == 1) {
    finstates[c++] = p->id;
  }
  c = find_accepting(finstates, p->l, c);
  c = find_accepting(finstates, p->r, c);
  return c;
}

/* finish
 * Frees nodes.
 */
int finish(DNODE* p) {
  if (p->l != NULL) finish(p->l);
  if (p->r != NULL) finish(p->r);
  free(p->s->ns);
  free(p->s);
  free(p->next);
  free(p);
  return 0;
}
