#include <stdlib.h>
#include <stdio.h> 

typedef struct nfa_node NODE;
typedef struct nfa_el EL;
typedef struct nfa_set SET;
typedef struct dfa_node DNODE;
typedef struct dfa_el DFA;

struct nfa_el {
  NODE *start0;      // start0 = concat, start1 = union
  NODE *start1;
  NODE *cur;
  char *back;
};

static int vi = 0;
static int nsize = 0;
static int dsize = 0;
static DNODE* root;

struct nfa_node {
  int c;             // character; range: 32-126. 127=branch, 128=start/blank
  NODE *n;           // next state to go to
  NODE *n1;          // next state to go to (if branch)
  int r;             // arbitrary assigned value
  int accept;
  int v;
};

struct dfa_node {
  SET *s;
  DNODE *next[95];   // 126 - 32 + 1 valid chars
  int at;
  int done;
  int label;

  DNODE *l; // used for tree traversal to check if sets are equivalent
  DNODE *r;
};

struct dfa_el {
  char* alphabet;
  int ** delta;      // transition table (as an int **)
  int * finalStates; // accepting states (int *)
  int numAccept;     // number of accepting states (int)
  int numStates;     // number of states (int)
  int start;         // start state (int)
};

struct nfa_set {
  NODE **ns;
  int num;
  int accept;
};

EL* to_nfa(EL *begin, char *regex) {
  NODE* start0 = begin->start0;
  NODE* start1 = begin->start1;
  NODE* cur = begin->cur;
  NODE** accepts = (NODE**) malloc(5*sizeof(NODE));
  accepts[0] = cur;
  int ac = 0;

  EL* re = (EL*) malloc(sizeof(EL));
  
  char* i;
  for (i = regex; *i; i++) {
    /* allocate nodes to be filled in */
    NODE* m = (NODE*) malloc(sizeof(NODE));
    m->r = nsize++;
    //printf("*i: %c\n", *i); 
    switch(*i) {
      case '\\': // escape sequence
        i++;
        if (*i == '\\') {
          m->c = '\\';
        } else if (*i == '*') {
          m->c = '*';
        } else if (*i == '+') {
          m->c = '+';
        } else if (*i == '|') { 
          m->c = '|';
        } else if (*i == '(') {
          m->c = '(';
        } else if (*i == ')') {
          m->c = ')';
        } else if (*i == 'd') {
          // allow decimals
        } else {
          break;
        }
        m->accept = 1;
        accepts[ac] = m;
        cur->n = m;
        cur->accept = 0;
        cur = m;
        
        //printf("char: %c\n", m->c);
        start1 = cur;
        break;
      case '*':
        m->c = start1->c;
        m->n = start1->n;
        m->n1 = start1->n1;
        m->accept = start1->accept;
        start1->c = (char) 127;
        start1->n1 = m;
        start1->accept = 1;
        accepts[ac] = m;
      
        //NODE* m1 = (NODE*) malloc(sizeof(NODE));
        if (start1 == cur) {
          cur = m;
        }
        cur->n = start1; // loop back current to m
        cur->accept = 0;
        
        //printf("*: %c(%d) to %c(%d)\n", m->n->c,m->n->c,cur->c, cur->c);
        cur = start1;
        break;
      case '+':
        break;
      case '|':
        m->c = start0->c; 
        m->n = start0->n;
        m->n1 = start0->n1;
        m->accept = start0->accept;
        start0->c = (char) 127; // split node into start/cur and post-cur
        start0->n1 = m;
        start0->accept = 0;

        cur->accept = 1;
        accepts[ac++] = cur;
        accepts[ac] = m;

        //printf("|: %c(%d) to %c(%d)\n", m->n->c, m->n->c, cur->c, cur->c);
        cur = start0;
        break;
      case '(':
        m->c = (char) 128;
        m->accept = 1;
        cur->n = m;
        cur->accept = 0;
        re->start0 = m;
        re->start1 = m;
        re->cur = m;
        re = to_nfa(re, i+1);

        start0 = re->start0;
        start1 = re->start0;
        cur = re->cur;
        accepts[ac] = cur;
        i = re->back;
        break;
      case ')':
        m->c = 128;
        m->accept = 1;
        int j;
        for (j = 0; j <= ac; j++) {
          accepts[j]->accept = 0;
          accepts[j]->n = m;
        }
        ac = 0;

        re->start0 = begin->start0;
        re->start1 = begin->start0;
        re->cur = m;
        re->back = i;
        return re;
        break;
      default: // any letter
        m->c = *i; // next node: add letter i as transition
        m->accept = 1;
        accepts[ac] = m;

        cur->n = m; // this node: to next node
        cur->accept = 0;

        //printf("char: %c\n", m->c);
        cur = m; // point to next node
        start1 = cur;
    }
    //printf("added %c, %d \n", cur->c, cur->accept);

  }
  NODE* m = (NODE*) malloc(sizeof(NODE));
  m->r = nsize++;
  m->c = 128;
  m->accept = 1;
  int j;
  for (j = 0; j <= ac; j++) {
    accepts[j]->accept = 0;
    accepts[j]->n = m;
  }
  re->start0 = begin->start0;
  re->start1 = start1;
  re->cur = cur;
  return re;
}

