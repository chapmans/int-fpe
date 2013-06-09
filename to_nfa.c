#include "nfa.h"

static int nsize = 1;
ALPHA* alphabet;

NODE* build_nfa(char* regex) {
  alphabet = (ALPHA*) malloc(sizeof(ALPHA));
  
  NODE *start = (NODE*) malloc(sizeof(NODE));
  start->c = EP;
  start->accept = 1;
  start->id = 0;
  
  NFA *begin = (NFA*) malloc(sizeof(NFA));
  begin->start_c = start;
  begin->start_s = start;
  begin->cur = start;

  to_nfa(begin, regex);
  //printf("root: %d\n", begin->start_c->id); 
  return begin->start_c;
}

/* to_nfa
 * Converts a regular expression into an NFA.
 */
NFA* to_nfa(NFA *begin, char *regex) {
  
  /* creates a node for the NFA */
  NODE* start_c = begin->start_c; // concat start: beginning
  NODE* start_s = begin->start_s; // star start: beginning
  NODE* cur = begin->cur;         // current node: beginning
  int curaccept = 0;
  int acceptsize = 5;
  NODE** accepts = (NODE**) calloc(acceptsize,sizeof(NODE));
  accepts[curaccept] = cur;     // add current node to array of accepting nodes

  NFA* re = (NFA*) malloc(sizeof(NFA));
  int temp;

  char* i; // go through each character of the regex
  for (i = regex; *i; i++) {

    /* allocate nodes to be filled in */
    NODE* m = (NODE*) malloc(sizeof(NODE));
    m->v = -1;
    m->id = nsize++;
    
    switch(*i) {
      case '\\': // escape sequence
        i++;
        if (*i == '\\') m->c = '\\';
        else if (*i == '*') m->c = '*';
        else if (*i == '+') m->c = '+';
        else if (*i == '|') m->c = '|';
        else if (*i == '(') m->c = '(';
        else if (*i == ')') m->c = ')';
        else if (*i == 'd') m->c = DIGITS; // allow decimals
        else break;

        m->accept = 1;
        accepts[curaccept] = m;
        cur->n = m;
        cur->accept = 0;
        
        cur = m;
        start_s = cur;
        break;
      case '*':
        temp = m->id;
        m->c = start_s->c;    // set node to be contents of star start
        m->n = start_s->n;
        m->n1 = start_s->n1;
        m->id = start_s->id;
        m->accept = start_s->accept;

        start_s->c = BRANCH;
        start_s->n = start_s;
        start_s->n1 = m;
        start_s->id = temp;
        start_s->accept = 1;
        accepts[curaccept] = m;
      
        if (start_s == cur) cur = m;
        cur->n = start_s; // loop back current to m
        cur->accept = 0;
        
        //printf("*: %c(%d) to %c(%d)\n", m->n->c,m->n->c,cur->c, cur->c);
        cur = start_s;
        break;
      case '+':
        break;
      case '|':
        temp = m->id;
        m->c = start_c->c; 
        m->n = start_c->n;
        m->n1 = start_c->n1;
        m->id = start_c->id;
        m->accept = start_c->accept;

        NODE* m1 = (NODE*) malloc(sizeof(NODE));
        m1->c = EP;
        m1->id = nsize++;
        //printf("m1->id %d\n", m1->id);
        m1->v = -1;
        m1->accept = 1;

        start_c->c = BRANCH; // split node into start/cur and post-cur
        start_c->n = m1;
        start_c->n1 = m;
        start_c->id = temp;
        start_c->accept = 0;

        /*if (begin->start_c->id == m->id) {
          begin->start_c = start_c;
        }*/

        cur->accept = 1;
        accepts[curaccept++] = cur;
        accepts[curaccept] = m1;

        //printf("or");
        NODE* qq = m;
        /*printf("\t %d(%d)",qq->id, qq->c);
        while ((qq = qq->n) != NULL) {
          printf("\t %d(%d)",qq->id, qq->c);
        }
        printf("\n");
        qq = m;
        while ((qq = qq->n1) != NULL) {
          printf("\t %d(%d)", qq->id, qq->c);
          NODE* tt = qq;
          printf("\n\t");
          while((tt = tt->n) != NULL) {
            printf("\t %d(%d)", qq->id, qq->c);
          }
        }*/
        //printf("\n");

        cur = m1;
        break;
      case '(':
        m->c = EP;
        m->accept = 1;
        
        cur->n = m;
        cur->accept = 0;
       
        re->start_c = m;
        re->start_s = m;
        re->cur = m;
        re = to_nfa(re, i+1);

        start_c = re->start_c;
        start_s = re->start_s;
        cur = re->cur;
        accepts[curaccept] = cur;
        i = re->back;
        
        break;
      case ')':
        m->c = EP;
        m->accept = 1;
        int j;
        for (j = 0; j <= curaccept; j++) {
          accepts[j]->accept = 0;
          accepts[j]->n = m;
        }
        //free(begin);
        // curaccept = 0;
        re->start_c = begin->start_c;
        re->start_s = begin->start_c;
        re->cur = m;
        re->back = i;
        return re;
        break;
      default: // any letter
        m->c = *i;
        m->accept = 1;
        accepts[curaccept] = m;

        cur->n = m; // this node: to next node
        cur->accept = 0;

        //printf("char: %c\n", m->c);
        cur = m; // point to next node
        start_s = cur;
    }
    if (!(m->c == EP || m->c == BRANCH)) {
      alphabet->in[m->c - 32] = 1; // add to array of used characters
    }
    //printf("added %c, %d \n", cur->c, cur->accept);
    if (curaccept >= acceptsize) {
      acceptsize += 5;
      accepts = (NODE**) realloc(accepts, acceptsize*sizeof(NODE));
    }
  }

  NODE* m = (NODE*) malloc(sizeof(NODE));
  m->c = EP;
  m->id = nsize++;
  m->v = -1;
  m->accept = 1;
  int j;
  for (j = 0; j <= curaccept; j++) {
    accepts[j]->accept = 0;
    accepts[j]->n = m;
  }
  free(accepts);
  free(re);
  return begin;
}

