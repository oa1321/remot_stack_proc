#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dyalloc.hpp"
#define MAX_DATA 2048
/*
|------------------------------------|
|======= stack implementation =======|
|------------------------------------|

struct of a node 
struct of a stack
a push function - push a string (max len is 1024) - return -1 if empty
a pop function - pop a string (max len is 1024) - return -1 if empty
a top function - return the top string (max len is 1024) - return -1 if empty
a dynmic_free_stack function - dynmic_free the stack nodes and set the stack to be empty
*/

struct node {
    //a node contains a string and a pointer to the next node 
  
    char *data;
    struct node *next;
};

struct stack {
    //a stack contains a pointer to the top node and size of the stack
    int size;
    char *stack_name;
    struct node *head;
};

int push(struct stack *s, struct node *new_node){
    //push a string to the stack top
    //return -1 if empty
    //return 0 if success
    if (new_node == NULL) {
        printf("ERROR: dynmic_alloc failed - PUSH OPERTION\n");
        return -1;
    }
    new_node->next = s->head;
    s->head = new_node;
    s->size++;
    return 0;
}

int pop(struct stack *s, char *data) {
    //pop a string from the stack top
    //return -1 if empty
    //return 0 if success
    if (s->size == 0) {
        printf("ERROR: STACK IS EMPTY - POP OPERTION\n");
        return -1;
    }
    struct node *temp = s->head;
    strcpy(data, temp->data);
    s->head = temp->next;
    dynmic_free(temp);
    s->size--;
    return 0;
}

int top(struct stack *s, char *data) {
    //return the top node string
    //return -1 if empty
    //return 0 if success
    if (s->size == 0) {
        printf("ERROR: STACK IS EMPTY - TOP OPERTION\n");
        return -1;
    }
    struct node *temp = s->head;
    strcpy(data, temp->data);
    printf("OUTPUT: %s\n", data);
    return 0;
}
int free_stack(struct stack *s) {
    //dynmic_free the stack nodes and set the stack to be empty
    //stack it self is not dynmic_freed because it can be reused or not on heap
    //need to dynmic_free it manually
    struct node *temp = s->head;
    while (temp != NULL) {
        struct node *next = temp->next;
        dynmic_free(temp->data);
        dynmic_free(temp);
        temp = next;
    }
    dynmic_free(s->stack_name);
    s->size = 0;
    s->head = NULL;
    return 0;
}
int print_stack(struct stack *s) {
    //print the stack
    struct node *temp = s->head;
    printf("%s CONTENT:\n", s->stack_name);
    printf(" _______________\n");
    printf("|   stack-top   |\n\n");
  
    while (temp != NULL) {
        printf("| %s |\n", temp->data);
        temp = temp->next;
    }
     printf(" _______________\n");
    printf("|   stack-end   |\n\n");
 
    return 0;
}
void* create_stack(char *stack_name){
    //create a stack
    struct stack *s = (struct stack *)dynmic_alloc(sizeof(struct stack));
    s->size = 0;
    s->stack_name = (char *)dynmic_alloc(sizeof(char) * strlen(stack_name));
    strcpy(s->stack_name, stack_name);
    s->head = NULL;
    return s;
}
/*
|------------------------------------|
|======= notes interface  =======    |
|------------------------------------|

1. shared memory check
*/
