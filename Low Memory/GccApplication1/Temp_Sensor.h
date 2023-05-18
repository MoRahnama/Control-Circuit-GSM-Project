/*
 * Temp_Sensor.h
 *
 * Created: 4/8/2016 3:17:49 PM
 *  Author: Mohammad Ali
 */ 

#define ADC_VREF_TYPE 0x40


// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
{
	ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
	// Delay needed for the stabilization of the ADC input voltage
	_delay_us(10);
	// Start the AD conversion
	ADCSRA|=0x40;
	// Wait for the AD conversion to complete
	while ((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	return ADCW;
}