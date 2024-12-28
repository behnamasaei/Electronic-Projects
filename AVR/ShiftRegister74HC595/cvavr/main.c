#include <mega16a.h>
#include <mega16_bits.h>
#include <io.h>
#include <delay.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Define shift register control pins
#define SH_CP PORTC0 // Shift Clock
#define DS PORTC1    // Data
#define ST_CP PORTC3 // Latch Clock

void init_shift_register();
void send_shift_register();
void send_pulse(uint8_t binaryNumber, bool sendZeroBit);

void main(void)
{
      // Configure control pins as outputs
      DDRC |= (1 << SH_CP) | (1 << DS) | (1 << ST_CP);

      UCSRA = (0 << RXC) | (0 << TXC) | (0 << UDRE) | (0 << FE) | (0 << DOR) | (0 << UPE) | (0 << U2X) | (0 << MPCM);
      UCSRB = (0 << RXCIE) | (0 << TXCIE) | (0 << UDRIE) | (0 << RXEN) | (1 << TXEN) | (0 << UCSZ2) | (0 << RXB8) | (0 << TXB8);
      UCSRC = (1 << URSEL) | (0 << UMSEL) | (0 << UPM1) | (0 << UPM0) | (0 << USBS) | (1 << UCSZ1) | (1 << UCSZ0) | (0 << UCPOL);
      UBRRH = 0x00;
      UBRRL = 0x19;

      init_shift_register();
      send_shift_register();

      // Infinite loop
      while (1)
      {
      }
}

void init_shift_register(void)
{
      // Initialize pins to low state
      PORTC &= ~(1 << SH_CP);
      PORTC &= ~(1 << DS);
      PORTC &= ~(1 << ST_CP);
}

void send_shift_register()
{
      int unit = 0, tens = 0;
      uint8_t unitNumber, tensNumber;
      uint8_t numbers[] = {
          //    gfedcba
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

      while (1)
      {

            if (unit == 10)
            {
                  unit = 0;
                  tens++;
            }

            unitNumber = numbers[unit]; // Binary number (7 bits)
            tensNumber = numbers[tens];

            send_pulse(unitNumber, true);
            delay_us(10);
            send_pulse(tensNumber, false);

            // Latch data
            PORTC |= (1 << ST_CP);
            delay_us(10);
            PORTC &= ~(1 << ST_CP);
            delay_ms(200);

            if (unit == 9 & tens == 9)
            {
                  tens = 0;
                  unit = 0;
            }
            else
            {
                  unit++;
            }
      }
}

void send_pulse(uint8_t binaryNumber, bool sendZeroBit)
{
      int j;
      uint8_t bitChek;
      for (j = 6; j >= 0; j--)
      {
            // Extract the bit
            bitChek = (binaryNumber >> j) & 1;

            // Set or clear DS based on the bit value
            if (bitChek)
            {
                  PORTC |= (1 << DS);
            }
            else
            {
                  PORTC &= ~(1 << DS);
            }

            // Pulse SH_CP
            PORTC |= (1 << SH_CP);
            delay_us(10);
            PORTC &= ~(1 << SH_CP);
            delay_us(10);
      }

      if (sendZeroBit)
      {

            PORTC &= ~(1 << DS);

            // Pulse SH_CP
            PORTC |= (1 << SH_CP);
            delay_us(10);
            PORTC &= ~(1 << SH_CP);
            delay_us(10);
      }
}