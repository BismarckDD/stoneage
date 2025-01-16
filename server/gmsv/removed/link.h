#ifndef __LINK_H__
#define __LINK_H__

typedef struct tagNode
{
  struct tagNode* next;
  char* val;
  int size;
} Node;

BOOL LinkAppendHead(Node** top, Node* add);
BOOL LinkAppendTail(Node** top, Node* add);
BOOL LinkRemoveHead(Node** top, Node* ret);
BOOL LinkRemoveTail(Node** top, Node* ret);
#endif
