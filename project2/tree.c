/*
project: 02
author: Odalis R. Flores
email: oflores4@umbc.edu
student id: Campus ID	GC12136
description: a kernel bst mailbox system
*/
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/errno.h>

struct lnode{
    unsigned long length;
    struct lnode *next;
    char * message[1080];
};

struct tNode{
    unsigned long uniqueID;
    struct tNode *left;
    struct tNode *right;
    struct lnode *messages;
};

//creating a new linked list node
struct lnode * createLNode(char *message){
    struct lnode *newNode = kzalloc(sizeof(struct lnode), GFP_KERNEL);
    if(sizeof(message) > 1080){
        *newNode->message = krealloc(*newNode->message, sizeof(message), GFP_KERNEL);
    }
    //strcpy(*newNode->message, message);
    *newNode->message= message;
    newNode->length = strlen(message);
    newNode->next = NULL;
    if(newNode == NULL) return NULL;//failure
    return newNode;//success
}

//adding a node to the linked list
struct lnode * enqueue(struct lnode * head, char *message){
	if(head == NULL)
		return createLNode(message);
	else if(head->next == NULL){
		head->next = enqueue(head->next, message);
    }
    else{
        struct lnode *curr = head;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = createLNode(message);
    }
    return head;
}

//removing the head
struct lnode * dequeue(struct lnode * head ){
    struct lnode *temp;
    if (head == NULL){
        return NULL;//failure, nothing to remove
    }else{
        if(head->next == NULL){
            kfree(head);
            return NULL;//success
        }
        temp = head;
        head = head->next;
        kfree(temp);
        return head;//success
    }
    return NULL;
}

//checks if the list is empty
bool isEmpty(struct lnode * head){
    if(head == NULL)
        return true;
    return false;
}

// create a new node for tree
struct tNode *createtNode(unsigned long uniqueID){
    struct tNode *tnode = kzalloc(sizeof(struct tNode), GFP_KERNEL);
    tnode->uniqueID = uniqueID;
    tnode->left = NULL;
    tnode->right = NULL;
    tnode->messages = NULL;
    return tnode;
}

//inserting a node in the tree
struct tNode *insert(struct tNode *node, unsigned long uniqueID){
    if (node == NULL){
        return createtNode(uniqueID);
    }
    else if(uniqueID > node->uniqueID){
        node->right = insert(node->right, uniqueID);
    }
    else if(uniqueID < node->uniqueID){
        node->left = insert(node->left, uniqueID);
    }
    else{
        return node;
    }
    return node;
}


// search for a node in the tree
static struct tNode *search(struct tNode *root, unsigned long uniqueID){
    if (root == NULL) return NULL;
    else if (root->uniqueID == uniqueID) return root;
    else{
        if (root->uniqueID < uniqueID) return search(root->right, uniqueID);
        else return search(root->left, uniqueID);
    }
    return NULL;
}
//finds the min value in the tree by travversing to the left
struct tNode* minTNode(struct tNode*root){
    struct tNode *curr = root;
    while (curr->left != NULL){
        curr = curr->left;
    }
    return curr;
}

//deletes a node from the tree
struct tNode * delete(struct tNode *root, unsigned long uniqueID){
    if(root == NULL) return NULL;
    else if(uniqueID < root->uniqueID){
        root->left = delete(root->left, uniqueID);
    }
    else if(uniqueID > root->uniqueID){
        root->right = delete(root->right, uniqueID);
    }
    else{
        if(root->left == NULL && root->right == NULL){
            kfree(root);
            return NULL;
        }
        else if(root->left == NULL || root->right == NULL){
            struct tNode *temp;
            if(root->left == NULL) temp = root->right;
            else temp = root->left;
            kfree(root);
            return temp;
        }
        else{
            struct tNode * temp = minTNode(root->right);
            root->uniqueID = temp->uniqueID;
            root->right = delete(root->right, temp->uniqueID);
        }
    }
    return NULL;//something went wrong
}
//checks the number of items in the list
long items (struct lnode * head){
    struct lnode *curr;
    unsigned long items;
    items = 0;
    curr= head;
    if(head == NULL) return 0;
    else{
        while(curr != NULL){
            items++;
            curr = curr->next;
        }
        return items;
    }
    return 0;
}

void free_tree(struct tNode *node){
    if(node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);

    while(node->messages != NULL){
        node->messages = dequeue(node->messages);
    }
    kfree(node->messages);
    kfree(node);
}


