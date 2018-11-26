/* 
 * "Small Hello World" example. 
 * 
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example 
 * designs. It requires a STDOUT  device in your system's hardware. 
 *
 * The purpose of this example is to demonstrate the smallest possible Hello 
 * World application, using the Nios II HAL library.  The memory footprint
 * of this hosted application is ~332 bytes by default using the standard 
 * reference design.  For a more fully featured Hello World application
 * example, see the example titled "Hello World".
 *
 * The memory footprint of this example has been reduced by making the
 * following changes to the normal "Hello World" example.
 * Check in the Nios II Software Developers Manual for a more complete 
 * description.
 * 
 * In the SW Application project (small_hello_world):
 *
 *  - In the C/C++ Build page
 * 
 *    - Set the Optimization Level to -Os
 * 
 * In System Library project (small_hello_world_syslib):
 *  - In the C/C++ Build page
 * 
 *    - Set the Optimization Level to -Os
 * 
 *    - Define the preprocessor option ALT_NO_INSTRUCTION_EMULATION 
 *      This removes software exception handling, which means that you cannot 
 *      run code compiled for Nios II cpu with a hardware multiplier on a core 
 *      without a the multiply unit. Check the Nios II Software Developers 
 *      Manual for more details.
 *
 *  - In the System Library page:
 *    - Set Periodic system timer and Timestamp timer to none
 *      This prevents the automatic inclusion of the timer driver.
 *
 *    - Set Max file descriptors to 4
 *      This reduces the size of the file handle pool.
 *
 *    - Check Main function does not exit
 *    - Uncheck Clean exit (flush buffers)
 *      This removes the unneeded call to exit when main returns, since it
 *      won't.
 *
 *    - Check Don't use C++
 *      This builds without the C++ support code.
 *
 *    - Check Small C library
 *      This uses a reduced functionality C library, which lacks  
 *      support for buffering, file IO, floating point and getch(), etc. 
 *      Check the Nios II Software Developers Manual for a complete list.
 *
 *    - Check Reduced device drivers
 *      This uses reduced functionality drivers if they're available. For the
 *      standard design this means you get polled UART and JTAG UART drivers,
 *      no support for the LCD driver and you lose the ability to program 
 *      CFI compliant flash devices.
 *
 *    - Check Access device drivers directly
 *      This bypasses the device file system to access device drivers directly.
 *      This eliminates the space required for the device file system services.
 *      It also provides a HAL version of libc services that access the drivers
 *      directly, further reducing space. Only a limited number of libc
 *      functions are available in this configuration.
 *
 *    - Use ALT versions of stdio routines:
 *
 *           Function                  Description
 *        ===============  =====================================
 *        alt_printf       Only supports %s, %x, and %c ( < 1 Kbyte)
 *        alt_putstr       Smaller overhead than puts with direct drivers
 *                         Note this function doesn't add a newline.
 *        alt_putchar      Smaller overhead than putchar with direct drivers
 *        alt_getchar      Smaller overhead than getchar with direct drivers
 *
 */

#include "sys/alt_stdio.h"
#include "stdio.h"
#include "altera_avalon_uart_regs.h"
#include <string.h>
#include <unistd.h>
#define UART_0_BASE 0x11020


void writenUart(char vetor[] , int tamanho );
void readUart();
void sendData(int escolha);
void writenUartQuick(char vetor , int tamanho);

int main()
{
  char comandoAT1[] = "AT";
  char comandoAT2[] = "AT+RST";
  char comandoAT3[] = "AT+CWJAP=\"WLessLEDS\",\"HelloWorldMP31\"";
  char comandoAT5[] = "AT+CIPSTART=\"TCP\",\"192.168.1.103\",1883";


  writenUart(comandoAT1 , strlen(comandoAT1));
  readUart();
  usleep(300000);

  writenUart(comandoAT3 , strlen(comandoAT3));
  readUart();
  usleep(300000);

  writenUart(comandoAT5 , strlen(comandoAT5) );
  readUart();
  usleep(300000);
  sendData(1);

  /* Event loop never exits. */
  while (1){
	  alt_putstr("Funciona");
	  break;
  }

  return 0;
}

