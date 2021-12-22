// 'C' source line config statements  
#include <xc.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <stdint.h> 
// #pragma config statements should precede project file includes.  
// Use project enums instead of #define for ON and OFF.  
// CONFIG1  
#pragma config FOSC = INTOSC // Oscillator Selection (INTOSCoscillator: I/O function on CLKIN pin)  
#pragma config WDTE = OFF // Watchdog Timer Enable (WDT disabled)  
#pragma config PWRTE = OFF // Power-up Timer Enable (PWRT disabled)  
#pragma config MCLRE = ON // MCLR Pin Function Select (MCLR/VPP pinfunction is MCLR)  
#pragma config CP = OFF // Flash Program Memory Code Protection(Program memory code protection is disabled)  
#pragma config CPD = OFF // Data Memory Code Protection (Datamemory code protection is disabled)  
#pragma config BOREN = OFF // Brown-out Reset Enable (Brown-out Resetdisabled)  
#pragma config CLKOUTEN = OFF // Clock Out Enable (CLKOUT function isdisabled. I/O or oscillator function on the CLKOUT pin)  
#pragma config IESO = OFF // Internal/External Switchover(Internal/External Switchover mode is disabled)  
#pragma config FCMEN = OFF // Fail-Safe Clock Monitor Enable (FailSafeClock Monitor is disabled)  
// CONFIG2  
#pragma config WRT = OFF // Flash Memory Self-Write Protection(Write protection off)  
#pragma config PLLEN = OFF // PLL Enable (4x PLL disabled)  
#pragma config STVREN = ON // Stack Overflow/Underflow Reset Enable(Stack Overflow or Underflow will cause a Reset)  
#pragma config BORV = LO // Brown-out Reset Voltage Selection(Brown-out Reset Voltage (Vbor), low trip point selected.)  
#pragma config LVP = ON // Low-Voltage Programming Enable (Highvoltageon MCLR/VPP must be used for programming)  
  
//These are the values to set baud rate for UART and configure some functions
#define BAUD 9600  
#define FOSC 4000000L  
#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1)) // Should be 25 for9600/4MhZ  
#define NINE_BITS 0  
#define SPEED 0x4  
#define RX_PIN TRISC5  
#define TX_PIN TRISC4  
#define _XTAL_FREQ 4000000.0 /*for 4mhz*/  

  
//*Function Prototypes*//  
void RA5Blink(void);  
void pinConfig(void);  
void setup_comms(void);  
void putch(unsigned char);  
unsigned char getch(void);  
unsigned char getche(void);  
int decode(void);
void pattern1(void);
void pattern2(void);
void pattern3(void);
void pattern4(void);
void pattern5(void);
void pattern6(void);
void pattern7(void);
void pattern8(void);
void pattern9(void);
  
//*Global Vars*//  
uint32_t codearr;
int delaycount = 0;
int pausecount = 0;
int trackcount = 0;
int counting = 0;


uint16_t address;
uint8_t command, inv_command;


void interrupt ExternalPin()
{
/*************** start external interrupt ISR ***************/

//begin interrupt 
  uint8_t count = 0, i;
  IOCBN = 0x00;

  // Check 9ms pulse (remote control sends logic high)
  while((!RB7) && (count < 200)){
    count++; 
    __delay_us(50);
  }
    if( (count > 199) || (count < 135)) {       // NEC protocol?
         IOCBF = 0x00;// clear port b flag
         IOCBN7 = 1;
        return ;      
    }
  count = 0;
  // Check 4.5ms space (remote control sends logic low)
  while((RB7) && (count < 100)){
    count++;
    __delay_us(50);}
    if( (count > 101) || (count < 60))  {        // NEC protocol?
         IOCBF = 0x00;// clear port b flag
         IOCBN7 = 1;        
        return ;                         
    }
  // Read code message (32-bit)
  for(i = 0; i < 32; i++){
    count = 0;
    while((RB7 == 0) && (count < 15)){
      count++;
      __delay_us(50);}
    if( (count > 14) || (count < 5))   {   // NEC protocol?
        return ;    
    }
    count = 0;
    while(RB7 && (count < 40)){
      count++; 
      __delay_us(50);}
    if( (count > 40) || (count < 5))  {    // NEC protocol?
         IOCBF = 0x00;// clear port b flag
         IOCBN7 = 1;       
        return ;
    }
    if( count > 25)    {                   // If space width > 1ms
      codearr |= (0b1 << (31-i));         // Write 1 to bit (31 - i)
    }
    else        {                          // If space width < 1ms
      codearr &= ~(0b0 << (31-i));       // Write 0 to bit (31 - i)
    }
  }
           IOCBF = 0x00;// clear port b flag
         IOCBN7 = 1;   
  return ;
  }

