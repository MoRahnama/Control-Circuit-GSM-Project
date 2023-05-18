/*
 * SIM900.c
 *
 * Created: 4/7/2016 7:37:56 PM
 *  Author: Mohammad Ali
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/xmega.h>
#include <string.h>
#include <avr/interrupt.h>
#include "Temp_Sensor.h"

volatile uint8_t count;


#define WRITE_PORTB_BIT0(x)  (PORTB = (PORTB & ~_BV(PB0)) | ((x) << PB0))
#define WRITE_PORTB_BIT1(x)  (PORTB = (PORTB & ~_BV(PB1)) | ((x) << PB1))
#define WRITE_PORTB_BIT2(x)  (PORTB = (PORTB & ~_BV(PB2)) | ((x) << PB2))
#define WRITE_PORTB_BIT3(x)  (PORTB = (PORTB & ~_BV(PB3)) | ((x) << PB3))
#define WRITE_PORTB_BIT4(x)  (PORTB = (PORTB & ~_BV(PB4)) | ((x) << PB4))


const char chkmsg1[] = {"+CMT: \"+989300093455\""};
char chkmsg2[50];

const char msg1[]={"ATE0\r"};
const char msg2[]={"AT+CMGF=1\r"};
char msg3[30];
char msg4[50];
char Call[50];

const char Gprs1[]={"AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r"};
const char Gprs2[]={"AT+SAPBR=3,1,\"APN\",\"www\"\r"};
const char Gprs3[]={"AT+SAPBR=1,1\r"};
const char Gprs4[]={"AT+SAPBR=2,1\r"};

const char Ftp1[]={"AT+FTPCID=1\r"};
const char Ftp2[]={"AT+FTPSERV=\"ftp.marahnama.ir\"\r"};
const char Ftp3[]={"AT+FTPUN=\"SIM900@marahnama.ir\"\r"};
const char Ftp4[]={"AT+FTPPW=\"admin123\"\r"};


const char Upld1[]={"AT+FTPPUTNAME=\"Status.txt\"\r"};
const char Upld2[]={"AT+FTPPUTPATH=\"/\"\r"};
const char Upld3[]={"AT+FTPPUT=1\r"};
const char Upld4[]={"AT+FTPPUT=2,50\r"};
const char Upld6[]={"AT+FTPPUT=2,0\r"};

const char Dwnld1[]={"AT+FTPGETNAME=\"Changes.txt\"\r"};
const char Dwnld2[]={"AT+FTPGETPATH=\"/\"\r"};
const char Dwnld3[]={"AT+FTPGET=1\r"};
const char Dwnld4[]={"AT+FTPGET=2,1024\r"};

const char DoubleQuote = 34;
const char Enter = 13;
const char CTRLZ = 26;
const char Dial = 59;

int Temperature;
char Information[50];

char Recieve ;
char Changes[100];
char Temp[];
int i = 0;
int j = 0;
char Flag;
char TA = '1';char TB = '1';char TC = '1';char TD = '1';
char* TTA ;char* TTB ;char* TTC ;char* TTD ;

void UART_ON(unsigned int boud,unsigned char fosc)

{
	unsigned char x;
	x=fosc/(0.000064*boud);
	x-=1;
	UBRRL=x;
	UBRRH=x>>8;
	UCSRB |=(1 << TXEN) | (1 << RXEN) | (1 << RXC);
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1) ;
	
	sei();
}


void Tx_putc(char send)
{
	while ((UCSRA & (1 << UDRE)) == 0);
	UDR = send;
}

ISR(USART_RXC_vect)
{
	Recieve = UDR;
	Temp[i] = Recieve;
	i++;
}

void Tx_puts(unsigned char name[])
{
	unsigned char z=0;
	for(z=0;z<strlen(name);z++)
	{
		while ((UCSRA & (1 << UDRE)) == 0);
		UDR=name[z];
	}
	return;
}

ISR(TIMER0_OVF_vect)
{
	count++;
	if(count == 30)
	{
		Lcd4_Write_String("END OF TIMER");
		_delay_ms(1000);
		Lcd4_Clear();
		
		char *ptr = Temp;
		char *ptr1 = Temp;

		char *prevptr = NULL;
		char *prevptr1 = NULL;

		while( (ptr = strstr(ptr,chkmsg1)))
		{
			prevptr = ptr++;
		}
		
		while( (ptr1 = strstr(ptr1,"Changes")))
		{
			prevptr1 = ptr1++;
		}
		
		sprintf (chkmsg2, "%s", prevptr);
		if (chkmsg2[49] == 'C' && chkmsg2[50] == 'M' && chkmsg2[51] == 'D')
		{
			showResult(chkmsg2[53],chkmsg2[54],chkmsg2[55],chkmsg2[56]);
			i=0;
			count=0;
			return;
		}
		
		if (chkmsg2[49] == 'D' && chkmsg2[50] == 'W' && chkmsg2[51] == 'N' && chkmsg2[52] == 'L'&& chkmsg2[53] == 'D')
		{
			GPRS_Connect();
			FTP_connect();
			Download_start();
			i=0;
			count=0;
			return;
		}
		
		if (chkmsg2[49] == 'U' && chkmsg2[50] == 'P' && chkmsg2[51] == 'L' && chkmsg2[52] == 'D')
		{
			GPRS_Connect();
			FTP_connect();
			Upload_start();
			i=0;
			count=0;
			return;
		}
		
		if (chkmsg2[49] == 'I' && chkmsg2[50] == 'N' && chkmsg2[51] == 'F' && chkmsg2[52] == 'O')
		{
			Temperature = read_adc(0);
			Temperature = Temperature/2 - 352;
			sprintf(Information,"LED1 = %c , LED2 = %c , LED3 = %c , LED4 = %c  , Temperature is %d ",TA,TB,TC,TD,Temperature);
			GSM_Send("09300093455",Information);
			i=0;
			count=0;
			return;
		}
		
		sprintf (Changes, "%s", prevptr1);
		if (Changes[10] == 'C' && Changes[11] == 'M' && Changes[12] == 'D')
		{
			showResult(Changes[14],Changes[15],Changes[16],Changes[17]);
			i=0;
			count=0;
			return;
		}
		
		i=0;
		count=0;
	}
	
}

void showResult (char A, char B , char C, char D)
{
	if (A != TA || B != TB || C != TC || D != TD)
	{
		if (A == '1')
		{
			WRITE_PORTB_BIT0(1);
		}
		if (A == '0')
		{
			WRITE_PORTB_BIT0(0);
		}
		if (B == '1')
		{
			WRITE_PORTB_BIT1(1);
		}
		if (B == '0')
		{
			WRITE_PORTB_BIT1(0);
		}
		if (C == '1')
		{
			WRITE_PORTB_BIT2(1);
		}
		if (C == '0')
		{
			WRITE_PORTB_BIT2(0);
		}
		if (D == '1')
		{
			WRITE_PORTB_BIT3(1);
		}
		if (D == '0')
		{
			WRITE_PORTB_BIT3(0);
		}
		
		GSM_Send("09300093455","New Changes Are Made.");
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Changes Are Made");
		_delay_ms(1000);
		Lcd4_Clear();
	}
	
	TA = A;
	TB = B;
	TC = C;
	TD = D;
	return ;
}
void GSM_Send (char Number[],char Messsge[])
{
	//---------------------GSM Send A Message ---------------------------------------------
	
	sprintf(msg3,"AT+CMGS=%c%s%c%c",DoubleQuote,Number,DoubleQuote,Enter);
	sprintf(msg4,"%s%c",Messsge,CTRLZ);
	
	Tx_puts(msg1);
	_delay_ms(500);
	Tx_puts(msg2);
	_delay_ms(500);
	Tx_puts(msg3);
	_delay_ms(500);
	Tx_puts(msg4);
	Tx_putc(Enter);
	_delay_ms(2000);

	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Message Sent");
	_delay_ms(1000);
	Lcd4_Clear();
	return;
	//----------------------------------------------------------------------------------------
}

void GPRS_Connect()
{
	
	//---------------------GPRS Initialization--------------------------------------------------------

	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("GPRS Connection");

	Tx_puts("AT\r");
	_delay_ms(500);
	Tx_puts("AT\r");
	_delay_ms(500);
	Tx_puts("AT\r");
	_delay_ms(500);
	Tx_puts(Gprs1);
	_delay_ms(500);
	Tx_puts(Gprs2);
	_delay_ms(500);
	Tx_puts(Gprs3);
	_delay_ms(5000);
	Tx_puts(Gprs4);
	_delay_ms(500);
	
	Lcd4_Clear();
	
	return;
	//-----------------------------------------------------------------------------------------------------

}

void FTP_connect()
{
	//---------------------FTP Initialization----------------------------------------------------------
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("FTP Connection");

	
	Tx_puts("AT\r");
	_delay_ms(500);
	Tx_puts("AT\r");
	_delay_ms(500);
	Tx_puts(Ftp1);
	_delay_ms(500);
	Tx_puts(Ftp2);
	_delay_ms(500);
	Tx_puts(Ftp3);
	_delay_ms(500);
	Tx_puts(Ftp4);
	_delay_ms(500);
	
	Lcd4_Clear();
	
	return;
	//---------------------------------------------------------------------------------------------------
}

void Download_start()
{
	
	//---------------------Download Initialization----------------------------------------------------------

	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Downloading ...");
	
	Tx_puts("AT\r");
	_delay_ms(500);
	Tx_puts("AT\r");
	_delay_ms(500);
	Tx_puts(Dwnld1);
	_delay_ms(500);
	Tx_puts(Dwnld2);
	_delay_ms(500);
	Tx_puts(Dwnld3);
	_delay_ms(50000);
	Tx_puts(Dwnld4);
	_delay_ms(10000);
	
	//sprintf (Changes, "%s", strstr (Temp, "Changes"));
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Download Complete");
	_delay_ms(1000);
	Lcd4_Clear();
	
	return;
	
	//----------------------------------------------------------------------------------------
}

void Upload_start()
{
	//---------------------Upload Initialization----------------------------------------------
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Uploading ...");
	
	
	Temperature = read_adc(0);
	Temperature = Temperature/2 - 352;
	
	switch (TA)
	{
		case '1':
		TTA = "ON";
		break;
		case '0':
		TTA = "OFF";
		break;
	}
	
	switch (TB)
	{
		case '1':
		TTB = "ON";
		break;
		case '0':
		TTB = "OFF";
		break;
	}
	switch (TC)
	{
		case '1':
		TTC = "ON";
		break;
		case '0':
		TTC = "OFF";
		break;
	}
	switch (TD)
	{
		case '1':
		TTD = "ON";
		break;
		case '0':
		TTD = "OFF";
		break;
	}

	sprintf(Information,"LED1 %s LED2 %s LED3 %s LED4 %s Temperature %d      ",TTA,TTB,TTC,TTD,Temperature);
	
	Tx_puts("AT\r");
	_delay_ms(500);
	Tx_puts("AT\r");
	_delay_ms(500);
	Tx_puts(Upld1);
	_delay_ms(500);
	Tx_puts(Upld2);
	_delay_ms(500);
	Tx_puts(Upld3);
	_delay_ms(30000);
	Tx_puts(Upld4);
	_delay_ms(500);
	Tx_puts(Information);
	_delay_ms(500);
	Tx_puts(Upld6);
	_delay_ms(500);
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Upload Complete");
	_delay_ms(1000);
	Lcd4_Clear();
	
	return;
	//----------------------------------------------------------------------------------------
}

void Dial_Number(char Number[])
{
	//-------------------------Dial A Number--------------------------------------------------
	
	
	sprintf(Call,"ATD+%s%c%c",Number,Dial,Enter);
	
	Tx_puts(Call);
	_delay_ms(500);
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Dialling ... ");
	_delay_ms(1000);
	Lcd4_Clear();
	
	
	//-----------------------------------------------------------------------------------------
}

