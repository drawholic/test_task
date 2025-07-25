#ifndef PARSER_H
#define PARSER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
enum CommandType{
	CD,
	MKDIR,
	TOUCH,
	FORMAT,
	LS,
	UNKNOWN
};

struct Command{
	enum CommandType type;
	char** arguments;
};

enum CommandType get_command_type(char*);

void parse(char*, struct Command*);

/**
 * Split input line into arguments: command [args]*
 */
char** split( const char*);

/**
 * Split input string of path into an array of node names:
 * /etc/etc/etc => /, etc, etc, etc
 */
char** split_path(char*);

#endif