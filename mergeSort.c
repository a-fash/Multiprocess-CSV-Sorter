#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
	void *data;
	char *line;
	struct Node *next;
}Node;

Node * sortedMerge(Node *a, Node *b, int (*comparatorFnPtr)(void*, void*));
void frontBackSplit(Node *source, Node **front, Node **head);
int comparator_INT(void *data0, void *data1);
int comparator_STR(void *data0, void *data);

void mergeSort(struct Node ** headRef, int (*comparatorFnPtr)(void*, void*))
{
	Node *head = *headRef;
	Node *a;
	Node *b;
	
	if((head == NULL) || (head->next == NULL))
	{
		return;
	}

	frontBackSplit(head, &a, &b);
	
	mergeSort(&a, comparatorFnPtr);
	mergeSort(&b, comparatorFnPtr);

	*headRef = sortedMerge(a, b, comparatorFnPtr);
}

Node * sortedMerge(Node *a, Node *b, int (comparatorFnPtr)(void*, void*))
{
	Node *result = NULL;
	
	if(a == NULL)
		return(b);

	else if(b == NULL)
		return(a);

	int outcome = comparatorFnPtr(a->data, b->data);
	
	if(outcome >= 0)
	{
		result = a;
		result->next = sortedMerge(a->next, b, comparatorFnPtr);
	}
	else
	{
		result = b;
		result->next = sortedMerge(a, b->next, comparatorFnPtr);
	}

	return result;
}

void frontBackSplit(Node *source, Node **front, Node **back)
{
	Node *fast;
	Node *slow;
	slow = source;
	fast = source->next;

	while(fast != NULL)
	{
		fast = fast->next;
		if(fast != NULL)
		{
			slow = slow->next;
			fast = fast->next;
		}
	}

	*front = source;
	*back = slow->next;
	slow->next = NULL;
}

void push(Node **headRef, void *data, size_t data_size, char *line, size_t line_size)
{
	Node *newNode = (Node *)malloc(sizeof(Node));
	
	newNode->data = malloc(data_size);
	newNode->line = malloc(line_size);
	newNode->next = (*headRef);

	int i;
	for(i=0; i<data_size; i++)
		*(char *)(newNode->data +i) = *(char *)(data + i);

	newNode->line = strcpy(newNode->line, line);
		
	(*headRef) = newNode;
}

void printList(Node *head)
{
	Node *ptr = NULL;
	
	while(head != NULL)
	{
		printf("%s\n", head->line);
		ptr = head->next;
		head = ptr;
		
	}
}

void freeList(Node *head)
{
	Node *ptr = head;	
	while(ptr !=NULL)
	{
		ptr = head->next;
		free(head);
		head = ptr;
	}
}


int comparator_INT(void *data0, void *data1)
{
	return *((int*)data1) - *((int*)data0);
}

int comparator_STR(void *data0, void *data1)
{
	return strcmp((char *)data1, (char *)data0);
}
