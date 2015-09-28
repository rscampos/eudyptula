#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include <string.h>

#define DEVICE "/sys/kernel/debug/eudyptula/id"

int main(int argc, char **argv){
        int size, fd,  ret;
        char ch, write_buf[100], read_buf[100];
	
	/* open for read/write */
        fd = open(DEVICE, O_RDWR); 
        if(fd == -1){
                printf("Error!\n");
                exit(1);
        }

        printf("r = read from device\nw = write to device\nenter cmd: ");
        scanf("%c", &ch);
        
	printf("Size: ");
        scanf("%d", &size);

        switch(ch){
                case 'w':
                        printf("[+] Data: ");
                        scanf(" %[^\n]",write_buf);
                        ret = write(fd, write_buf, size);
                        printf("[+] Write's return :%d\n", ret);
                        break;
                case 'r':
                        ret = read(fd, read_buf, size);
                        printf("[+] Read's return :%d\n", ret);
                        printf("[+] ID :%s\n",read_buf);
                        break;
                default:
                        printf("Cmd not recognized\n");
        }
        close(fd);
        return 0;
}
