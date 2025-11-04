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

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    // NOTE: if I'm getting bad readings, the issue is likely here
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;   // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                       // disable break processing
    tty.c_lflag = 0;                              // no signaling chars, no echo
    tty.c_oflag = 0;                              // no remapping, no delays
    tty.c_cc[VMIN]  = 0;                          // non-blocking read
    tty.c_cc[VTIME] = 5;                          // 0.5 sec read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);       // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);              // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);            // no parity
    tty.c_cflag &= ~CSTOPB;                       // 1 stop bit

    #ifdef CRTSCTS
        tty.c_cflag &= ~CRTSCTS;    // disable hardware flow control
    #endif


    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return 1;
    }

    const char *msg = "cat /init.rc\n";
    write(fd, msg, strlen(msg));


    char buffer[256];
    while (1)
    {
        int n = read(fd, buffer, sizeof(buffer));
        if (n > 0)
        {
            fwrite(buffer, 1, n, stdout);
            fflush(stdout);
        }
        else
            return 0;
    }
    close(fd);
    return 0;
}
