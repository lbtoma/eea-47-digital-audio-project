#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <avr/iom2560.h>

#define CPU_CLOCK 16000000uL
#define SAMPLE_PERIOD_US 125 // Sample rate == 8 kHz
#define CLOCKS_PER_MILISECONDS CPU_CLOCK / SAMPLE_PERIOD_US
#define NUM_SAMPLES 20

void set_ports(void);
void timer_1_init(void);

volatile unsigned long int g_current_sample;

unsigned int samples[NUM_SAMPLES] = {
    127,
    166,
    202,
    230,
    248,
    255,
    248,
    230,
    202,
    166,
    127,
    88,
    52,
    24,
    6,
    0,
    6,
    24,
    52,
    88};

int main(void)
{
  set_ports();
  timer_1_init();
  sei();

  while (1)
  {
  };
}

ISR(TIMER1_COMPA_vect)
{
  unsigned char sample = samples[g_current_sample % NUM_SAMPLES];

  PORTH = sample;
  PORTJ = sample;
  g_current_sample++;
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

  OCR1AH = CLOCKS_PER_MILISECONDS >> 8;
  OCR1AL = CLOCKS_PER_MILISECONDS & 0xff;

  // Timer/Counter 1 Interrupt(s) initialization
  TIMSK1 = (0 << ICIE1) | (0 << OCIE1C) | (0 << OCIE1B) | (1 << OCIE1A) | (0 << TOIE1);
}
