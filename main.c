
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

int main(int argc, char **argv) {
    char *msg;
    size_t len;
    int i;
    uint8_t result[16];
 
    if (argc < 2) {
        printf("usage: %s 'string'\n", argv[0]);
        return 1;
    }
    msg = argv[1];
 
    len = strlen(msg);
 
    // benchmark
     md5((uint8_t*)msg, len, result);
 
    // display result
    for (i = 0; i < 16; i++)
        printf("%2.2x", result[i]);
    puts("");
 
    return 0;
}
