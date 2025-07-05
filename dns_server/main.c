#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include "Config.h"
#include "server.h"
int startup();

struct Config c;
struct Server s;

int main(int argc, char const *argv[])
{
	startup();

	server_loop();

	return 0;
}

int startup(){
	read_config(c);
};