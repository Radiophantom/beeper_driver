#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

int main( int argc, char **argv ){
	if( argc != 2 ) {
		printf("Must be only 1 argument - char_device name!\n");
		return 1;
	}

	//int fd;
	//char msg [100] = "Hello, my friend!\n";
	//char *resp;
	//resp = malloc(100);

	//fd = open(argv[1], O_WRONLY);
	//write(fd,msg,strlen(msg));
	//close(fd);
	//fd = open(argv[1], O_RDONLY);
	////lseek(fd,5,SEEK_SET);
	//read(fd,resp,5);
	//printf("%s\n", resp);
	//read(fd,resp,5);
	//printf("%s\n", resp);
	//read(fd,resp,5);
	//printf("%s\n", resp);
	//close(fd);
	//free(resp);

	int fd;
	char msg [100] = "Hello, my friend!";
	char *mapbuf;
	char *resp;

	fd = open(argv[1], O_RDWR);
	if(fd<0){
		printf("Cannot open device!\n");
		return -1;
	}
	mapbuf = mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	strcpy(mapbuf,msg);
	resp = malloc(1000);
	strcpy(resp,mapbuf);
	printf("%s\n",resp);
	strcpy(resp,mapbuf+5);
	printf("%s\n",resp);
	strcpy(resp,mapbuf+10);
	printf("%s\n",resp);
	munmap(mapbuf,4096);
	free(resp);
	close(fd);

	return 0;
}
	

