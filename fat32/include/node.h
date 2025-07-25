#ifndef NODE_H
#define NODE_H

#include <stdlib.h>
#include <stddef.h>

#include "parser.h"

struct Node {
    char* name;
    struct Node** children;
    struct Node* parent;
    size_t children_count;
};

// Node management
struct Node* create_node(const char* name, struct Node* parent);
struct Node* get_root(struct Node*);

// Path handling
size_t get_depth(struct Node* n);
char* get_path(struct Node* n);
struct Node* check_path_valid(struct Node* root, char* path);

// Command execution
void handle_command(struct Node** current_dir, struct Command* command);
void handle_mkdir(struct Node* root, char** args);

void handle_cd(struct Node**, char**);

#endif
