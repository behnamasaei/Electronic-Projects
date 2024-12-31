#include <stdio.h>

int main(void) {
    unsigned char address = 0B01111100;

    printf("Size of address: %zu\n", sizeof(0B01111100));  // Use %zu for size_t type
    for (int i = 0; i < 8; ++i) {
        address <<= 1;
        printf("%02X\n", address);  // Print address as hexadecimal value
    }

    return 0;
}
