#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct list_nodes { // Linked list struct
    int size; // Size of file
    char *path; // Path name of file
    struct list_nodes *link;
} list_nodes;

typedef struct temp { // Used to store sub directories
    char *path; // Path name of file
    struct temp *link;
} temp;

void new_node(struct list_nodes** list, int dataSize, char *path) { // Add another node to end of initial node
    struct list_nodes *prev = *list;
    struct list_nodes *node = (struct list_nodes*) malloc(sizeof(struct list_nodes)); // Allocate memory to node
    node->size = dataSize; // Insert file size
    node->path = malloc(strlen(path) + 1); // Allocate memory to fit path name
    strcpy(node->path, path); // Insert path name
    node->link = NULL;

    if (*list == NULL) { // If list is empty, make new node as list
        *list = node;
    }
    else {
        while (prev->link != NULL) { // Loop through till the last node
            prev = prev->link;
        }
        prev->link = node; // Change link of last node
    }
}

void subdir_node(struct temp** sdlist, char *path) { // Add nodes for sub directories
    struct temp *last = *sdlist;
    struct temp *new_node = (struct temp*) malloc(sizeof(struct temp)); // Allocate memory to node
    new_node->path = malloc(strlen(path) + 1); // Allocate memory to fit path name
    strcpy(new_node->path, path); // Insert path name
    new_node->link = NULL;

    if (*sdlist == NULL) { // If list is empty, make new node as head
        *sdlist = new_node;
    }
    else {
        while (last->link != NULL) { // Loop through till the last node
            last = last->link;
        }
        last->link = new_node; // Change link of last node
    }
}

struct list_nodes *sort(struct list_nodes *list) { // Sort the linked list by node
    int temp;
    char *temp2;
    struct list_nodes *first_node = list, *next_node = list->link; // Nodes to store data and next link

    while (next_node != NULL) {
        while (next_node != first_node) { // Loop through values
            if (next_node->size < first_node->size) { // Compares two data values and swaps data using temp var
                temp = first_node->size;
                temp2 = first_node->path;
                first_node->size = next_node->size; // Swap size
                first_node->path = next_node->path; // Swap path name
                next_node->size = temp;
                next_node->path = temp2;
            }
            first_node = first_node->link;
        }
        first_node = list; // Make sorted node equal back to original node
        next_node = next_node->link;
    }
    return first_node;
}

void traverse(char *pathName, temp *sdList, list_nodes **list) {
    DIR *dir = opendir(pathName); // Open directory
    struct dirent *dp;
    struct stat s;

    if (dir != NULL) {
        while ((dp = readdir(dir)) != NULL) { // Keep reading through directories until there is none left
            char path[256]; // Assumes longest pathname does not exceed 256 characters
            snprintf(path, sizeof(path), "%s/%s", pathName, dp->d_name); // Gets full path address

            if (dp->d_type == DT_DIR) { // If it is a directory
                if (strcmp(".", dp->d_name) != 0 && strcmp("..", dp->d_name) != 0) { // Ignoring current and parent directory
                    subdir_node(&sdList, path); // Add first sub directory to node
                }
            }
            else if (dp->d_type == DT_REG) { // Only considering regular files
                lstat(path, &s);
                int dataSize = s.st_size; // Gets size of file in bytes
                new_node(list, dataSize, path); // Adds new node to linked list
            }
        }
    }
    else {
        perror("");
        exit(EXIT_FAILURE); // Exit if no such directory exists
    }
    closedir(dir); // Close directory

    while (sdList != NULL) { // Non-recursive loop to read through sub directories in linked list until there is none left
        DIR *subdir = opendir(sdList->path); // Open subdirectory
        struct dirent *sdp;
        struct stat ss;

        while ((sdp = readdir(subdir)) != NULL) {
            char path[256]; // Assumes longest pathname does not exceed 256 characters
            snprintf(path, sizeof(path), "%s/%s", sdList->path, sdp->d_name); // Gets full path address

            if (sdp->d_type == DT_DIR) { // If it is a directory
                if (strcmp(".", sdp->d_name) != 0 && strcmp("..", sdp->d_name) != 0) { // Ignoring current and parent directory
                    subdir_node(&sdList, path); // Append new node with sub-subdirectories etc to linked list
                }
            }
            else if (sdp->d_type == DT_REG) { // Only considering regular files
                lstat(path, &ss);
                int dataSize = ss.st_size; // Gets size of file in bytes
                new_node(list, dataSize, path); // Adds new node to linked list
            }
        }
        sdList = sdList->link;
        closedir(subdir); // Close subdirectory
    }
}

int main(int argc, char *argv[]) {
    struct list_nodes *list = NULL; // Starting with empty linked list
    struct temp *sdlist = NULL; // Linked list to store subdirectories temporarily
    traverse(argv[1], sdlist, &list); // Traverse through files in directories and add data to linked list
    sort(list); // Sort data in linked list

    while (list != NULL) { // Prints sorted list
        printf("%d\t%s\n", list->size, list->path);
        list = list->link;
    }
    return 0;
}
