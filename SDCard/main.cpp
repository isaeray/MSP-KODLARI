
#include "main.h"

using namespace MSP430;

void TS_Interrupt(void);

/*OUTPUTS*/
pulsar led((unsigned char*)&P8OUT, 0x01, 0);
pulsar TCLK((unsigned char*)&P8OUT, 4, 0);
pulsar READ((unsigned char*)&P8OUT, 2, 0);
pulsar SHRCLK((unsigned char*)&P8OUT, 1, 0);
pulsar SIN((unsigned char*)&P7OUT, 8, 0);
pulsar TIN((unsigned char*)&P7OUT, 4, 0);
pulsar CIN((unsigned char*)&P5OUT, 128, 0);
pulsar CSHIFT((unsigned char*)&P5OUT, 64, 0);
pulsar CS((unsigned char*)&P5OUT, 32, 1);		// initial 1
pulsar CLS((unsigned char*)&P8OUT, 16, 0);
pulsar ACQ((unsigned char*)&P8OUT, 32, 0);
pulsar ADCCLK((unsigned char*)&P2OUT, 4, 0);
pulsar ENABLE_5V((unsigned char*)&P2OUT, 8, 1);

/*INPUTS*/
pulsar TS1((unsigned char*)&P2OUT, 8, FallingEdge, &TS_Interrupt);
pulsar TS2((unsigned char*)&P9OUT, 128);
pulsar TOUT((unsigned char*)&P9OUT, 32);
pulsar SOUT((unsigned char*)&P9OUT, 16);
pulsar OVRFLOW((unsigned char*)&P9OUT, 8);
pulsar OTR((unsigned char*)&P7OUT, 32);
pulsar BITS[14] = {
		pulsar((unsigned char*)&P7OUT, 64),
		pulsar((unsigned char*)&P7OUT, 128),
		pulsar((unsigned char*)&P5OUT, 1),
		pulsar((unsigned char*)&P5OUT, 2),
		pulsar((unsigned char*)&P1OUT, 1),
		pulsar((unsigned char*)&P1OUT, 2),
		pulsar((unsigned char*)&P1OUT, 4),
		pulsar((unsigned char*)&P1OUT, 8),
		pulsar((unsigned char*)&P1OUT, 16),
		pulsar((unsigned char*)&P1OUT, 32),
		pulsar((unsigned char*)&P1OUT, 64),
		pulsar((unsigned char*)&P1OUT, 128),
		pulsar((unsigned char*)&P2OUT, 1),
		pulsar((unsigned char*)&P2OUT, 2)};

struct channelconfig ChannelConfigs[36];
struct channelconfig deneme;
UART_Mode CurrentUARTMode = NoOp;
unsigned int MeasurementPeriod = 0;
unsigned char ReceivedData[512]={};

void ADCCLOCK(void);
ADC adc;

Timer ADCACQTimer(TimerA0, &ADCACQTimerInterrupt);
Timer ADCTIMER(TimerB0, &ADCCLOCK);

rena RENA;

AD9243 ad9243(BITS, &ADCCLK, &OTR);

UART pc(UCA0, 115200, &CommandVector);

double volt = 0;


