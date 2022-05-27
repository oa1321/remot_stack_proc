#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_DATA 2048

void std_output_use();
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

void* create_stack(char *stack_name);
int stack_command(struct stack *s, char * data);
int print_stack(struct stack *s);
int free_stack(struct stack *s);
int top(struct stack *s, char *data);
int pop(struct stack *s, char *data);
int push(struct stack *s, struct node *new_node);