void writenUart(char vetor[] , int tamanho ){
	unsigned long uartStatus = 0;
	//verifica até que possa ser feita a transmissão de dados.
	while((uartStatus & 0x00000040) != 0x00000040){
		uartStatus = IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE);
	}
	int i;
	for(i = 0 ; i < tamanho ; i++){
		IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, vetor[i]);
		usleep(1000);
	}
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, '\r');
	usleep(1000);
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, '\n');
}

void writenUartQuick(char vetor , int tamanho){
	unsigned long uartStatus = 0;
	//verifica até que possa ser feita a transmissão de dados.
	while((uartStatus & 0x00000040) != 0x00000040){
		uartStatus = IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE);
	}
	int i;
	for(i = 0 ; i < tamanho ; i++){
		IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, vetor);
		usleep(1000);
	}
}

void readUart(){
   char check;
   while(1){
	   if(IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE) & 0x80){
		   check = IORD_ALTERA_AVALON_UART_RXDATA(UART_0_BASE);
		   printf("%c",check);
		   if(check == 'K'){
		   	  break;
		   }
	   }
   }
}

void sendData(int escolha){
	int z;
	char comandoSendC[] = "AT+CIPSEND=19";
	//pacote controle 0x\10\11\00\04\MQTT\04\02\00\14\00\05\Nios2
	char console[] = {0x10 , 0x11 , 0x00 , 0x04 , 0x4d , 0x51 ,0x54 ,0x54, 0x04 , 0x02 , 0x00 , 0x14 , 0x00 , 0x06 , 0x4d , 0x51 , 0x54 , 0x54 , 0x32};

	//pacote disconnect
	char disconnect[] = {0xe0 , 0x00};
	char comandoD[] = "AT+CIPSEND=2";

	//pacote publisher1 0x\30\18\00\0c\teste\teste1\HelloWorld
	//(1)topico teste/teste1 mensagem HelloWorld
	char publish1[] = {0x30, 0x18, 0x00, 0x0C,
				0x74, 0x65, 0x73, 0x74, 0x65, 0x2f, 0x74, 0x65, 0x73, 0x74, 0x65, 0x31,
				0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x57, 0x6F, 0x72, 0x6C, 0x64};

	//publish 0x\30\0b\00\03\PBL\Opcao1
	//(2) topico PBL mensagem opcao1
	char publish[] = {0x30 , 0x0b , 0x00 , 0x03 ,
				0x50 , 0x42 ,0x4c,
				0x4f , 0x50 , 0x43 , 0x41 , 0x4f , 0x32};

	//(1) - 26 (2) - 12
	// a depender do pacote deve mudar para os valores que estao em cima
	char comandoSendP[] = "AT+CIPSEND=26";
	writenUart(comandoSendC , strlen(comandoSendC)); //"AT+CIPSEND=19"
	for(z = 0 ; z < sizeof(console); z++){
		printf("%c", console[z]);
		writenUartQuick(1, console[z]);
	}
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, '\r');
	usleep(1000);
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, '\n');

	writenUart(comandoSendP,strlen(comandoSendP)); //"AT+CIPSEND=26" ou 12
	switch(escolha){
		case 1:
			for(z = 0 ; z < sizeof(publish1); z++){
				printf("%c", publish1[z]); // pode testar com publish ou publish1
				writenUartQuick(1, publish1[z]);
			}
			IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, '\r');
			usleep(1000);
			IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, '\n');
			break;
		case 2:
			//writenUart(comandoSend , strlen(comandoSend) , 2); //AT+CIPSEND=
			//writenUart(publish2,strlen(publish2) );
			//usleep(1000);
			break;
		case 3:
			//writenUart(comandoSend , strlen(comandoSend) , 2); //AT+CIPSEND=
			//writenUart(publish3,strlen(publish3));
			//usleep(1000);
			break;
		case 4:
			//writenUart(comandoSend , strlen(comandoSend) , 2); //AT+CIPSEND=
			//writenUart(publish4,strlen(publish4));
			//usleep(1000);
			break;
		case 5:
			//writenUart(comandoSend , strlen(comandoSend) , 2); //AT+CIPSEND=
			//writenUart(publish5,strlen(publish5));
			//usleep(1000);
			break;
	}
	for(z = 0 ; z < sizeof(disconnect); z++){
			printf("%c", disconnect[z]);
			writenUartQuick(1, disconnect[z]);
		}
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, '\r');
	usleep(1000);
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, '\n');
	usleep(1000);
}






