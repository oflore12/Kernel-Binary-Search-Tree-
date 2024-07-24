#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define __NR_mailbox_init 548
#define __NR_mailbox_create 549
#define __NR_mailbox_delete 550
#define __NR_mailbox_destroy 551
#define __NR_message_count 552
#define __NR_mailbox_send 553
#define __NR_mailbox_recv 554
#define __NR_mailbox_shutdown 555
#define __NR_message_length 556

long mailbox_init_syscall(void) {
    return syscall(__NR_mailbox_init);
}
long mailbox_create_syscall(unsigned long id) {
    return syscall(__NR_mailbox_create, id);
}
long mailbox_destroy_syscall(unsigned long id) {
    return syscall(__NR_mailbox_destroy, id);
}

long mailbox_delete_syscall(unsigned long id) {
    return syscall(__NR_mailbox_delete, id);
}
 long message_count_syscall(unsigned long id) {
    return syscall(__NR_message_count, id);
}
long mailbox_send_syscall(unsigned long id, const char *msg, unsigned long len) {
    return syscall(__NR_mailbox_send, id, msg, len);
}
long mailbox_recv_syscall(unsigned long id, const char *msg, unsigned long len) {
    return syscall(__NR_mailbox_recv, id, msg, len);
}
long message_length_syscall(unsigned long id) {
    return syscall(__NR_message_count, id);
}
long mailbox_shutdown_syscall(void) {
    return syscall(__NR_mailbox_destroy);
}
int main(int argc, char *argv[]) {
    long rv, rv1, rv2;
    const char *msg = "first msg";
    const char *msg1 = "second msg";
    const char *msg2 = "third msg";
    char *msg3 = malloc(1024);
    //testing mailbox setup
    rv = mailbox_init_syscall();
    if(rv < 0) {
        perror("mailbox_init syscall failed");
    }
    else {
        printf("mailbox_init syscall successfull\n");
    }
    //testing mailbox create with id 2,3,4
    rv = mailbox_create_syscall(2);
    rv1 = mailbox_create_syscall(3);
    rv2 = mailbox_create_syscall(4);
    if(rv < 0 || rv1 < 0 || rv2 < 0) {
        perror("mailbox_create syscall failed");
    }
    else {
        printf("mailbox_create syscall successfull\n");
    }
    //testing mailbox send msg to mailbox id 3
    rv = mailbox_send_syscall(3, msg2, sizeof(msg2));
    if(rv < 0) {
        perror("mailbox_send syscall failed");
    }
    else {
        printf("mailbox_send syscall successfull\n");
    }
    //testing mailbox count
    mailbox_send_syscall(3, msg, sizeof(msg));
    rv=message_count_syscall(3);
    if(rv < 0) {
        perror("mailbox_count syscall failed");
    }
    else if(rv==2) {
        printf("mailbox_count syscall successfull\n");
    }
    else {
        printf("mailbox_count syscall failed\n");
    }

    //testing mailbox recv
    rv = mailbox_recv_syscall(3, msg3, sizeof(msg3));
    if(rv < 0) {
        perror("mailbox_recv syscall failed");
    }
    else {
        printf("mailbox_recv syscall successfull:\n");
    }

    rv = mailbox_delete_syscall(3);
    if(rv < 0) {
        perror("mailbox_delete syscall failed");
    }
    else {
        printf("mailbox_delete syscall successfull\n");
    }
    mailbox_send_syscall(2, msg1, sizeof(msg1));
    rv = mailbox_destroy_syscall(2);

    if(rv < 0) {
        perror("mailbox_destroy syscall failed");
    }
    else {
        printf("mailbox_destroy syscall successfull\n");
    }
    
    rv = mailbox_shutdown_syscall();
    if(rv < 0) {
        perror("mailbox_shutdown syscall failed");
    }
    else {
        printf("mailbox_shutdown syscall successfull\n");
    }


    return 0;
}
