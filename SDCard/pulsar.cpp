#include "pulsar.h"
#include "rena.h"

namespace MSP430
{

void (*interruptVectorPORT2)(void);
pulsar::pulsar(unsigned char* port, unsigned char pin, bool init)
{


	CurrentPort = port;
	CurrentPin = pin;
	CurrentDir = 1;

	if(init)
		*CurrentPort |= CurrentPin;
	else
		*CurrentPort &= ~CurrentPin;

	*(CurrentPort + 0x0002) |= CurrentPin;
}

pulsar::pulsar(unsigned char* port, unsigned char pin)
{
	CurrentPort = port;
	CurrentPin = pin;
	CurrentDir = 0;

	*(CurrentPort + 0x0002) &= ~CurrentPin;
}

pulsar::pulsar(unsigned char* port, unsigned char pin, InterruptType type, void (*interruptVector)(void))
{
	CurrentPort = port;
	CurrentPin = pin;
	CurrentDir = 0;


	*(CurrentPort + 0x0002) &= ~CurrentPin; //direction
	*(CurrentPort + 0x0018) |= CurrentPin;  //Interrupt enable
	if(type == RisingEdge)
		*(CurrentPort + 0x0016) &= ~CurrentPin;
	else if(type == FallingEdge)
		*(CurrentPort + 0x0016) |= CurrentPin;
	*(CurrentPort + 0x001A) &= ~CurrentPin;


	*(CurrentPort + 0x0004)|= CurrentPin;               // user guide table 10.1
	  P2OUT |= CurrentPin;                            // Set P1.4 as pull-Up resistance
	 // P2IE |= CurrentPin;                           // P1.4 interrupt enabled
	  //P2IES |= 0x00;                           // P1.4 Hi/Lo edge
	  *(CurrentPort + 0x001A) &= ~CurrentPin;

	interruptVectorPORT2 = interruptVector;
}
void pulsar::set(bool value)
{
	if(CurrentDir)
	{
	if(value)
		*CurrentPort |= CurrentPin;
	else
		*CurrentPort &= ~CurrentPin;
	}
}

bool pulsar::get()
{
	if(CurrentDir)
	{
	if((*CurrentPort) & CurrentPin)
		return true;
	else
		return false;
	}
	else
	{
		if( *(CurrentPort-0x0002) & CurrentPin )
				return true;
			else
				return false;
	}
}


void pulsar::GenerateSinglePulse(unsigned long pulsewidth)
{
	set(!get());
	for(unsigned long i = 0; i < pulsewidth; i++)
	{
		__delay_cycles(CLOCK_DELAY);
	}
	set(!get());
}

void pulsar::GenerateClock(unsigned long pulsewidth, unsigned int clockcount )
{

	for(unsigned int j = 0; j < clockcount; j++)
	{
		set(!get());

		for(unsigned long i = 0; i < pulsewidth; i++)
		{
			__delay_cycles(CLOCK_DELAY);
		}
		set(!get());
		for(unsigned long i = 0; i < pulsewidth; i++)
		{
			__delay_cycles(CLOCK_DELAY);
		}
	}
}


void pulsar::GenerateSHRClock(unsigned long pulsewidth, unsigned int clockcount, pulsar sout, unsigned char* channels)
{

	for(unsigned int j = 0; j < clockcount; j++)
	{
		set(!get());

		for(unsigned long i = 0; i < pulsewidth; i++)
		{
			__delay_cycles(CLOCK_DELAY);
		}

		if(sout.get())
		{
			channels[j]=1;
		}
		else
			channels[j]=0;

		set(!get());
		for(unsigned long i = 0; i < pulsewidth; i++)
		{
			__delay_cycles(CLOCK_DELAY);
		}
	}
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	interruptVectorPORT2();
	 P2IFG &= ~0x08;
}

} /* namespace msp430 */