int main(void)
{
	__bis_SR_register(GIE);

ChannelConfigs[0].DF=0;
ChannelConfigs[0].DS=65;
ChannelConfigs[0].ECAL=1;
ChannelConfigs[0].ENF=0;
ChannelConfigs[0].ENS=1;
ChannelConfigs[0].FB_TC=0;
ChannelConfigs[0].FETSEL=0;
ChannelConfigs[0].FM=0;
ChannelConfigs[0].FPDWN=1;
ChannelConfigs[0].PDWN=0;
ChannelConfigs[0].POL=1;
ChannelConfigs[0].PZSEL=0;
ChannelConfigs[0].RANGE=0;
ChannelConfigs[0].RSEL=0;
ChannelConfigs[0].SEL=10;
ChannelConfigs[0].SIZEA=0;
ChannelConfigs[0].address=0;
ChannelConfigs[0].gainselect=3;


ChannelConfigs[1].DF=0;
ChannelConfigs[1].DS=65;
ChannelConfigs[1].ECAL=1;
ChannelConfigs[1].ENF=0;
ChannelConfigs[1].ENS=1;
ChannelConfigs[1].FB_TC=0;
ChannelConfigs[1].FETSEL=0;
ChannelConfigs[1].FM=0;
ChannelConfigs[1].FPDWN=1;
ChannelConfigs[1].PDWN=0;
ChannelConfigs[1].POL=1;
ChannelConfigs[1].PZSEL=0;
ChannelConfigs[1].RANGE=0;
ChannelConfigs[1].RSEL=0;
ChannelConfigs[1].SEL=10;
ChannelConfigs[1].SIZEA=0;
ChannelConfigs[1].address=35;
ChannelConfigs[1].gainselect=3;





//ADCTIMER.Start(1);

//RENA.ConfigureRena(ChannelConfigs,2,CIN,CSHIFT,CS);
while(1)
	{
	//RENA.ConfigureRena(ChannelConfigs,2,CIN,CSHIFT,CS);
led.set(1);
delay(1000);
led.set(0);
delay(1000);
	//ad9243.ReadVoltage(&volt);
		delay(100);
		UARTCommandHandler(CurrentUARTMode);
	}



}

void delay(unsigned int wait)
{
	for(unsigned int i = 0; i < wait; i++)
	{
		__delay_cycles(CLOCK_DELAY);
	}
}



void CommandVector(unsigned char Phase, unsigned char* Data, unsigned int Length)
{
	switch(Phase)
		{
		case 2:
			CurrentUARTMode = Success;
			for(int i = 0; i < Length - 3; i++)
			{
				ReceivedData[i] = Data[i];
			}
			break;
		case 10:
			CurrentUARTMode = HeaderError;
			break;
		case 11:
			CurrentUARTMode = DataError;
			break;
		}
}

