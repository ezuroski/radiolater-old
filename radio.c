#include "rpi.h"
#include "gpio.h"

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void enable_irq ( void );

// run everything on a 32khz heartbeat
#define BASE_FREQ 32000

volatile unsigned int irq_counter;

#define IRQ_INTERVAL RPI_TIMER_CLOCK/BASE_FREQ
void c_irq_handler ( void )
{
    irq_counter++;
	PUT32( C1, GET32( CLO ) + IRQ_INTERVAL );
    PUT32( CS,2 );
}

// GPIOs used for TM1638 display module
#define DISP_DIO 19
#define DISP_CLK 20
#define DISP_STB 21

#define DISP_DELAY 1
// rough microsecond delay (poll)
void delay_us( unsigned int us )
{
    // dummy volatile variable to prevent this function from being optimized into a nop
    volatile unsigned int loop_count;
	
	set_gpio( 47, 0 );
    // clock is 700mhz, 700 cycles per microsecond, 4 cycles per loop
    loop_count = us * (700/4);
    while( loop_count-- );
	set_gpio( 47, 1 );
}

void disp_sendbyte( unsigned int data )
{
    unsigned int index;

    //setup_gpio( DISP_DIO, GPIO_OUTPUT );
    set_gpio( DISP_CLK, 1 );
    delay_us( DISP_DELAY );
    for( index=0; index<8; index++ )
    {
        set_gpio( DISP_DIO, data & 0x1 );
        set_gpio( DISP_CLK, 0 );
        delay_us( DISP_DELAY );
        set_gpio( DISP_CLK, 1 );
        delay_us( DISP_DELAY );
        data >>= 1;
    }
}

unsigned int disp_readbyte()
{
    unsigned int index;
    unsigned int data;
    data = 0;
    
    setup_gpio( DISP_DIO, GPIO_INPUT );
    set_gpio( DISP_CLK, 1 );
    delay_us( DISP_DELAY );
    for( index=0; index<8; index++ )
    {
        data <<= 1;
        set_gpio( DISP_CLK, 0 );
        delay_us( DISP_DELAY );
        data |= get_gpio( DISP_DIO );
        set_gpio( DISP_CLK, 1 );
        delay_us( DISP_DELAY );
    }
    return data;
}

int notmain()
{
    unsigned int timer;
    unsigned int index;
	unsigned int act_led_count;
	unsigned int act_led_interval;
	
	// right now blinking the activity led is the only task that doesn't run at 32khz
	act_led_interval = BASE_FREQ * 0.5;
	act_led_count = act_led_interval;
    
    setup_gpio( 47, GPIO_OUTPUT );
    set_gpio( 47, 0 );

    setup_gpio( DISP_DIO, GPIO_OUTPUT );
    set_gpio( DISP_DIO, 0 );
    setup_gpio( DISP_CLK, GPIO_OUTPUT );
    set_gpio( DISP_CLK, 1 );
    setup_gpio( DISP_STB, GPIO_OUTPUT );
    set_gpio( DISP_STB, 1 );
    delay_us( DISP_DELAY );
    
    set_gpio( DISP_STB, 0 );
    delay_us( DISP_DELAY );
    disp_sendbyte( 0b01000000 ); // Data instruction, data write, autoincrement, not test mode
    set_gpio( DISP_STB, 1 );
    delay_us( DISP_DELAY );

    set_gpio( DISP_STB, 0 );
    delay_us( DISP_DELAY );
    disp_sendbyte( 0b11000000 ); // Address instruction, address 0

	disp_sendbyte( 0x54 ); // 'n'
	disp_sendbyte( 0x02 );  // odd addresses are seg9-10, unused

	disp_sendbyte( 0x5C ); // 'o'
	disp_sendbyte( 0x02 );

	disp_sendbyte( 0x0 ); // ' '
	disp_sendbyte( 0x03 );

	disp_sendbyte( 0x6D ); // 'S'
	disp_sendbyte( 0x02 );

	disp_sendbyte( 0x58 ); // 'c'
	disp_sendbyte( 0x03 );

	disp_sendbyte( 0x5C ); // 'o'
	disp_sendbyte( 0x03 );

	disp_sendbyte( 0x73 ); // 'P'
	disp_sendbyte( 0x02 );

	disp_sendbyte( 0x79 ); // 'E'
	disp_sendbyte( 0x03 );

    set_gpio( DISP_STB, 1 );
    delay_us( DISP_DELAY );

    set_gpio( DISP_STB, 0 );
    delay_us( DISP_DELAY );
    disp_sendbyte( 0b10001010 ); // Display control, display on, brightness 4/16
    //disp_sendbyte( 0b10001111 ); // Display control, display on, brightness 14/16
    set_gpio( DISP_STB, 1 );
    delay_us( DISP_DELAY );
    
	PUT32( C1, GET32( CLO ) + IRQ_INTERVAL );
    PUT32( CS, 2 );
    PUT32( 0x2000B210, 0x00000002 );
    enable_irq();
	
    while(1)
    {
		while( irq_counter == 0 );
		
		// todo: signal error if irq_counter is higher than 1
		irq_counter = 0;
		
		if( act_led_count-- == 0 )
		{
			act_led_count = act_led_interval;
			set_gpio( 47, get_gpio( 47 ) ^ 1 );
		}
    }
}
