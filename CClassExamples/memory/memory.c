#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>

void usage(char *prog)
{
    fprintf(stderr, "usage:␣%s␣MEMFILE\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "MEMFILE␣must␣be␣valid␣file\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int rc, fd = -1;
    uint64_t value;
    unsigned char *mem = NULL;
    struct stat fileinfo;
    int exitcode = EXIT_FAILURE;
    if (argc != 2) {
        usage(argv[0]);
    }
    /* Determine size of memory file. */
    rc = stat(argv[1], &fileinfo);
    if (-1 == rc) {
        perror("could␣not␣determine␣size␣of␣memory␣file");
        goto done;
    }
    fd = open(argv[1], O_RDONLY);
    if (-1 == fd) {
        perror("could␣not␣open␣memory␣file");
        goto done;
    }
    /* Allocate memory to store file. */
    mem = malloc(fileinfo.st_size);
    if (NULL == mem) {
        perror("allocation␣failed");
        goto done;
    }
    /* Read entire file into buffer. */
    rc = read(fd, mem, fileinfo.st_size);
    if (rc != fileinfo.st_size) {
        perror("could␣not␣read␣memory␣file");
        goto done;
    }
    /* mem points to an unsigned char; cast it to point to a uint64_t. */
    value = *((uint64_t *) mem);
    printf("%" PRIx64 "\n", value);
done:
    if (fd != -1) {
        close(fd);
    }
    free(mem);
    exit(exitcode);
}