int traverse(NODE *nd) {
  while (nd != NULL) {
   if (nd->accept == 1) {
     //printf("\n");
   }
   if(nd->c == 128 || nd->c == -128) {
     nd = nd->n;
     //printf("-");
   }
   else if (nd->c == 127) {
     //printf("split\n");
     //printf("<");
     traverse(nd->n1);
     nd = nd->n;
   } else {
     //printf("%c", nd->c);
     nd = nd->n;
   }
  }
  return 0;
}

int checkmatch(NODE* nd, char* l) {
  int m = 0;
  if (nd == NULL) {
    return 0;
  }
  if (nd->accept) {
    if (!*l) return 1;
  }
  if (nd->c == 128 || nd->c == -128) {
    m = checkmatch(nd->n, l);
  } else if (nd->c == 127) {
    m = checkmatch(nd->n1, l);
    if (m == 0) {
      m = checkmatch(nd->n, l);
    }
  } else {
    if (nd->c != *l) {
      m = 0;
    } else {
      m = checkmatch(nd->n, ++l); 
    }
  }
  //printf("%d", m);
  return m;
}

static int valcmp(const void *a, const void *b) {
  NODE *s = (NODE*) a;
  NODE *t = (NODE*) b;

  if (s->r > t->r) {
    return 1;
  } else if (s->r < t->r) {
    return -1;
  } else {
    return 0;
  }

}

static int setcmp(SET *a, SET *b) {
  if (a->num > b->num) return 1;
  else if (a->num < b->num) return -1;
  
  NODE** u = a->ns;
  NODE** v = b->ns;

  int y;
  for(y = 0; y < a->num; y++) {
    if (u[y]->r < v[y]->r) {
      return -1;
    } else if (u[y]->r > v[y]->r) {
      return 1;
    }
  }
  return 0;
}

SET* eset(SET* nodes, NODE* nd) {
  if (nd == NULL) return nodes;
  if (nd->v == dsize) return nodes;
  char c = nd->c;
  if (c == 128 || c == -128) {
    nodes->ns[nodes->num++] = nd; // add empty node to set
    if (nd->accept == 1) nodes->accept = 1;
    nd->v = dsize;
    nodes = eset(nodes, nd->n);
  } else if (c == 127) { // add branched nodes to set
    nodes->ns[nodes->num++] = nd;
    if (nd->accept == 1) nodes->accept = 1;
    nd->v = dsize;
    nodes = eset(nodes, nd->n);
    nodes = eset(nodes, nd->n1);
  }
  return nodes;
}

int build_dfa(NODE* nd) {
  printf("building dfa\n");
  /* create set */
  SET *startset = (SET*) malloc(sizeof(SET));
  startset->num = 1;

  /* add current (first) nfa node into set */
  startset->ns = (NODE**) malloc(sizeof(NODE*)*5);
  startset->ns[0] = nd;
  if (nd->accept == 1) startset->accept = 1;
  else startset->accept = 0;

  // create dfa node
  DNODE *dfanode = (DNODE*) malloc(sizeof(DNODE));
  dfanode->s = startset; // set of dfa node
  dfanode->at = 0;
  dfanode->label = dsize++;
  nd->v = 0;
  startset = eset(startset, nd->n); // grab ep-closure
  if (nd->c == 127) eset(startset, nd->n1);
  qsort(startset->ns, startset->num, sizeof(startset->ns[0]), valcmp); // sort states
  root = dfanode;
  printf("dfa started\n");
  
  /* create dfa with this starting node */
  to_dfa(dfanode);
  return 0;
}

