#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include <string.h>

#define DEVICE "/sys/kernel/debug/eudyptula/jiffies"

int main(int argc, char **argv){
        int size, fd,  ret, read_buf3=33;
	unsigned int read_buf2;
        char ch, write_buf[100], read_buf[4];
	
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
			printf("Antes: 0x%x 0x%x\n", read_buf3, &read_buf3);
                        ret = read(fd, &read_buf3, size);
			printf("Depois: 0x%x 0x%x\n", read_buf3, &read_buf3);
                        printf("[+] Read's return :%d\n", ret);
                        //printf("[+] ID : [0x%x] %s\n", read_buf, read_buf);
                        break;
                default:
                        printf("Cmd not recognized\n");
        }
        close(fd);
        return 0;
}
