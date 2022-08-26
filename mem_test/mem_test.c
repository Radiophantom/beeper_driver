#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main( int argc, char **argv ){
	if( argc != 2 ) {
		printf("Must be only 1 argument - char_device name!\n");
		return 1;
	}

	int fd;
	char msg [100] = "Hello, my friend!\n";
	char *resp;
	resp = malloc(100);

	fd = open(argv[1], O_WRONLY);
	write(fd,msg,strlen(msg));
	close(fd);
	fd = open(argv[1], O_RDONLY);
	//lseek(fd,5,SEEK_SET);
	read(fd,resp,5);
	printf("%s\n", resp);
	read(fd,resp,5);
	printf("%s\n", resp);
	read(fd,resp,5);
	printf("%s\n", resp);
	close(fd);
	free(resp);

	return 0;
}
	

