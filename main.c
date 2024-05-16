#define WORK_WITH_TIMER 1
#define WORK_WITH_DELAY 0

#if WORK_WITH_DELAY
//==============================================//
// Task 1: generate output with delay functions //
//==============================================//
#define F_CPU 16000000UL // 16 MHz frequency; should be defined before the inclusion of delay.h
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB  = 0x20;  // output, built-in led L
	while (1)
	{
		PORTB = 0x20;
		_delay_ms(1000); // Wait 100 ms
		PORTB = 0x00; // Switch LED off
		_delay_ms(1000); // Wait 100 ms
	}
}
#endif
#if WORK_WITH_TIMER
//======================================================//
// Task 2: generate output with timer instead of delays //
//======================================================//
#define F_CPU 16000000L
#define sei() asm volatile("sei"::)
#include <avr/io.h>
#include <avr/interrupt.h>

// First we need some global variables
volatile uint32_t iCounter = 0; // Timer overflow interrupt counter
volatile uint8_t flagB0 = 0;    // Flag associated to port B0

void configureIO()
{
	DDRD = 0x00;
	DDRB = 0x00;

	//  Configure the output:
	DDRB = 1 << PORTB5;
	PORTB = 0x00; //1 << PORTB5;
}

void configure_prescaler(uint8_t prescaler)
{
	//  The three bits of TCCR0B CS00, CS01 and CS02 configure the prescaler:
	//
	//  | CS02 | CS01 | CS00 | Prescaler |
	//  |   0  |   0  |   0  |  no clock |
	//  |   0  |   0  |   1  |    1      |
	//  |   0  |   1  |   0  |    8      |
	//  |   0  |   1  |   1  |    64     |
	//  |   1  |   0  |   0  |    256    |
	//  |   1  |   0  |   1  |    1024   |
	switch (prescaler)
	{
		case 2: // prescaling factor 8
		TCCR0B &=~(1 << CS02); // Set CS02 to 0
		TCCR0B |= (1 << CS01); // Set CS01 to 1
		TCCR0B &=~(1 << CS00); // Set CS00 to 0
		break;
		case 3: // prescaling factor 64
		TCCR0B &=~(1 << CS02); // Set CS02 to 0
		TCCR0B |= (1 << CS01); // Set CS01 to 1
		TCCR0B |= (1 << CS00); // Set CS00 to 1
		break;
		case 4: // prescaling factor 256
		TCCR0B |= (1 << CS02); // Set CS02 to 1
		TCCR0B &=~(1 << CS01); // Set CS01 to 0
		TCCR0B &=~(1 << CS00); // Set CS00 to 0
		break;
		case 5: // prescaling factor 1024
		TCCR0B |= (1 << CS02); // Set CS02 to 1
		TCCR0B &=~(1 << CS01); // Set CS01 to 0
		TCCR0B |= (1 << CS00); // Set CS00 to 1
		break;
		default: // prescaling factor 1
		TCCR0B &=~(1 << CS02); // Set CS02 to 0
		TCCR0B &=~(1 << CS01); // Set CS01 to 0
		TCCR0B |= (1 << CS00); // Set CS00 to 1
		break;
	}
}

void configureTimer()
{
	//  Timer Counter Control Register 0 A (TCCR0A) Set register to 0 (default)
	//  For further information see ATmega328P manual
	TCCR0A = 0x00;

	//  Timer Counter Control Register 0 B (TCCR0B) Set register to 0 (default)
	TCCR0B = 0x00;


	//  Configure the prescaler:
	//volatile const uint8_t prescaler = 5;
	configure_prescaler(5);

	//  TC0 Interrupt Mask Register (TIMSK0)
	//  The TOIE bit activates the overflow interrupt
	
	TIMSK0 = 0x00;
	TIMSK0 |= (1 << TOIE0);
}


//  TIMER0_OVF_vect is the interrupt vector for an overflow of timer 0
//  ISR is the interrupt service routine
ISR(TIMER0_OVF_vect)
{
	//  The clock of the ATmega328P is 16 MHz
	//  The timer has 8 bits -> it can count from 0 to 255 before overflow

	//  The overflow therefore occurs with frequency
	//  f = 16 MHz / 256 / (prescaler) = 62.5 kHz / (prescaler)

	//  To define the time at which the event happens, calculation of number for iCounter is needed

	iCounter++;
	if((iCounter>=40))
	{
		PORTB ^=(1 << PORTB5);
		iCounter=0;
	}
}

int main()
{
	configureIO();
	configureTimer();
	configure_prescaler(4);
	//  Set the interrupt register bits
	sei();
	while(1)
	{
	}
}
#endif