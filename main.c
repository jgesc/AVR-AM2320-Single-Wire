#define F_CPU 1000000L
#include <avr/interrupt.h>
#include <stdint.h>

uint8_t data[5] = {0}; // Sensor data
uint8_t offset = 0; // Data storage offset
uint8_t bits = 0; // Read bits
uint8_t buff = 0; // Buffer
uint8_t h = 0; // Pin state HIGH
uint8_t skip = 3; // Preamble counter

ISR(PCINT0_vect)
{
  // Ignore preamble
  if(skip)
  {
    skip--;
    return;
  }

  cli(); // Disable interrupts
  h = !h; // Switch HIGH flag
  if(h)
  {
    // Record time
  }
  else
  {
    // Store received bit
    t = 0; // Get time diff

    buff <<= 1;
    if(t > 50)
    {
      // Write 1
      buff |= 1;
    }

    bits++;
    // If buffer full, store and clear
    if(bits == 8)
    {
      data[offset] = buff;
      offset++;
      buff = 0;

      if(offset == 5)
      {
        // Finished transmitting

      }
    }
  }
  sei(); // Enable interrupts
}

int main(void)
{
  GIMSK = 0b00100000;
  PCMSK = 0b00000001;
  sei();

  while(1)
  {
    if(offset == 5) break;
  }

  return 0;
}
