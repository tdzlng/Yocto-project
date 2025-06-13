#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define CDEV_PATH "/dev/m_device"

char write_buf[1024];

int main(int argc, char**arv)
{   
    size_t len = 1024;
    if(argc>1){
        memcpy(write_buf,arv[1],len);
    } else {
        return -1;
    }

    int fd = open(CDEV_PATH, O_RDWR);
    if (fd < 0) {
        printf("Cannot open device file: %s...\n", CDEV_PATH);
        return -1;
    }


    printf("Data Writing %s\n", write_buf);
    write(fd, write_buf, strlen(write_buf)+1);
    
    close(fd);

    return 0;
}