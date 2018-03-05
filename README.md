# XV6 Custom shared memory and semaphores

My assignment from exercise 3 for the 'Operating Systems' lesson where the xv6 operating system is used to implement functionality of system calls for shared memory and semaphores. 


### Compile & run

Open the terminal in a linux environment and type the following commands in the 'xv6-public-ex3' folder:

```
make qemu
```

or the below command to run the emulator in current terminal:

```
make qemu-nox
```

to exit from **xv6** shell and return in your system shell just type **ctrl + 'a'** and then **'x'**

### Testing

You can try to run **s** workload or to create your own workloads, for example type the below command in xv6 shell
```
$ s
```

### Disable/Enable kernel messages

Open **param.h** file and set **SHMD** value to **0**        
