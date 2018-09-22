#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


// Headers

#define F_CPU 4915200
#include <util/delay.h>

//--- Proto Type Declaration ---//

void uart_init(void);      //--- Usart Initialize



void SRAM_test()
{
	volatile char *ext_ram = (char *) 0x1800; // Start address for the SRAM
	uint16_t ext_ram_size = 0x800;
	uint16_t write_errors = 0;
	uint16_t retrieval_errors = 0;
	printf("Starting SRAM test...\n");
	// rand() stores some internal state, so calling this function in a loop will
	// yield different seeds each time (unless srand() is called before this function)
	uint16_t seed = rand();
	// Write phase: Immediately check that the correct value was stored
	srand(seed);
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		ext_ram[i] = some_value;
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			printf("Write phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
			write_errors++;
		}
	}
	// Retrieval phase: Check that no values were changed during or after the write phase
	srand(seed); // reset the PRNG to the state it had before the write phase
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			printf("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
			retrieval_errors++;
		}
	}
	printf("SRAM test completed with \n%4d errors in write phase and \n%4d errors in retrieval phase\n\n", write_errors, retrieval_errors);
}


//--- Main Program ---//

int main()
{	
	
	MCUCR = (1 << SRE);					//external memory enable
	//EMCUCR = (1 << SRW11);
	uart_init();						//UART initialization
	SRAM_test();						
	/*
	while(1)
	{
		
		transmitByte('8');
		printf("Hello this is a ..");
		//uart_msg("8");
		_delay_ms(1000);
	}
	*/

}


// SRAM


void transmitByte(uint8_t data)
{
	loop_until_bit_is_set(UCSR0A,UDRE0) ;
	UDR0 = data;
}

uint8_t receiveByte(void)
{
	loop_until_bit_is_set(UCSR0A,RXC0);
	return UDR0;
}

void printString(const char myString[])
{
	uint8_t i = 0;
	while(myString[i])
	{
		transmitByte(myString[i]);
		i++;
	}
}

void uart_init()
{
	UBRR0H=0x00;
	UBRR0L=0x1F; // To set the Buad rate to 9600.. get value through above formulaF
	UCSR0B=(1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);
	UCSR0C=(1<<URSEL0) | (1<<UCSZ01) | (1<<UCSZ00) ;
	fdevopen(transmitByte, receiveByte);
}
