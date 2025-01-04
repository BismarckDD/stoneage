#include "link.h"
#include "buf.h"

BOOL LinkAppendTail(Node **top, Node *add) {
  Node *c;
  Node *next;
  if (*top == NULL) {
    *top = allocateMemory(sizeof(Node));
    if (*top == NULL)
      return FALSE;
    (*top)->next = NULL;
    (*top)->size = add->size;
    (*top)->val = add->val;
    return TRUE;
  }
  for (c = *top; c->next; c = c->next);
  next = allocateMemory(sizeof(Node));
  if (next == NULL)
    return FALSE;
  c->next = next;
  next->next = NULL;
  next->val = add->val;
  next->size = add->size;
  return TRUE;
}

BOOL LinkAppendHead(Node **nowtop, Node *add) {
  Node *newtop;
  if (*nowtop == NULL) {
    *nowtop = allocateMemory(sizeof(Node));
    if (*nowtop == NULL)
      return FALSE;
    (*nowtop)->next = NULL;
    (*nowtop)->size = add->size;
    (*nowtop)->val = add->val;
    return TRUE;
  }

  newtop = allocateMemory(sizeof(Node));
  newtop->next = *nowtop;
  newtop->val = add->val;
  newtop->size = add->size;
  *nowtop = newtop;
  return TRUE;
}

BOOL LinkRemoveHead(Node **top, Node *ret) {
  Node *newtop;
  if (*top == NULL)
    return FALSE;
  ret->val = (*top)->val;
  ret->size = (*top)->size;
  newtop = (*top)->next;
  freeMemory(*top);
  *top = newtop;
  return TRUE;
}

BOOL LinkeRemoveTail(Node **top, Node *ret) {
  if (*top == NULL)
    return FALSE;
  Node *curr = *top;
  Node *next = c->next;
  while (1) {
    if (next->next == NULL)
      break;
    curr = curr->next;
    next = next->next;
  }
  curr->next = NULL;
  ret->val = next->val;
  ret->size = next->size;
  freeMemory(next);
  return TRUE;
}
