/*
Odalis Flores
Basic BST implementation
CMSC421 Project2
Spring24
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <sys/wait.h>
//#include <linux/list.h>
#include <ctype.h>
#include <stdbool.h>
unsigned long count = 0;


//x/////////structure for linked list/////////////////
typedef struct lnode{
    unsigned long length;
    struct lnode *next;
    char * message[1080];
} lnode;


//creating a new linked list node
struct lnode * createLNode(char *message){
    struct lnode *newNode = (struct lnode *)malloc(sizeof(struct lnode));
    if(sizeof(message) > 1080){
        *newNode->message = realloc(*newNode->message, sizeof(message));
    }
    *newNode->message= message;
    newNode->length = strlen(message);
    newNode->next = NULL;
}


//adding a node to the linked list
struct lnode * enqueue(struct lnode * head, char *message){
    if(head == NULL)
        return createLNode(message);
    else
        head->next = enqueue(head->next, message);
}


//removing the head
struct lnode * dequeue(struct lnode * head ){
    if (head == NULL){
        return NULL;//failure, nothing to remove
    }else{
        struct lnode *temp = head;
        head = head->next;
        free(temp);
        return head;//success
    }
    return NULL;
}


//peeking at the head
struct lnode * peek(struct lnode * head){
    if (head == NULL){
        return NULL;//failure, nothing to peek
    }else{
        printf("%s\n", *head->message);
        return head;//success
    }
    return NULL;
}


//print the messages in the list
void printList(struct lnode * head){
    if(head == NULL){
        printf("List is empty\n");
    }
    else{
        lnode *current = head;
        while(current != NULL){
            printf("%s, ", *current->message);
            current = current->next;
        }
    }
}
//checks if the list is empty
bool isEmpty(struct lnode * head){
    if(head == NULL)
        return true;
    return false;
}
//checks the number of items in the list
unsigned long items (struct lnode * head){
    if(head == NULL) return 0;
    else{
        unsigned long count = 0;
        lnode *current = head;
        while(current != NULL){
            count++;
            current = current->next;
        }
        return count;
    }
}


///////////structure for BST tree/////////////////
typedef struct tNode{
    unsigned long uniqueID;
    struct tNode *left;
    struct tNode *right;
    lnode *messages;
} tNode;


// create a new node for tree
struct tNode *createtNode(unsigned long uniqueID){
    struct tNode *tnode = (struct tNode *)malloc(sizeof(struct tNode));
    tnode->uniqueID = uniqueID;
    tnode->left = NULL;
    tnode->right = NULL;
    tnode->messages = createLNode("empty");
    return tnode;
}


//inserting a node in the tree
struct tNode *insert(struct tNode *node, unsigned long uniqueID){
    if (node == NULL) return (createtNode(uniqueID));
    else{
        if (uniqueID <= node->uniqueID) node->left = insert(node->left, uniqueID);
        else node->right = insert(node->right, uniqueID);
        return (node);
    }
}


// search for a node in the tree
struct tNode *search(struct tNode *root, unsigned long uniqueID){
    if (root == NULL || root->uniqueID == uniqueID) return root;
   
    if (root->uniqueID < uniqueID) return search(root->right, uniqueID);
   
    return search(root->left, uniqueID);
}


// print the tree in inorder traversal
void traverseInorder(struct tNode *root){
    if (root != NULL){
        traverseInorder(root->left);
        printf("%lu \n", root->uniqueID);
        traverseInorder(root->right);
    }
}


// delete a tNode from the tree
int delete(struct tNode *root, unsigned long uniqueID){
    struct tNode *parent = NULL;
    struct tNode *current = root;
    // search for the tNode
    while (current != NULL){
        if (current->uniqueID == uniqueID) {
             if (current->left == NULL && current->right == NULL){
                if (parent == NULL)
                    free(root);
                else if (parent->left == current)
                    free(parent->left);
                else
                    free(parent->right);
                return 1; //removed a leaf node successfully


            }else if (current->right == NULL){ //only has a left child
                if (parent == NULL)
                    root = current->left;
                else if (parent->left == current)
                    parent->left = current->left;
                else
                    parent->right = current->left;


            }else if (current->left == NULL){ //only has a right child
                if (parent == NULL)
                    root = current->right;
                else if (parent->left == current)
                    parent->left = current->right;
                else
                    parent->right = current->right;


            }else{
                struct tNode *tempParent = current; //node has 2 children, find the successor
                struct tNode *temp = current->right;
                while (temp->left != NULL){
                    tempParent = temp;
                    temp = temp->left;
                }
                if (tempParent->left == temp)
                    tempParent->left = temp->left;
                else
                    tempParent->right = temp->right;
                current->uniqueID = temp->uniqueID;
                free(temp);
            }
            return 1;//successfully removed node with 2 children


        }else if (current->uniqueID < uniqueID){ //move to the right
            parent = current;
            current = current->right;
        }
        else{ //move to the left
            parent = current;
            current = current->left;
        }
    }
    return 0; //something went wrong
}


//finds the min value in the tree by travversing to the left
struct tNode *findMin( struct tNode *root){
    if (root == NULL) return NULL;
    while (root->left != NULL){
        root = root->left;
    }
    return root;
}


//finds the max value in the tree by travversing to the right
struct tNode *findMax( struct tNode *root){
    if (root == NULL) return NULL;
    while (root->right != NULL){
        root = root->right;
    }
    return root;
}




int main(int argc, char **argv){
    tNode *root = NULL;
    root = insert(root, count++);
    insert(root, count++);
    insert(root, count++);
    insert(root, count++);
    insert(root, count++);
    insert(root, count++);
    insert(root, count++);
    delete (root, count++);
    printf("Inorder traversal with deletion of node 20 \n");
    traverseInorder(root);
    printf("Min: %lu\n", findMin(root)->uniqueID);
    printf("Max: %lu\n", findMax(root)->uniqueID);
}
