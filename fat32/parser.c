#include "parser.h"

enum CommandType get_command_type(char* str){

	if(strcmp(str, "ls") == 0)
	{
		return LS;
	}else if(strcmp(str, "cd") == 0)
	{
		return CD;
	}else if(strcmp(str, "format") == 0)
	{
		return FORMAT;
	}else if(strcmp(str, "mkdir") == 0)
	{
		return MKDIR;
	}else if(strcmp(str, "touch") == 0)
	{

	}else{
		return UNKNOWN;
	};

};

char** split(const char* str) {
	if (!str) return NULL;

	size_t len = strlen(str);
	size_t spaces_count = 0;

	// Count spaces
	for (size_t i = 0; i < len; i++) {
		if (str[i] == ' ')
			spaces_count++;
	}

	// Allocate lexems array (+1 for last token, +1 for NULL)
	char** lexems = malloc((spaces_count + 2) * sizeof(char*));
	if (!lexems) return NULL;

	size_t token_idx = 0;
	const char* start = str;
	const char* end = strchr(start, ' ');

	while (end != NULL) {
		size_t tok_len = end - start;
		lexems[token_idx] = malloc(tok_len + 1);
		strncpy(lexems[token_idx], start, tok_len);
		lexems[token_idx][tok_len] = '\0';

		token_idx++;
		start = end + 1;
		end = strchr(start, ' ');
	}

	// Last token
	if (*start != '\0') {
		lexems[token_idx] = strdup(start);
		token_idx++;
	}

	lexems[token_idx] = NULL; // Null terminate array
	return lexems;
} 

void parse(char* s, struct Command* c)
{
	char** args = split(s);
	c->type = get_command_type(args[0]);
	c->arguments = args;

};