//peeking at the head
struct lnode * peek(struct lnode * head){
	if (head == NULL){
		return NULL;//failure, nothing to peek
	}else{
		return head;//success
	}
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////////////////////

static struct tNode * root;
//Initializes the queue system, setting up the initial state of the BST.
//You may initialize the BST by adding the root node. Return 0 on success.
SYSCALL_DEFINE0(mailbox_init){
    // Implementation code.
    root=NULL;
    if(root==NULL) return 0;
    return -1;
}


//Creates a new queue with the given id if it does not already exist (no duplicates are allowed).
//Returns 0 on success or an appropriate error on failure.
//If a negative id or an id equal to or greater than (264 - 1) is passed,
//this is considered an invalid ID and an appropriate error shall be returned.
SYSCALL_DEFINE1(mailbox_create, unsigned long, id){
    struct tNode *temp;
    temp = search(root, id);

    if(id<0 || id>18446744073709551615){
        return -EINVAL;//error
    }
    
    if(temp == NULL){
        root=insert(root, id);
        return 0;
    }
    return -ENODEV;//error
}


// Deletes the queue identified by id if it exists. If the queue has any messages stored in it,
//they should be deleted. Returns 0 on success or an appropriate error code on failure.
SYSCALL_DEFINE1(mailbox_destroy, unsigned long, id){
    // Implementation code.
    struct tNode *temp;
    temp = search(root, id);

    if (temp == NULL) return -ENOENT; //queue does not exist
    if(temp->messages == NULL) return -ESRCH; //queue is empty
    while(temp->messages != NULL) {
            temp->messages = dequeue(temp->messages);
        }

    if (isEmpty(temp->messages) == true) {
        delete(root, id);
        if(search(root, id) == NULL) return 0;//success
        else return -1;//error
        
    }
    else{
        printk("Error: Queue %ld has messages\n", id);
        return -1;
    }

    return -1;//error
}

//Deletes the oldest added message in the mailbox identified by id if it exists.
//This is a FIFO data structure - hence why we are deleting the oldest message. 
//HINT: Keep in mind all the things you have to do for this one - it's not just a 
//matter of just deleting a node in the queue. What other data within your mailbox 
//data structure (not the BST data structure, just one single mailbox) gets affected 
//by the deletion other than just the message node in the FIFO queue? An appropriate 
//return code is issued if the queue is empty or does not exist.

SYSCALL_DEFINE1(mailbox_delete, unsigned long, id){
    // Implementation code.
    struct tNode *tTemp;
    tTemp = search(root, id);
    if (tTemp == NULL){
        printk("Error: Queue %ld does not exist\n", id);
        return -ENOENT; //error id doesnt exist
    }
    else{
        if(tTemp->messages == NULL){
            printk("Error: Queue %ld does not have any messages\n", id);
            return -ESRCH; //error no messages
        }
        tTemp->messages = dequeue(tTemp->messages);
        return 0;//success
    }
    return -1;//error
}

//Returns the number of messages in the queue to the system log identified by id if it exists. Returns an appropriate error code on failure.
SYSCALL_DEFINE1(message_count, unsigned long, id){
    // Implementation code.
    struct tNode *temp;
    struct lnode *temp2;
    temp = search(root, id);
    if (temp == NULL) {
        printk("Error: Queue %ld does not exist\n", id);
        return -ENOENT; //queue does not exist
    }

    //get head of queue
    temp2 = temp->messages;
    if (temp == NULL){
        printk("Error: Queue %ld does not have any messages\n", id);
        return -ESRCH;
    }else{
        return items(temp2);
    }
    return -1;//error
}

// Sends a new message to the queue identified by id if it exists. The message shall be read 
//from the user-space pointer msg and shall be len bytes long. Returns 0 on success or an 
//appropriate error code on failure.
SYSCALL_DEFINE3(mailbox_send, unsigned long, id, const unsigned char __user *, msg, long, len){
    struct tNode *temp;
    char *message;
    int check_msg= access_ok(msg, len);
    message = kzalloc(sizeof(msg), GFP_KERNEL);
    
    if(check_msg != 1){
        printk("Error: Message not accessible\n");
        return -EFAULT;
    }
    //check for the queue in tree
    temp = search(root, id);
    if (temp == NULL) return -ENOENT; //queue does not exist
    
    //copies the message from user space to kernel space
    if (copy_from_user(message, msg, sizeof(msg)) != 0) {
        printk("Error: Message not copied\n");
        return -EFAULT;
    }

    temp->messages = enqueue(temp->messages, message);
    if(temp->messages != NULL) {
        kfree(message);
        return 0; //success
    }
    return -1; //error
}

//Reads the first message that is in the mailbox identified by id if it exists, 
//storing either the entire length of the message or len bytes to the user-space pointer msg, 
//whichever is less. The entire message is then removed from the mailbox (even if len was less 
//than the total length of the message). Returns the number of bytes copied to the user space
//pointer on success or an appropriate error code on failure.
SYSCALL_DEFINE3(mailbox_recv, unsigned long, id, unsigned char __user *, msg, long, len){
    struct tNode *temp;
    long bytes_copies = -1;
    int check_msg= access_ok(msg, len);
    if(check_msg != 1){
        printk("Error: Message not accessible\n");
        return -EFAULT;
    }
    //check for the queue in tree
    temp = search(root, id);
    if (temp == NULL) return -ENOENT; //queue does not exist
    
    //check if there is a queue
    if (temp->messages == NULL) return -ESRCH; //queue is empty
    
    //copies the message from kernel space to user space
    bytes_copies = copy_to_user(msg,peek(temp->messages)->message, sizeof(peek(temp->messages)->message));
    
    if ( bytes_copies != 0) {
        printk("Error: Message not copied, bytes not copied %ld\n", bytes_copies);
        return -EFAULT;
    }
    else{
        temp->messages = dequeue(temp->messages);
        return  bytes_copies; //success
    }

    return bytes_copies; //error
}

SYSCALL_DEFINE1(message_length, unsigned long, id){
    struct tNode *temp;
    struct lnode *temp2;
    temp = search(root, id);
    if (temp == NULL) return -ENOENT; //queue does not exist
    temp2 = peek(temp->messages);
    if (temp2 == NULL) return -ESRCH; //queue is empty
    return temp2->length;
}

SYSCALL_DEFINE0(mailbox_shutdown){
    struct tNode * curr;
    long id;
    curr=root;
    id = curr->uniqueID;
    free_tree(root);
    if(search(root, id) == NULL) return 0;
    return -1;
}