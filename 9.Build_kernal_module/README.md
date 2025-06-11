## Description

This section for overview of basic kernal module:

1. Set Up Development Environment

2. Write the first module code (init exit)

3. Device number, device file and charactor device

4. File operation

Interaction between a user-space application and a GPIO (General Purpose Input/Output) pin, specifically GPIO 27, through a custom device driver in the Linux kernel:

![block diagram](./9.2.file-operation/block_diagram.jpg)

## Cheat code command line in terminal

1. Check version kernal using currently: `uname -r`

2. Turn on message log in kernal: `sudo dmesg -w`

3. List device file: `ls /dev -l`

4. List kernal module: `lsmod`

5. Check system call when interact a kernal module: `strace <write to deivce>`