#include <sys/alt_stdio.h>
#include <stdio.h>
#include "altera_avalon_pio_regs.h"
#include "system.h"

int calc_delay (){
    int switch_datain = IORD_ALTERA_AVALON_PIO_DATA(SWITCH_BASE);
    float switch_ratio = switch_datain / 1024.0; // Normalize switch data to range [0, 1]
    int delay = (int)(1000000 * (1 - switch_ratio)); // Adjust delay inversely proportional to switch data
    return delay;
}
int main()
{
    int button_datain;
    alt_putstr("Hello from Nios II!\n");

    while (1)
    {
    
        button_datain = IORD_ALTERA_AVALON_PIO_DATA(BUTTON_BASE);

        int currlight = 1;
        int delay;
        // Calculate delay based on switch data


        for (int i = 0; i < 9; i++)
        {
            currlight = currlight * 2;
            delay=calc_delay();
            alt_putstr("Switch Data: ");
            alt_printf("%d\n", delay);
            usleep(delay); // Delay based on switch data
            IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, currlight);
        }

        for (int j = 10; j > 1; j--)
        {
            currlight = currlight / 2;
            delay=calc_delay();
            alt_putstr("Switch Data: ");
            alt_printf("%d\n", delay);
            usleep(delay); // Delay based on switch data
            IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, currlight);
        }
    }

    return 0;
}
