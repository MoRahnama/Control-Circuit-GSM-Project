/*
 * Virtual Terminal.c
 *
 * Created: 11/29/2015 10:42:57 AM
 *  Author: MohammadAli
 */


#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTD2
#define EN eS_PORTD3

#include <avr/io.h>
#include <util/delay.h>
#include <avr/xmega.h>
#include <string.h>
#include <avr/interrupt.h>
#include "SIM900.h"
#include "lcd.h"


int main(void)
{
	
	DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) ;
	char buffer[20];
	int k;
	Lcd4_Init();
	UART_ON(9600,8);
	
	/*
	Lcd4_Set_Cursor(1,1);
	Lcd4_Write_String("electroSome LCD Hello World");
	Lcd4_Clear();
	Lcd4_Write_Char('e');
	*/
	
	 Lcd4_Clear();
	 Lcd4_Set_Cursor(1,5);
	 Lcd4_Write_String("Welcome");
	 _delay_ms(500);
	 Lcd4_Set_Cursor(2,0);
	 Lcd4_Write_String("Loading");
	 
	 for (k=0;k<101;k+=1)
	 {
		 Lcd4_Set_Cursor(2,12);
		 sprintf(buffer,"%d",k);
		 Lcd4_Write_String(buffer);
		 Lcd4_Set_Cursor(2,15);
		 Lcd4_Write_String("%");
		 _delay_ms(200);
	 }
	 _delay_ms(3000);
	 Lcd4_Clear();
	
	
	DDRB  = 0xFF ;
	PORTB = 0xFF ;
	
	
	// Prescaler = FCPU/1024
	TCCR0|=(1<<CS02)|(1<<CS00);

	//Enable Overflow Interrupt Enable
	TIMSK|=(1<<TOIE0);

	//Initialize Counter
	TCNT0=0;
	
	ADMUX=ADC_VREF_TYPE & 0xff;
	ADCSRA=0x83;
	

	//---------------------Send Module ON Notification to Admin ------------------------------
	
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("Module ON");
	_delay_ms(1000);
	Lcd4_Clear();
	
	GSM_Send("09300093455","Hello Admin, Module is on .");
	
	//-----------------------------------------------------------------------------------------

	//---------------------GPRS Connection Settings -------------------------------------------
	
	//	GPRS_Connect();
		
	//-----------------------------------------------------------------------------------------
	
	//---------------------FTP Conection Setting ----------------------------------------------
	
	//	FTP_connect();
		
	//-----------------------------------------------------------------------------------------
	//---------------------Downloading Chenges.txt ---------------------------------------------
	
	//    Download_start();
	
	//----------------------------------------------------------------------------------------
	//---------------------Uploading Information----------------------------------------------
	
	
	//Upload_start("Wi-Fi ON -- Alarms OFF -- Lights OFF -- Cooler OFF -- RoomTemprature 30'c -- OutsideTemprature 45'c -- UVSensor ON","114");


	//----------------------------------------------------------------------------------------
	//-------------------------Dial A Number--------------------------------------------------
	
	//	Dial_Number("+989300093455");
		
	//-----------------------------------------------------------------------------------------
	
	Lcd4_Clear();
	while (1)
	{
		
	}
}
