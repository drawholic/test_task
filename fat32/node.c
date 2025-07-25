#include "node.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct Node* create_node(const char* name, struct Node* parent) {
    struct Node* node = malloc(sizeof(struct Node));
    node->name = strdup(name);
    node->parent = parent;
    node->children = NULL;
    node->children_count = 0;
    return node;
}

size_t get_depth(struct Node* n) {
    size_t depth = 0;
    while (n->parent != NULL) {
        depth++;
        n = n->parent;
    }
    return depth;
}
char* get_path(struct Node* n) {
    if (!n) return NULL;
    if (!n->parent) return strdup("/"); // root

    size_t depth = get_depth(n);
    char** segments = malloc(depth * sizeof(char*));

    struct Node* tmp = n;
    for (int i = depth - 1; i >= 0; i--) {
        segments[i] = tmp->name;
        tmp = tmp->parent;
    }

    size_t total = 1; // for initial '/'
    for (size_t i = 0; i < depth; i++) {
        total += strlen(segments[i]) + 1; // '/' + segment
    }

    char* path = malloc(total);
    path[0] = '\0';
    strcat(path, "/");

    for (size_t i = 0; i < depth; i++) {
        strcat(path, segments[i]);
        if (i != depth - 1) strcat(path, "/");
    }

    free(segments);
    return path;
}


void handle_command(struct Node** current_dir, struct Command* command) {
    switch (command->type) {
        case MKDIR:
            handle_mkdir(*current_dir, command->arguments);
            break;
        case LS:
        case TOUCH:
        case FORMAT:
        case CD:
            handle_cd(current_dir, command->arguments);
        default:
            break;
    }
}

void handle_mkdir(struct Node* current_dir, char** args) {
    for (size_t i = 1; args[i] != NULL; i++) {
        char* new_dir_name = strdup(args[i]);

        current_dir->children = realloc(
            current_dir->children, 
            (current_dir->children_count + 1) * sizeof(struct Node*));
        current_dir->children[current_dir->children_count++] = create_node(new_dir_name, current_dir);
    }
}

struct Node* get_root(struct Node* n) {
    while (n->parent != NULL) {
        n = n->parent;
    }
    return n;
}

struct Node* check_path_valid(struct Node* root, char* path) {
    if (!path || path[0] != '/') return NULL;

    char* temp = strdup(path);
    char* token = strtok(temp, "/");
    struct Node* cur = root;

    while (token) {
        struct Node* next = NULL;
        for (size_t i = 0; i < cur->children_count; i++) {
            if (cur->children[i] && strcmp(cur->children[i]->name, token) == 0) {
                next = cur->children[i];
                break;
            }
        }
        if (!next) {
            free(temp);
            return NULL;
        }
        cur = next;
        token = strtok(NULL, "/");
    }

    free(temp);
    return cur;
}
    

void handle_cd(struct Node** current_dir, char** args){
    struct Node *root = get_root(*current_dir);
    struct Node *new_dir = check_path_valid(root, args[1]);
    if(new_dir != NULL)
    {
        *current_dir = new_dir;
    };
};