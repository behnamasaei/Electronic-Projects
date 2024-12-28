#include <mega16a.h>
#include <mega16_bits.h>
#include <stdint.h>
#include <delay.h>

uint16_t number[] = {
    //    gfedcba
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111  // 9
};

void main(void)
{
      DDRC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3) | (1 << PORTC4) | (1 << PORTC5) | (1 << PORTC6);
      PORTC = 0x0;

      while (1)
      {
            uint16_t counter = 0;

            for (counter = 0; counter < 10; counter++)
            {
                  PORTC = number[counter];
                  delay_ms(1000);     // Display each number for 1 second
                  PORTC = 0x00000000; // Turn off display
                  delay_ms(10);      // Add a small delay between numbers
            }
      }
}