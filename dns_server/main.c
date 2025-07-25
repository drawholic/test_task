#include "server.h"
#include "config.h"
#include "string.h"

void startup(struct Server*, struct Config*);

int main(int argc, char const *argv[])
{
	struct Server s;
	struct Config c;

	if(argc > 1){
		memcpy(c.pathname, argv[1], strlen(argv[1]) + 1);

	}else{
		memcpy(c.pathname, "config", 7);
	};
 
	startup(&s, &c);

	server_loop(&s, &c);

	return 0;
}


void startup(struct Server* s, struct Config* c)
{
	read_config(c); 
	fill_server(s, c);
};