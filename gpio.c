#include "rpi.h"
#include "gpio.h"

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );

void setup_gpio( unsigned int gpio_num, unsigned int gpio_func )
{
    unsigned int gpf_reg;
    unsigned int gpf_startbit;
    unsigned int gpf_val;
    
    if( gpio_num < 54 )
    {
		// don't handle alt functions yet
        if( gpio_func > 1 )
        {
            gpio_func = 1;
        }
        gpf_reg = (gpio_num / 10) * 4 + GPFSEL0;
        gpf_startbit = (gpio_num % 10) * 3;
        
        gpf_val = GET32(gpf_reg);
        gpf_val &= ~(0x7 << gpf_startbit);
        gpf_val |= (gpio_func << gpf_startbit);
		PUT32( gpf_reg, gpf_val );
    }
}

void set_gpio( unsigned int gpio_num, unsigned int gpio_val )
{
    if( gpio_num < 54 )
    {
        if( gpio_val == 0 )
        {
            if( gpio_num < 32 )
            {
                PUT32( GPCLR0, (1 << gpio_num));
            }
            else
            {
                PUT32( GPCLR1, (1 << (gpio_num - 32)));
            }
        }
        else // gpio_val == 1
        {
            if( gpio_num < 32 )
            {
                PUT32( GPSET0, (1 << gpio_num));
            }
            else
            {
                PUT32( GPSET1, (1 << (gpio_num - 32)));
            }
        }
    }
}

unsigned int get_gpio( unsigned int gpio_num )
{
    unsigned int retval;
    retval = 0;
    if( gpio_num < 54 )
    {
        if( gpio_num < 32 )
        {
            retval = GET32( GPLEV0 ) >> gpio_num & 0x1;
        }
        else
        {
            retval = GET32( GPLEV1 ) >> (gpio_num - 32) & 0x1;
        }
    }
    return retval;
}











