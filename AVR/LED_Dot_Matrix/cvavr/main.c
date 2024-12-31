#include <mega32.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Define pins for MAX7219
#define MAX7219_DIN PORTB .5 // Data in
#define MAX7219_CS PORTB .4  // Chip select
#define MAX7219_CLK PORTB .3 // Clock

// MAX7219 registers
#define DECODE_MODE 0x09
#define INTENSITY 0x0A
#define SCAN_LIMIT 0x0B
#define SHUTDOWN 0x0C
#define DISPLAY_TEST 0x0F

// Example smiley face pattern
unsigned char IMAGES[][8] = {
    {0b00010000,
     0b00110000,
     0b00010000,
     0b00010000,
     0b00010000,
     0b00010000,
     0b00010000,
     0b00111000},
    {0b00111000,
     0b01000100,
     0b00000100,
     0b00000100,
     0b00001000,
     0b00010000,
     0b00100000,
     0b01111100},
    {0b00111000,
     0b01000100,
     0b00000100,
     0b00011000,
     0b00000100,
     0b00000100,
     0b01000100,
     0b00111000},
    {0b00000100,
     0b00001100,
     0b00010100,
     0b00100100,
     0b01000100,
     0b01111100,
     0b00000100,
     0b00000100},
    {0b01111100,
     0b01000000,
     0b01000000,
     0b01111000,
     0b00000100,
     0b00000100,
     0b01000100,
     0b00111000},
    {0b00111000,
     0b01000100,
     0b01000000,
     0b01111000,
     0b01000100,
     0b01000100,
     0b01000100,
     0b00111000},
    {0b01111100,
     0b00000100,
     0b00000100,
     0b00001000,
     0b00010000,
     0b00100000,
     0b00100000,
     0b00100000},
    {0b00111000,
     0b01000100,
     0b01000100,
     0b00111000,
     0b01000100,
     0b01000100,
     0b01000100,
     0b00111000},
    {0b00111000,
     0b01000100,
     0b01000100,
     0b01000100,
     0b00111100,
     0b00000100,
     0b01000100,
     0b00111000},
    {0b00111000,
     0b01000100,
     0b01000100,
     0b01000100,
     0b01000100,
     0b01000100,
     0b01000100,
     0b00111000}};

const int IMAGES_LEN = sizeof(IMAGES) / 8;

// Initialize pins as outputs
void MAX7219_Init(void)
{
      // Configure pins as outputs
      DDRB .5 = 1; // DIN
      DDRB .4 = 1; // CS
      DDRB .3 = 1; // CLK

      // Default pin states
      MAX7219_CS = 1;  // CS high
      MAX7219_CLK = 0; // CLK low
      MAX7219_DIN = 0; // DIN low
}

// Send data to MAX7219
void MAX7219_Send(unsigned char address, unsigned char data)
{
      unsigned char i;

      MAX7219_CS = 0; // Select device

      // Send address byte
      for (i = 0; i < 8; i++)
      {
            MAX7219_CLK = 0;
            MAX7219_DIN = (address & 0x80) ? 1 : 0;
            MAX7219_CLK = 1;
            address <<= 1;
      }

      // Send data byte
      for (i = 0; i < 8; i++)
      {
            MAX7219_CLK = 0;
            MAX7219_DIN = (data & 0x80) ? 1 : 0;
            MAX7219_CLK = 1;
            data <<= 1;
      }

      MAX7219_CS = 1; // Deselect device
}

// Configure MAX7219
void MAX7219_Configure(void)
{
      unsigned char i;

      // Disable BCD decoding
      MAX7219_Send(DECODE_MODE, 0x00);

      // Set medium intensity (8/32)
      MAX7219_Send(INTENSITY, 0x03);

      // Scan all digits (0-7)
      MAX7219_Send(SCAN_LIMIT, 0x07);

      // Normal operation mode
      MAX7219_Send(SHUTDOWN, 0x01);

      // Disable display test
      MAX7219_Send(DISPLAY_TEST, 0x00);

      // Clear all digits
      for (i = 1; i <= 8; i++)
      {
            MAX7219_Send(i, 0x00);
      }
}

// Set a single LED in the matrix
void MAX7219_SetLED(unsigned char row, unsigned char col, unsigned char state)
{
      unsigned char current_row;

      // Read current row data
      MAX7219_Send(row + 1, state ? (1 << col) : 0);
}

// Display a pattern on the matrix
void MAX7219_DisplayPattern(unsigned char pattern[8])
{
      unsigned char i;

      for (i = 0; i < 8; i++)
      {
            MAX7219_Send(i + 1, pattern[i]);
      }
}

void MAX7219_Clear(void)
{
      unsigned char i;
      for (i = 1; i <= 8; i++)
      {
            MAX7219_Send(i, 0x00);
      }
}

void main(void)
{
      unsigned char i;
      // Initialize MAX7219
      MAX7219_Init();
      MAX7219_Configure();
      MAX7219_Clear();

      while (1)
      {

            for (i = 0; i < IMAGES_LEN; i++)
            {
                  // Display smiley face
                  MAX7219_DisplayPattern(IMAGES[i]);

                  delay_ms(500); // Show for 1 second

                  // Clear display
                  MAX7219_Clear();
                  delay_ms(100); // Off for 0.5 second
            }
      }
}