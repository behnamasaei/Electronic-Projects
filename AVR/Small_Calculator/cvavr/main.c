#include <mega16a.h>
#include <mega16_bits.h>
#include <stdio.h>
#include <stdint.h>
#include <delay.h>
#include <stdbool.h>
#include <stdlib.h>
#include <alcd.h>
#include <string.h>

// Keypad configurations
#define ROW_MASK 0x00 // Rows: PC0-PC3 as output
#define COL_MASK 0xF0 // Columns: PC4-PC7 as input

// Keypad mapping
char keyPad[4][4] = {
    {'7', '8', '9', '/'},
    {'4', '5', '6', '*'},
    {'1', '2', '3', '-'},
    {'c', '0', '=', '+'}};

char numberOneArr[16], numberTwoArr[16];
char operator= '\0';

// Declare variables
char num1_str[16];
char num2_str[16];
int numberOne = 0;
int numberTwo = 0, result = 0;
char buffer[16];

void check_key_selected();

void main(void)
{
      // // Configure rows (PC0-PC3) as output
      // DDRC |= ROW_MASK;

      // // Configure columns (PC4-PC7) as input with pull-ups
      // DDRC &= ~COL_MASK;

      DDRC = (0 << DDC7) | (0 << DDC6) | (0 << DDC5) | (0 << DDC4) | (1 << DDC3) | (1 << DDC2) | (1 << DDC1) | (1 << DDC0);

      // Clear all rows initially
      // PORTC &= ~ROW_MASK;

      // USART initialization
      // Communication Parameters: 8 Data, 1 Stop, No Parity
      // USART Receiver: Off
      // USART Transmitter: On
      // USART Mode: Asynchronous
      // USART Baud Rate: 9600
      UCSRA = (0 << RXC) | (0 << TXC) | (0 << UDRE) | (0 << FE) | (0 << DOR) | (0 << UPE) | (0 << U2X) | (0 << MPCM);
      UCSRB = (0 << RXCIE) | (0 << TXCIE) | (0 << UDRIE) | (0 << RXEN) | (1 << TXEN) | (0 << UCSZ2) | (0 << RXB8) | (0 << TXB8);
      UCSRC = (1 << URSEL) | (0 << UMSEL) | (0 << UPM1) | (0 << UPM0) | (0 << USBS) | (1 << UCSZ1) | (1 << UCSZ0) | (0 << UCPOL);
      UBRRH = 0x00;
      UBRRL = 0x19;

      // Alphanumeric LCD initialization
      // Connections are specified in the
      // Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
      // RS - PORTA Bit 0
      // RD - PORTA Bit 1
      // EN - PORTA Bit 2
      // D4 - PORTA Bit 4
      // D5 - PORTA Bit 5
      // D6 - PORTA Bit 6
      // D7 - PORTA Bit 7
      // Characters/line: 16
      lcd_init(16);

      // Main loop
      while (1)
      {

            check_key_selected();
      }
}

void check_key_selected()
{
      uint8_t row, col;
      int inputCounter = 0;
      char inputCharector = NULL;
      bool inputOneFlag = true;

      while (1)
      {
            // Iterate through each row
            for (row = 0; row < 4; row++)
            {
                  // // Set the current row LOW (active) and others HIGH
                  // PORTC = (~(1 << row) & ROW_MASK) | (PORTC & COL_MASK);
                  PORTC |= (1 << row);

                  // Small delay for stabilization

                  // Check each column
                  for (col = 4; col < 8; col++)
                  {
                        if ((PINC & (1 << col))) // If column is LOW (key pressed)
                        {
                              inputCharector = keyPad[row][col - 4];

                              // Clear variables if 'C' is pressed
                              if (inputCharector == 'c')
                              {
                                    while ((PINC & (1 << col)))
                                          ;
                                    lcd_clear();
                                    memset(num1_str, 0, sizeof(num1_str));
                                    memset(num2_str, 0, sizeof(num2_str));
                                    operator= '\0';
                                    numberOne = numberTwo = result = 0;
                                    inputOneFlag = true;
                                    inputCounter = 0;

                                    return;
                              }

                              // Handle operator
                              if (inputCharector == '/' || inputCharector == '*' || inputCharector == '-' || inputCharector == '+')
                              {
                                    operator= inputCharector;
                                    inputOneFlag = false;
                                    inputCounter = 0;
                                    lcd_putchar(inputCharector);
                                    while ((PINC & (1 << col)))
                                          ;
                                    continue;
                              }

                              // Handle '=' (calculate result)
                              if (inputCharector == '=')
                              {
                                    while ((PINC & (1 << col)))
                                          ;
                                    numberOne = atoi(num1_str);
                                    numberTwo = atoi(num2_str);

                                    printf("%d\n\r", numberOne);

                                    printf("%d\n\r", numberTwo);

                                    lcd_gotoxy(0, 1); // Move to the second row
                                    lcd_putchar('=');

                                    switch (operator)
                                    {
                                    case '/':
                                          if (numberTwo != 0)
                                                result = numberOne / numberTwo;
                                          else
                                                lcd_putsf("Error: Div/0");
                                          break;

                                    case '*':
                                          result = numberOne * numberTwo;
                                          break;

                                    case '-':
                                          result = numberOne - numberTwo;
                                          break;

                                    case '+':
                                          result = numberOne + numberTwo;
                                          break;

                                    default:
                                          lcd_putsf("Invalid Op");
                                          return;
                                    }

                                    // Ensure buffer is clear and write the result
                                    sprintf(buffer, "%d", result);
                                    lcd_puts(buffer); // Display the result
                                                      // Print the buffer contents
                                    printf("Buffer content: %s\n", buffer);

                                    // Reset for the next calculation
                                    memset(num1_str, 0, sizeof(num1_str));
                                    memset(num2_str, 0, sizeof(num2_str));
                                    operator= '\0';
                                    inputOneFlag = true;
                                    inputCounter = 0;
                                    return;
                              }

                              // Append to the appropriate string
                              if (inputOneFlag)
                              {
                                    num1_str[inputCounter] = inputCharector;
                                    inputCounter++;
                                    lcd_putchar(inputCharector);
                              }
                              else
                              {
                                    num2_str[inputCounter] = inputCharector;
                                    inputCounter++;
                                    lcd_putchar(inputCharector);
                              }

                              // Display the input character
                              printf("inputCounter: %d \n\r", inputCounter);

                              // Wait until the key is released (debounce)
                              while ((PINC & (1 << col)))
                                    ;
                              delay_ms(50); // Additional debounce delay
                        }
                  }

                  // if (inputCharector == '=' && operator!= NULL)
                  //       break;

                  PORTC &= ~(1 << row);
                  delay_ms(10);
            }
      }
}
