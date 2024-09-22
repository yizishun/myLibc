#include <malloc.h>
#include <linux/filter.h>
#include <linux/prctl.h>
#include <linux/seccomp.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h>

char* notes[16];
int   note_size[16];
void bark() __attribute__((noinline));

void init() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}

void menu() {
    puts("welcome to evil crop database.");
    puts("1. Add data");
    puts("2. Delete data");
    puts("3. Edit data");
    puts("4. Show data");
    puts("5. Exit");
    puts(">");
}

void add() {
    unsigned int index;
    unsigned int size;

    printf("Index: ");
    scanf("%u", &index);

    if (notes[index] != NULL) {
        printf("This page has been used.");
        return;
    }

    printf("Size: ");
    scanf("%u", &size);

    notes[index]     = malloc(size);
    note_size[index] = size;
    return;
}

void delete() {
    unsigned int index;

    printf("Index: ");
    scanf("%u", &index);

    if (notes[index] == NULL) {
        printf("Page not found.\n");
        return;
    }

    free(notes[index]);
    return;
}

void edit() {
    unsigned int index;

    printf("Index: ");
    scanf("%u", &index);

    if (notes[index] == NULL) {
        printf("Page not found.\n");
        return;
    }

    printf("Content: ");
    read(0, notes[index], note_size[index]);

    return;
}

void show() {
    unsigned int index;

    printf("Index: ");
    scanf("%u", &index);

    if (notes[index] == NULL) {
        printf("Page not found.\n");
        return;
    }

    puts(notes[index]);

    return;
}

void bark() {
    printf("/bin/sh");
}
int main(int argc, char* argv[]) {
    init();
    bark();

    while (1) {
        int choice;

        menu();
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                add();
                break;
            case 2:
                delete();
                break;
            case 3:
                edit();
                break;
            case 4:
                show();
                break;
            case 5:
                exit(0);
                break;
            default:
                puts("Wrong choice!");
        }
    }

    return 0;
}
