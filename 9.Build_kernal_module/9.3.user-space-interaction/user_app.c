#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define CDEV_PATH "/dev/m_device"

int fd, option;
char write_buf[1024];
char read_buf[1024];

int main()
{   
    size_t len = 1024;
    fd = open(CDEV_PATH, O_RDWR);
    if (fd < 0) {
        printf("Cannot open device file: %s...\n", CDEV_PATH);
        return -1;
    }

    do {
        fflush(stdin);
        scanf("%d ",&option); 
        fflush(stdin);
        switch (option) {
            case 1:
                memset(write_buf,len,0);

                printf("Enter the string to write into driver: ");
                fflush(stdin);
                scanf(" %[^\t\n]s", write_buf);
                printf("Data Writing %s\n", write_buf);
                write(fd, write_buf, strlen(write_buf)+1);
                break;

            case 2:
                read(fd, read_buf, 1024);                
                printf("Data: %s\n\n\n", read_buf);
                break;

            default:
                printf("Enter Valid option = %c\n",option);
                break;
        }
    } while(option < 2);
    
    close(fd);

    return 0;
}