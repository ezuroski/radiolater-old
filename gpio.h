#define GPIO_INPUT 0
#define GPIO_OUTPUT 1

void setup_gpio( unsigned int gpio_num, unsigned int gpio_func );

void set_gpio( unsigned int gpio_num, unsigned int gpio_val );

unsigned int get_gpio( unsigned int gpio_num );
