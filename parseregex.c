#include <stdlib.h>
#include <stdio.h>

typedef struct nfanode NODE;
typedef struct nfavertex VERTEX;

struct nfanode {
  int state;
  int accept;
  VERTEX* vertices[5];
};

struct nfavertex {
  NODE* prev;
  NODE* next;
  char letter;
};


int main(int argc, char *argv[]) {
  printf("%d\n",argc);
  if (argc == 2) {
    int i;
    int curState = 0;

    NODE* beginNode = NULL;
    NODE* curNode;
    char* regex = argv[1];

    for (i = 0; regex[i] != '\0'; i++) {
      if (regex[i] == '*') {
        printf("%c", regex[i]);
        
        NODE* ta = (NODE*) malloc(sizeof(NODE));
        VERTEX* vab = (VERTEX*) malloc(sizeof(VERTEX));
        ta->state = ++curState;
        ta->accept = 0;
        vab->prev = ta;
        vab->next = curNode->vertices[0]->prev;
        vab->letter = ' ';

        NODE* tc = (NODE*) malloc(sizeof(NODE));
        VERTEX* vbc = (VERTEX*) malloc(sizeof(VERTEX));
        ta->state = ++curState;
        ta->accept = 0;
        vbc->prev = curNode;
        vbc->next = tc;
        vbc->letter = ' ';

        VERTEX* vca = (VERTEX*) malloc(sizeof(VERTEX));
        vca->prev = tc;
        vca->next = ta;
        vca->letter = ' ';

        VERTEX* vbb = (VERTEX*) malloc(sizeof(VERTEX));
        vbb->prev = curNode;
        vbb->next = curNode->vertices[0]->prev;
        vbb->letter = curNode->vertices[0]->letter;

        ta->vertices[0] = vab;
        curNode->vertices[1] = vbc;
        curNode->vertices[2] = vbb;
        tc->vertices[0] = vca;

      } else if (regex[i] == '|') {
        printf("%c", regex[i]);
      } else {
        printf("%c", regex[i]);
        NODE* t1;
        if (curNode) {
          t1 = curNode;
          t1->accept = 0;
        } else {
          t1 = (NODE*) malloc(sizeof(NODE) + 50);
          t1->state = ++curState;
          t1->accept = 0;
        }
        NODE* t2 = (NODE*) malloc(sizeof(NODE) + 50);
        t2->state = ++curState;
        t2->accept = 1;
        curNode = t2;
        
        VERTEX* v = (VERTEX*) malloc(sizeof(VERTEX));
        v->prev = t1;
        v->next = t2;
        v->letter = regex[i];
        t1->vertices[0] = v;
        printf("[%d %d]", t1->state, t2->state);
      }
    }
    printf("\n");
  }
  return 0;
}
