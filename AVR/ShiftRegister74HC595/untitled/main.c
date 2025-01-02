#include <stdio.h>


void print_binary(unsigned char value) {
    for (int i = 7; i >= 0; i--) { // Iterate from the most significant bit to the least significant bit
        printf("%c", (value & (1 << i)) ? '1' : '0');
    }
    printf("\n\r");
}

int main(void) {
    unsigned char shift_register = 0b1001; // مقدار اولیه: 0011

    for (int i = 0; i < 10; i++) { // 10 تکرار برای مثال
        print_binary(shift_register);
        // شیفت حلقوی به چپ
        shift_register = (shift_register >> 1) | ((shift_register << 3) & 0b1000);

        // 0110 0110 & 0001
        // 1100 1001 & 0001 =



    }

    return 0;
}