void main(void) {  
  
 pinConfig();  
// GIE = 0;
setup_comms();// set up the USART - settings defined in usart.h  
//uint8_t command, address;
//IOCBN = 0x20;
//printf("started");
GIE=1;
 __delay_ms(1000);  
 RA5 = 1;
 RA2 = 1;
 //IOCBN = 0x20;
 uint32_t codehold;
 int lastvalid;
 while (1)  {
     __delay_ms(1000);
     printf("Full code: %X", codearr);
     if(codearr == 0){
         codehold = codehold;
     }
     else {
         codehold=codearr;
         printf("%X", codehold);
     }
     codearr = 0;
     
 if(codehold == 0x30CF){
     pattern1();
 }
 else if(codehold == 0x18E7 ){
     pattern2();
 }
 else if  (codehold == 0x7A85){
     pattern3();
 }
 else if (codehold == 0x10EF){
     pattern4();
 }
 else if (codehold == 0x38C7){
     pattern5();
 }
 else if (codehold == 0x5AA5){
     pattern6();
 }
 else if (codehold == 0x42BD){
     pattern7();
 }
 else if (codehold == 0x4AB5){
     pattern8();
 }
     
 else if(codehold == 0x52AD){
     pattern9();
    }
      
 }
 return;  
  }  
void pattern1(void){
    RC0 = 0;
    RC1 = 0;
    RC2 = 0;
    __delay_ms(250);
    RC0 = 1;
    __delay_ms(250);
    RC1 = 1;
    __delay_ms(250);
    RC2 = 1;
    __delay_ms(250);
    RC0 = 0;
    __delay_ms(250);
    RC1 = 0;
    __delay_ms(250);
    RC2=0;
    return;
}
void pattern2(void){
    RC0= 1;
    RC1 = 0;
    RC2 = 0;
    __delay_ms(250);
    return;
}
void pattern3(void){
    RC0= 0;
    RC1 = 1;
    RC2 = 0;
    __delay_ms(250);
    return;
}
void pattern4(void){
    RC0= 0;
    RC1 = 0;
    RC2 = 1;
    __delay_ms(250);
    return;
}
void pattern5(void){
    RC0=0;
    RC1=0;
    RC2=0;
    __delay_ms(250);
    RC0=1;
    __delay_ms(250);
    return;
}
void pattern6(void){
    RC0=0;
    RC1=0;
    RC2=0;
    __delay_ms(250);
    RC1=1;
    __delay_ms(250);
    return;
}
void pattern7(void){
    RC0=0;
    RC1=0;
    RC2=0;
    __delay_ms(250);
    RC2=1;
    __delay_ms(250);
    return;
}
void pattern8(void){
    printf("pattern8");
    __delay_ms(250);
    RC0 = 1;
    RC1 = 1;
    RC2 = 1;
    return;
}
void pattern9(void){
    RC0 = 0;
    RC1 = 0;
    RC2 = 0;
    return;
}
void setup_comms(void)  {  
RX_PIN = 1;  
TX_PIN = 1;  
SPBRG = DIVIDER;  
RCSTA = (NINE_BITS | 0x90);  
TXSTA = (SPEED | NINE_BITS | 0x20);  
TXEN = 1;  
SYNC = 0;  
SPEN = 1;  
BRGH = 1;  
  }  

void putch(unsigned char byte)  
  {  
/* output one byte */  
while(!TXIF) /* set when register is empty */  
continue;  
TXREG = byte;  
  }  

unsigned char getch()  
  {  
/* retrieve one byte */  
while(!RCIF) /* set when register is not empty */  
continue;  
return RCREG;  
  }  

unsigned char getche(void)  
  {  
unsigned char c;  
putch(c = getch());  
return c;  
  }  
  
void pinConfig(void)  {  
 OSCCON = 0x6A; /* b6..4 = 1101 = 4MHz */  
 TXCKSEL = 1; // both bits in APFCON0 MUST BE 1 for 1829 0 for 1825  
 RXDTSEL = 1; /* makes RC4 & 5 TX & RX for USART (Allows ICSP)*/  
  
 TRISA = 0x10;  
 ANSELA =0x10;  
 TRISC = 0x08; // C as output
 ANSELC =0xFF; /* all ADC pins to digital I/O */  
 ANSELB = 0x00;
 TRISB = 0xF0;
 ANSELB =0x00;
 WPUB = 0x00;
 INTCON = 0x08; // enable global interupt 
 IOCBF = 0x00;// clear port b flag
 IOCBN7 = 1; // enable falling edge interupt (sends 1 until button push)
 

}  
  




 