/* traverse
 * Traverses the NFA.
 */
int traverse(NODE *nd) {
  int visited[nsize];
  int k;
  for (k = 0; k < nsize; k++) {
    visited[k] = 0;
  }
  int level = 0;
   if (nd->accept == 1) {
     printf("[");
   }
   
   printf("%c(%d)/%d\n", nd->c, nd->c, nd->id);
   visited[nd->id] = 1;

   if (nd->accept == 1) {
     printf("]");
   }
   rtraverse(nd->n, level+1, visited);
  if (nd->c == BRANCH) rtraverse(nd->n1, level+1, visited);
  return 0;

}

int rtraverse(NODE *nd, int level, int* visited) {
  if (nd == NULL) return 0;
  if (visited[nd->id] == 1) return 0;
  int w;
  for (w = 0; w < level; w++) {
    printf("\t");
  }
  if (nd->accept == 1){
    printf("*");
  }

  printf("%c(%d)/%d\n", nd->c, nd->c, nd->id);
  visited[nd->id] = 1;

  rtraverse(nd->n, level+1, visited);
  if (nd->c == BRANCH) rtraverse(nd->n1, level+1, visited);
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
  if (nd->c == EP) {
    m = checkmatch(nd->n, l);
  } else if (nd->c == BRANCH) {
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


/* build_alphabet
 * Builds the alphabet based on the characters used.
 */
ALPHA* build_alphabet() {
  
  int count = 0;
  int i;
  for (i = 0; i < 95; i++) {
    if (alphabet->in[i] == 1) {
      alphabet->out[count] = i+32;
      alphabet->in[i] = count;
      count++;
    }
  }
  alphabet->alen = count;

  return alphabet;
}
