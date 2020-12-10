#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/iom2560.h>

#define CPU_CLOCK 16000000uL
#define SAMPLE_RATE 8000 // Sample rate == 8 kHz
#define CLOCKS_PER_SAMPLE 50
#define SAMPLES_PER_FRAME 40
#define NUM_SAMPLES_L 20
#define NUM_SAMPLES_R 40
#define SILENCE_CLOCKS 10

void set_ports(void);
void timer_1_init(void);
void enable_timer_1(void);
void disable_timer_1(void);
void timer_2_init(void);
unsigned long int clock(void);

volatile unsigned long int g_sample_counter = 0;
volatile unsigned long int reference_clock = 0;
volatile unsigned char preamble_count = 0;
volatile unsigned char left_data = 0;
volatile unsigned char right_data = 0;
volatile unsigned char last_level = 0;


int main(void)
{
  set_ports();
  timer_1_init();
  timer_2_init();
  sei();

  while (1)
  {
    while (clock() - reference_clock < SILENCE_CLOCKS)
    {
    }
    while (preamble_count < 8)
    {
    }
    enable_timer_1();
    preamble_count = 0;
    while (g_sample_counter < 17)
    {
    }
    disable_timer_1();
    PORTH = left_data;
    PORTJ = right_data;
    g_sample_counter = 0;
    preamble_count = 0;
    reference_clock = clock();
  };
}

ISR(PCINT0_vect) {
  preamble_count++;
  reference_clock = clock();
}

ISR(TIMER1_COMPA_vect)
{
  if (g_sample_counter == 0) {
    TCNT3H = 0;
    TCNT3L = CLOCKS_PER_SAMPLE/2;
  }
  unsigned char current_level = (PINB & (1 << PC0));
  unsigned char incoming = !(current_level ^ last_level);
  last_level = current_level;
  if (g_sample_counter < 8) {
    left_data |= 1 << g_sample_counter;
  } else if (g_sample_counter < 16) {
    right_data |= 1 << (g_sample_counter - 8);
  }
  g_sample_counter++;
}

void set_ports(void)
{
  DDRH = 0xff;       // Left Output Channel
  DDRB = 0 << PORT0; // Digital audio input
  DDRJ = 0xff;       // Right Output Channel
}

void timer_1_init(void)
{
  // Mode 4

  TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) |
           (0 << COM1C1) | (0 << COM1C0) | (0 << WGM11) | (0 << WGM10);
  TCCR1B = (0 << ICNC1) | (0 << ICES1) | (0 << WGM13) | (1 << WGM12) |
           (0 << CS12) | (0 << CS11) | (1 << CS10);

  OCR1AH = CLOCKS_PER_SAMPLE >> 8;
  OCR1AL = CLOCKS_PER_SAMPLE & 0xff;
}

void enable_timer_1(void) {
  // Timer/Counter 1 Interrupt(s) initialization
  TIMSK1 = (0 << ICIE1) | (0 << OCIE1C) | (0 << OCIE1B) | (1 << OCIE1A) | (0 << TOIE1);
}

void disable_timer_1(void) {
  // Timer/Counter 1 Interrupt(s) initialization
  TIMSK1 = (0 << ICIE1) | (0 << OCIE1C) | (0 << OCIE1B) | (0 << OCIE1A) | (0 << TOIE1);
}

void timer_2_init(void)
{
  // Mode 4

  TCCR1A = (0 << COM3A1) | (0 << COM3A0) | (0 << COM3B1) | (0 << COM3B0) |
           (0 << COM3C1) | (0 << COM3C0) | (0 << WGM31) | (0 << WGM30);
  TCCR1B = (0 << ICNC3) | (0 << ICES3) | (0 << WGM33) | (1 << WGM32) |
           (0 << CS32) | (0 << CS31) | (1 << CS30);

  OCR3AH = 0;
  OCR3AL = 1;
}

unsigned long int clock(void)
{
  return (TCNT3H<<8) | TCNT3L;
} 