DNODE* get_dnode(SET* s) {
  qsort(s->ns, s->num, sizeof(s->ns[0]), valcmp); // sort states

  // see if there exists a dfa node that already matches the set
  DNODE* t = root;
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
  newnode->label = dsize++;
  newnode->s = s;
  newnode->at = 0;
  if (last == -1) prev->l = newnode; // insert into tree
  else prev->r = newnode;
  return newnode;
}

DNODE* node2d(NODE* nd) {
  if (nd == NULL) return NULL;
  
  char c = nd->c; // get character of node  
  printf("node char: %c (%d)\n", c, c);

  if (c >= 32 && c <= 126) { // check if node is a character
    /* create set */
    SET *sset = (SET*) malloc(sizeof(SET));
    sset->num = 1;
    sset->ns = (nODE**) malloc(sizeof(NODE*) * 5);
    sset->ns = nd; // add nfa node into set
    sset->accept = nd->accept == 1 ? 1 : 0;
    eset(sset, nd->n); // get set of nodes
    if (nd->c == 127) eset(sset, nd->n1);
    int q;
    printf("set: ");
    for (q = 0; q < sset->num; q++) {
      printf("%d(%c) ", sset->ns[q]->r, sset->ns[q]->c);
    }
    printf("\n");

    /* get/create dfanode corresponding to set */
    DNODE* dn = get_dnode(sset);
    return dn;
  }
  return NULL;
}

int to_dfa(DNODE* lastnode) {
  
  if (lastnode->done == 1) return 0; // return if last node was completed
  SET *s = lastnode->s;
  int m = lastnode->at;

  printf("iterating\n");

  /* go through nodes of the set */
  for (m = 0; m < s->num; m++) { 
    printf("%d %d\n", lastnode->at, m);
    //if (lastnode->at > m) return 0;
    //lastnode->at = m;
    NODE* b = s->ns[m]->n;
    DNODE* x = node2d(bs->ns[m]->n);
    DNODE* y;
    if (bs->ns[m]->c == 127) y = node2d(bs->ns[m]->n1);
      
        visited++;
        lastnode->next[c-32] = dn; // link dfa node to transitions
      
        printf("dfa node %x to node %x\n", &lastnode, &dn);
      
        to_dfa(dn);
      }
    }
    //m = lastnode->at;
  }
  lastnode->done = 1;
  return 0;
}

int build_transition_table() {
  DNODE* p = root;
  int u;
  for (u = 0; u < 95; u++) {
    printf("%c ", u+32);
  }
  printf("\n");
  printf("label %d (%d)\n", p->label, p->s->accept);
  for (u = 0; u < 95; u++) {
    if (p->next[u] != NULL) {
      printf("%d ",p->next[u]->label);
    } else {
      printf("0 ");
    }
  }
  printf("\n");
  tree_table(p->l);
  tree_table(p->r);
  printf("\n");
  return 0;
}

int tree_table(DNODE* p) {
  if (p == NULL) return 0;
  int u;
  printf("label %d (%d)\n", p->label, p->s->accept);
  for (u = 0; u < 95; u++) {
    if (p->next[u] != NULL) {
      printf("%d ", p->next[u]->label);
    } else {
      printf("0 ");
    }
  }
  printf("\n");
  tree_table(p->l);
  tree_table(p->r);
  return 0;
}

int main(int argc, char *argv[]) {
  printf("args: %d\n", argc);
  NODE *st = (NODE*) malloc(sizeof(NODE));
  EL *begin = (EL*) malloc(sizeof(EL));
  st->c = (char) 128;
  st->accept = 1;
  st->r = nsize++;
  begin->start0 = st;
  begin->start1 = st;
  begin->cur = st;
  EL *re;
  if (argc >= 2) {
    re = to_nfa(begin, argv[1]);  
    build_dfa(re->start0);
    build_transition_table();
  }
  if (argc == 3) {
    int k = checkmatch(re->start0, argv[2]);
    if (k == 0) printf("match: false\n");
    else printf("match: true\n");
  }
  //traverse(re->start0);
}
