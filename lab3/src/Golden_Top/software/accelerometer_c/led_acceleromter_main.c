#include "system.h"
#include "altera_up_avalon_accelerometer_spi.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_timer.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include <stdlib.h>

#include "alt_types.h"
#include "sys/times.h"

#define OFFSET -32
#define PWM_PERIOD 16

alt_8 pwm = 0;
alt_u8 led;
int level;
#define taps 5

float coeffs[taps] = {0};
alt_32 samples[taps] = {0};

void initFIR(){
    for(int i=0;i<taps;i++){
        samples[i]=0;
        coeffs[i]=1.0/taps;
    }
}

void led_write(alt_u8 led_pattern) {
    IOWR(LED_BASE, 0, led_pattern);
}

alt_32 FIRLP(alt_32 newestVal) {
    alt_32 avg=0;
    for(int i=0;i<taps-1;i++){
        samples[i+1]=samples[i];
    }
    samples[0]=newestVal;
     
    for(int i=0;i<taps;i++){
        avg += coeffs[i]*samples[i];
    }
    return avg;
}

void convert_read(alt_32 acc_read, int * level, alt_u8 * led) {
    acc_read += OFFSET;
    alt_32 avg=FIRLP(acc_read);//moving avg
    alt_u8 val = (avg >> 6) & 0x07;
    * led = (8 >> val) | (8 << (8 - val));
    * level = (avg >> 1) & 0x1f;
}

void sys_timer_isr() {
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);

    if (pwm < abs(level)) {

        if (level < 0) {
            led_write(led << 1);
        } else {
            led_write(led >> 1);
        }

    } else {
        led_write(led);
    }

    if (pwm > PWM_PERIOD) {
        pwm = 0;
    } else {
        pwm++;
    }

}

void timer_init(void * isr) {

    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x0003);
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, 0x0900);
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_BASE, 0x0000);
    alt_irq_register(TIMER_IRQ, 0, isr);
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x0007);

}

int main() {

    alt_32 x_read;
    alt_up_accelerometer_spi_dev * acc_dev;
    acc_dev = alt_up_accelerometer_spi_open_dev("/dev/accelerometer_spi");
    if (acc_dev == NULL) { // if return 1, check if the spi ip name is "accelerometer_spi"
        return 1;
    }
    initFIR();

    timer_init(sys_timer_isr);
    while (1) {
    	clock_t exec_t1, exec_t2;
		exec_t1=times(NULL);

        alt_up_accelerometer_spi_read_x_axis(acc_dev, & x_read);
        //alt_printf("raw data: %x\n", x_read);
        convert_read(x_read, & level, & led);

		exec_t2 = times(NULL); // get system time after finishing the process
		float delta_t=exec_t2-exec_t1;
		alt_printf("proc time = %dticks\n", delta_t);
    }


    return 0;
}
