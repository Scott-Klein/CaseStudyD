#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#define BUFFERSIZE 4096
#define COPYMODE 0644

void oops(char *, char *);

void createFile(int *fd, char *av[]);

int main(int ac, char *av[])
{
    struct stat sb1, sb2;
    int in_fd, out_fd, n_chars;
    char buf[BUFFERSIZE];

    if (ac != 3)
    {
        fprintf(stderr, "usage: %s source destination\n", *av);
        exit(1);
    }

    if ((in_fd = open(av[1], O_RDONLY)) == -1)
        oops("Cannot open ", av[1]);

    fstat(in_fd, &sb1);

    createFile(&out_fd, av);

    fstat(out_fd, &sb2);

    //if file1 inode and file2 inode are the same and on the same device
    if (sb1.st_ino == sb2.st_ino && sb1.st_dev == sb2.st_dev) // since files are frequently on the same device, compare inode first to short circuit the comparison for slight performance gain.
    {
        oops("Files are identical: ", av[2]);
    }

    fchmod(out_fd, sb1.st_mode); // Set the same permissions baby.

    while ((n_chars = read(in_fd, buf, BUFFERSIZE)) > 0)
        if (write(out_fd, buf, n_chars) != n_chars)
            oops("Write error to ", av[2]);
    if (n_chars == -1)
        oops("Read error from ", av[1]);

    if (close(in_fd) == -1 || close(out_fd) == -1)
        oops("Error closing files", "");
}

void createFile(int *fd, char *av[])
{
    if ((*fd = creat(av[2], COPYMODE)) == -1)
    {
        if (errno == EISDIR) // If we got a plane old directory.
        {
            printf("IS A DIRECTORY\n");
            char pathName[100];
            strcpy(pathName, av[2]); // get the new path
            strcat(pathName, av[1]); // make the new file feature the name of the original
            if ((*fd = creat(pathName, COPYMODE)) == -1) //acquire the file descriptor.
            {
                oops("Cannot creat", av[2]);
            }
        }
        else
        {
            oops("Cannot creat", av[2]);
        }
    }
}

void oops(char *s1, char *s2)
{

    fprintf(stderr, "Error: %s ", s1);
    perror(s2);
    exit(1);
}
