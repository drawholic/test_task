#include "loop.h"
#include "parser.h"

void loop()
{
	struct Node* current_dir = malloc(sizeof(struct Node));
	current_dir->name = NULL; // or ""
	current_dir->children_count = 0;
	current_dir->parent = NULL;
	current_dir->children = NULL;

	struct Command* c = malloc(sizeof(struct Command));

	size_t buffersize = 256;

	char* buffer = malloc(buffersize);

	char* current_dir_str = get_path(current_dir);

	while (1) {
	    printf("%s>", current_dir_str);
	    ssize_t nread = getline(&buffer, &buffersize, stdin);
	    if (nread == -1) break;

	    parse(buffer, c);
	    handle_command(&current_dir, c);

	    free(current_dir_str);
	    current_dir_str = get_path(current_dir);

	    // print children names for debug
	    for (unsigned i = 0; i < current_dir->children_count; i++) {
	        printf("%s ", current_dir->children[i]->name);
	    }
	    printf("\n");
	}
	free(current_dir_str);


};