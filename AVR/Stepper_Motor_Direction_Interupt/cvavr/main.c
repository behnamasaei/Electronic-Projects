

#include <mega16a.h>
#include <mega16_bits.h>
#include <stdio.h>
#include <stdbool.h>
#include <delay.h>

#define PLAY_BUTTON 0b1
#define SPEED_UP_BUTTON 0b10
#define SPEED_DOWN_BUTTON 0b100
#define DIR_LEFT_BUTTON 0b1000
#define DIR_RIGHT_BUTTON 0b10000
#define MAX_SPEED_CHANGE_COIL 100
#define MIN_SPEED_CHANGE_COIL 2000

bool RIGHT_DIR = true;
bool POWER = true;
int SPEED = 1000;

unsigned char COIL_PIN_RIGHT_SUB_DEG = 0b0011;
unsigned char COIL_PIN_LEFT_SUB_DEG = 0b0011;
unsigned char COIL_PIN_MAIN_DEG = 0b0001;

interrupt[EXT_INT0] void ext_int0_isr(void)
{
      printf("SPEED Change Coil: %d\n\r", SPEED);

      if (PINA & PLAY_BUTTON)
      {
            POWER = !POWER;
      }
      else if (PINA & SPEED_UP_BUTTON)
      {
            if ((SPEED - 100) > MAX_SPEED_CHANGE_COIL)
                  SPEED -= 100;
      }
      else if (PINA & SPEED_DOWN_BUTTON)
      {
            if ((SPEED + 100) < MIN_SPEED_CHANGE_COIL)
                  SPEED += 100;
      }
      else if (PINA & DIR_RIGHT_BUTTON)
      {
            RIGHT_DIR = true;
            COIL_PIN_RIGHT_SUB_DEG = 0b0011;
            COIL_PIN_LEFT_SUB_DEG = 0b0011;
            COIL_PIN_MAIN_DEG = 0b0001;
      }
      else if (PINA & DIR_LEFT_BUTTON)
      {
            RIGHT_DIR = false;
            COIL_PIN_RIGHT_SUB_DEG = 0b0011;
            COIL_PIN_LEFT_SUB_DEG = 0b1001;
            COIL_PIN_MAIN_DEG = 0b0001;
      }
}

void run_stepper()
{
      if (POWER)
      {
            if (RIGHT_DIR)
            {
                  COIL_PIN_MAIN_DEG = (COIL_PIN_MAIN_DEG << 1) | ((COIL_PIN_MAIN_DEG >> 3) & 0x01);
                  COIL_PIN_MAIN_DEG &= 0x0F;
                  PORTC |= COIL_PIN_MAIN_DEG;
                  delay_ms(SPEED / 2);
                  PORTC &= 0xF0;

                  COIL_PIN_RIGHT_SUB_DEG = (COIL_PIN_RIGHT_SUB_DEG << 1) | ((COIL_PIN_RIGHT_SUB_DEG >> 3) & 0x01);
                  COIL_PIN_RIGHT_SUB_DEG &= 0x0F;
                  PORTC |= COIL_PIN_RIGHT_SUB_DEG;
                  delay_ms(SPEED / 2);
                  PORTC &= 0xF0;
            }
            else if (RIGHT_DIR == false)
            {

                  COIL_PIN_MAIN_DEG = (COIL_PIN_MAIN_DEG >> 1) | ((COIL_PIN_MAIN_DEG << 3) & 0b1000);
                  COIL_PIN_MAIN_DEG &= 0x0F;
                  PORTC |= COIL_PIN_MAIN_DEG;
                  delay_ms(SPEED / 2);
                  PORTC &= 0xF0;

                  COIL_PIN_LEFT_SUB_DEG = (COIL_PIN_LEFT_SUB_DEG >> 1) | ((COIL_PIN_LEFT_SUB_DEG << 3) & 0b1000);
                  COIL_PIN_LEFT_SUB_DEG &= 0x0F;
                  PORTC |= COIL_PIN_LEFT_SUB_DEG;
                  delay_ms(SPEED / 2);
                  PORTC &= 0xF0;
            }
      }
}

void main(void)
{

      DDRC |= (1 << DDC3) | (1 << DDC2) | (1 << DDC1) | (1 << DDC0);
      PORTC &= ~((1 << PORTC3) | (1 << PORTC2) | (1 << PORTC1) | (1 << PORTC0));

      // External Interrupt(s) initialization
      // INT0: On
      // INT0 Mode: Rising Edge
      // INT1: Off
      // INT2: Off
      MCUCR |= (1 << ISC01) | (1 << ISC00);
      GICR |= (1 << INT0);
      SREG |= (1 << 7);
      GIFR |= (1 << INTF0);

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

// Global enable interrupts
#asm("sei")

      while (1)
      {
            run_stepper();
      }
}
