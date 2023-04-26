/*************************************
* Lab 1 Exercise 3
* Name: Nicholas Jimmy Alden & Nixon Widjaja
* Student No: A0244833A & A0236430N
* Lab Group: B13 & B06
*************************************/

#include "node.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Add in your implementation below to the respective functions
// Feel free to add any headers you deem fit (although you do not need to)


// Traverses list and returns the sum of the data values
// of every node in the list.
long sum_list(list *lst) {
    long sum = 0;
    node *curr = lst -> head;
    while (curr != NULL) {
        sum += curr -> data;
        curr = curr -> next;
    }
    return sum;
}

// Traverses list and returns the number of data values in the list.
int list_len(list *lst) {
    node *now = lst -> head;
    int ans = 0;
    while (now != NULL) {
        ans++;
        now = now -> next;
    }
    return ans;
}

// Inserts a new node with data value at index (counting from head
// starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) { 
    node *inp = (node*) malloc(sizeof(node));
    inp -> data = data;
    node *curr = lst -> head;
    if (index) {
        for (int i = 1; i < index; i++) {
            curr = curr -> next;
        }
        inp -> next = curr -> next;
        curr -> next = inp;
    } else {    
        inp -> next = curr;
        lst -> head = inp;
    }
}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.
void delete_node_at(list *lst, int index) {
    node *curr = lst -> head;
    for (int i = 1; i < index; i++) {
        curr = curr->next;
    }
    if (index) {
        node *del = curr -> next;
        curr -> next = del -> next;
        free(del);
    } else {    
        lst -> head = curr -> next;
        free(curr);
    }
}

// Search list by the given element.
// If element not present, return -1 else return the index. If lst is empty return -2.
//Printing of the index is already handled in ex2.c
int search_list(list *lst, int element) {
    node *now = lst -> head;
    if (now == NULL) {
        return -2;
    }
    int i = 0;
    while (now != NULL) {
        if (now -> data == element) {
            return i;
        }
        i++;
        now = now -> next;
    }
    return -1;
}

// Reverses the list with the last node becoming the first node.
void reverse_list(list *lst) {
    node *now = lst -> head, *prev = NULL;
    while (now != NULL) {
        node *temp = now -> next;
        now -> next = prev;
        prev = now;
        now = temp;
    }
    lst -> head = prev;
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
    node *now = lst -> head, *prev = NULL;
    lst -> head = NULL;
    while (now != NULL) {
        prev = now;
        now = now -> next;
        free(prev);
    }
}

// Traverses list and applies func on data values of
// all elements in the list.
void map(list *lst, int (*func)(int)) {
    node *now = lst -> head;
    while (now != NULL) {
        now -> data = (*func)(now -> data);
        now = now -> next;
    }
}
	