void UARTCommandHandler(UART_Mode mode)
{
	switch(mode)
	{
	case Success:
		/*Connect Command*/
		if(CompareCharArrayToString(ReceivedData, "Connect", 7))
		{
			pc.Send(AssembleDataPacket((unsigned char*)"OK", 2), 12);
		}
		/*************************************************************/

		/*Connect Command*/
		else if(CompareCharArrayToString(ReceivedData, "GetGPIO", 7))
		{
			int index=7;
			ReceivedData[index++]=CLS.get();
			ReceivedData[index++]=ACQ.get();
			ReceivedData[index++]=TCLK.get();
			ReceivedData[index++]=READ.get();
			ReceivedData[index++]=SHRCLK.get();
			ReceivedData[index++]=SIN.get();
			ReceivedData[index++]=TIN.get();
			ReceivedData[index++]=CIN.get();
			ReceivedData[index++]=CSHIFT.get();
			ReceivedData[index++]=CS.get();

			ReceivedData[index++]=TS1.get();
			ReceivedData[index++]=TS2.get();
			ReceivedData[index++]=TOUT.get();
			ReceivedData[index++]=SOUT.get();
			ReceivedData[index++]=OVRFLOW.get();

			pc.Send(AssembleDataPacket(ReceivedData, index), index+10);
		}
		/*************************************************************/
		/*Get All ADC Command*/
		else if(CompareCharArrayToString(ReceivedData, "GetAllADC", 9))
		{
			unsigned char PACKET[41];
			for(int i = 0; i < 9; i++)
			{
				PACKET[i] = ReceivedData[i];
			}
			unsigned char TempValues[32];
			adc.ReadAllValuesToCharArray(TempValues);
			for (int i = 9; i < 41; i++)
			{
				PACKET[i] = TempValues[i - 9];
			}
			pc.Send(AssembleDataPacket(PACKET, 41), 51);
		}
		/****************************************************************/
		/*Start ADC Acquisition Command*/
		else if(CompareCharArrayToString(ReceivedData, "StartADCACQ", 11))
		{
			MeasurementPeriod = ReceivedData[11] + ReceivedData[12] * 256;
			ADCACQTimer.Start(MeasurementPeriod);
		}
		/*****************************************************************/
		/*Stop ADC Acquisition Command*/
		else if(CompareCharArrayToString(ReceivedData, "StopADCACQ", 10))
		{
			ADCACQTimer.Stop();
		}
		/*****************************************************************/
		/*Send Channel Config Command*/
		if(CompareCharArrayToString(ReceivedData, "SendChannelConfig", 17))
		{


				for(int i = 0; i < 36; i++)
				{
					ChannelConfigs[i].FM = ( ( ReceivedData[6*i+17] & 0x01 ) );
					ChannelConfigs[i].ENS = ( ( ReceivedData[6*i+17] & 0x02 ) >> 1 );
					ChannelConfigs[i].ENF = ( ( ReceivedData[6*i+17] & 0x04 ) >> 2 );
					ChannelConfigs[i].DS = ( ( ReceivedData[6*i+17] & 0xF8 ) >> 3 ) | ( (ReceivedData[6*i+18] & 0x07) << 5);
					ChannelConfigs[i].POL = ( ( ReceivedData[6*i+18] & 0x08 ) >> 3 );
					ChannelConfigs[i].DF = ( ( ReceivedData[6*i+18] & 0xF0 ) >> 4) | ((ReceivedData[6*i+19] & 0x0F) << 4);
					ChannelConfigs[i].SIZEA =  ( ( ReceivedData[6*i+19] & 0x10 ) >> 4 );
					ChannelConfigs[i].SEL = ( ( ReceivedData[6*i+19] & 0xE0 ) >> 5 ) | ( ( ReceivedData[6*i+20] & 0x01 ) << 3 ) ;
					ChannelConfigs[i].RSEL = ( ( ReceivedData[6*i+20] & 0x02 ) >> 1 );
					ChannelConfigs[i].RANGE = ( ( ReceivedData[6*i+20] & 0x04 ) >> 2 );
					ChannelConfigs[i].PZSEL = ( ( ReceivedData[6*i+20] & 0x08 ) >> 3 );
					ChannelConfigs[i].PDWN = ( ( ReceivedData[6*i+20] & 0x10 ) >> 4 );
					ChannelConfigs[i].gainselect = ( ( ReceivedData[6*i+20] & 0x60 ) >> 5 );
					ChannelConfigs[i].FETSEL = ( ( ReceivedData[6*i+20] & 0x80 ) >>7 );
					ChannelConfigs[i].FPDWN= ( ( ReceivedData[6*i+21] & 0x01 ));
					ChannelConfigs[i].ECAL = ( ( ReceivedData[6*i+21] & 0x02 ) >> 1);
					ChannelConfigs[i].FB_TC = ( ( ReceivedData[6*i+21] & 0x04 ) >> 2 );
					ChannelConfigs[i].address = ( ( ReceivedData[6*i+21] & 0xF8 ) >> 3 ) | ( ( ReceivedData[6*i+22] & 0x01 ) << 5 );
				}

				RENA.ConfigureRena(ChannelConfigs, 36, CIN, CSHIFT, CS);
			pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
		}
		/*************************************************************/


		CurrentUARTMode = NoOp;
		break;
	case HeaderError:
		pc.Send((unsigned char*)"HeaderError", 11);
		CurrentUARTMode = NoOp;
		break;
	case DataError:
		pc.Send((unsigned char*)"DataError", 9);
		CurrentUARTMode = NoOp;
		break;
	case NoOp:
		delay(1);
		break;
	}
}

unsigned int value;
void ADCCLOCK(void)
{
	value = 0;
	ADCCLK.set(1);
	for(int i=0; i<14; i++)
	{
		value += BITS[i].get() * (0x01 << (13 - i));
	}
	ADCCLK.set(0);
	volt = ((double) value / 16383)*2 - 1;
	//ADCCLK.set(!ADCCLK.get());
}

void ADCACQTimerInterrupt()
{
    unsigned char PACKET[41];
    unsigned char* Command = (unsigned char*)"GetAllADC";
    for(int i = 0; i < 9; i++)
    {
        PACKET[i] = Command[i];
    }
    unsigned char TempValues[32];
    adc.ReadAllValuesToCharArray(TempValues);
    for (int i = 9; i < 41; i++)
    {
        PACKET[i] = TempValues[i - 9];
    }
    pc.Send(AssembleDataPacket(PACKET, 41), 51);
}

void TS_Interrupt(void)
{
	led.set(!led.get());

}













