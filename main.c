#define F_CPU 8000000L
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

uint8_t edgc = 0; // Edge counter
uint8_t tm[85] = {0}; // Timing buffer
uint8_t msg[5] = {0}; // Message buffer

/// Edge interrupt
ISR(PCINT0_vect)
{
	// Limit edges to 85
	if(edgc >= 85) return;
	// Record timer counter value
	tm[edgc] = TCNT1;
	// Reset timer
	TCNT1 = 0;
	// Increase edge counter
	edgc++;
}

/// Initialization routine
void init()
{
	// Block interrupts
	cli();

	// Modify system clock
	CLKPR = 0b10000000; // Unlock clock speed modification
	CLKPR = 0b00000000; // Set divisor to 1 so frequency is 8MHz

	// Set interrupt mask
	GIMSK = 0b00100000; // Enable edge interrupt
	PCMSK = 0b00010000; // Enable edge interrupt on PB4

	/// Enable time on fast peripheral clock
	// Select frequency divisor 4 (2 MHz)
	TCCR1 |= (0 << CS13);
	TCCR1 |= (0 << CS12);
	TCCR1 |= (1 << CS11);
	TCCR1 |= (1 << CS10);

	// Set pin as input
  DDRB &= ~(1 << DDB4);
  // Set pull-up
  PORTB |= (1 << PB4);
	// Wait to stabilize
  _delay_ms(1000);
}

/// Polls sensor data
void poll()
{
	// Reset edge counter
	edgc = 0;
	// Waje signal
	PORTB ^= (1 << PB4);
	_delay_ms(1);
	PORTB ^= (1 << PB4);
	// Reset timer
	TCNT1 = 0;
	// Enables interrupts
	sei();
}

/// Parse timings. Returns a 'true' value if the CRC check is correct.
uint8_t parsetm()
{
	uint8_t off = 0; // Message buffer offset
	uint8_t bit = 0; // Bit counter
	uint8_t tmoff; // Timing buffer offset

	// Clean msg buffer
	int i;
	for(i = 0; i < 5; i++) msg[i] = 0;

	// Iterate over timings
	for(tmoff = 6; tmoff < 85; tmoff += 2)
	{
		// Get ready to write next bit
		msg[off] <<= 1;
		// Compare current timing with response time
		if(tm[tmoff] > tm[tmoff - 1]) msg[off] |= 1;
		// Increase bit counter
		bit++;

		// Check if we have completed a byte
		if(bit == 8)
		{
			// Increase buffer offset
			off++;
			// Reset bit counter
			bit = 0;
			// Check if we have written the 5 bytes
			if(off == 5)
			{
				// Check CRC
				return (uint8_t)(msg[0] + msg[1] + msg[2] + msg[3]) == msg[4];
			}
		}
	}
}

int main(void)
{
	// Initialize sensor
	init();

	// Send message
  while(1)
  {
		// Poll sensor
		poll();
		// Wait and send results
		_delay_ms(1000);
		if(parsetm())
		{
			// Readings stored in 'msg'
			_delay_ms(10);
		}
		// Wait for next reading
		_delay_ms(10000);
	}
  return 0;
}
