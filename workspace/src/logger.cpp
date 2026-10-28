#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define FIFO "/tmp/FIFO_LOGGER"

int main(int argc, char **argv) {
    int fd;
    char buffer[1024];
    int n;

    // borrar FIFO por si existiera previamente
    unlink(FIFO);

    /* crear el FIFO */
    if (mkfifo(FIFO, 0666)<0) {
        perror("No puede crearse el FIFO");
        return(1);
    }
    /* Abrir el FIFO */
    if ((fd = open(FIFO, O_RDONLY)) < 0) {
        perror("No puede abrirse el FIFO");
        return(1);
    }
    /* Leer de la FIFO */
    while ((n = read(fd, buffer, sizeof(buffer)-1)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }
    close(fd);
    unlink(FIFO);
    return(0);
}
