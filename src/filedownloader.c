#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define PORT "/dev/ttyUSB0"

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <path_to_file>", argv[0]);
        return -1;
    }

    int fd = open(PORT, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        perror("Failed to open TTY device");
        return -1;
    }


    struct termios tty;
    if (tcgetattr(fd, &tty) != 0)
    {
        perror("tcgetattr");
        return -2;
    }

    cfmakeraw(&tty);

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 5;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);


    /* disable hardware flow control if defined */
#ifdef CRTSCTS
        tty.c_cflag &= ~CRTSCTS;
#endif


    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return 1;
    }

    size_t user_path_len = sizeof(argv[1]);
    char *command = malloc(100+user_path_len);


    // If you need to change the command which is run on the device, do it here
    sprintf(command, "dd if=%s of=/proc/self/fd/1 2>/dev/null\n", argv[1]);
    /* sprintf(command, "nandread -d %s \n", argv[1]); */
    write(fd, command, strlen(command));


    // You can control how the output file is written here
    char *filename = strrchr(argv[1], '/');
    if (filename)
        filename++;
    else
        filename = argv[1];


    printf("filename: %s\n", filename);
    FILE *userfile;
    userfile = fopen(filename, "wb");
    if (!userfile)
    {
        perror("Error while creating local file");
        return -2;
    }


    // If you are missing the first line of all transmits
    // comment out this next check
    // Most UART ttys echo back your command before running it
    // if its not the case on yours then it needs to be commented
    // out.
    int n = 1;
    char c = ' ';
    while (c != '\n' && n > 0)
        n = read(fd, &c, 1);
    // End of bit to comment out


    char buffer[256];
    while (1)
    {
        n = read(fd, buffer, sizeof(buffer));
        if (n > 0)
        {
            fwrite(buffer, 1, n, userfile);
            fflush(userfile);
        }
        else
            return 0;
    }
    fclose(userfile);
    close(fd);
    return 0;
}
