#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include <string.h>

#define DEVICE "/sys/kernel/debug/eudyptula/foo"

int main(int argc, char **argv){
        int size, fd,  ret, i;
        unsigned char *buffer;
        char ch;

        /* open for read/write */
        fd = open(DEVICE, O_RDWR);
        if(fd == -1){
                printf("Error!\n");
                printf("%s not found!\n", DEVICE);
                exit(1);
        }

        printf("r = read from device\nw = write to device\nenter cmd: ");
        scanf("%c", &ch);

        printf("Size: ");
        scanf("%d", &size);

        buffer = malloc(size+1);
        buffer[size] = '\0';

        switch(ch){
                case 'w':
                        printf("[+] Data: ");
                        scanf(" %[^\n]",buffer);
                        ret = write(fd, buffer, size);
                        printf("[+] Write's return :%d\n", ret);
                        break;
                case 'r':
                        ret = read(fd, buffer, size);
                        printf("After : %s [0x", buffer);
                        for(i=0;i<size;i++)
                                printf("%x", buffer[i]);
                        printf("]\n");
                        printf("[+] Read's return :%d\n", ret);
                        break;
                default:
                        printf("Cmd not recognized\n");
        }
        close(fd);
        return 0;
}
