/*****************************************************************//**
 * @file main_vanilla_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"

/**
 * blink once per second for 5 times.
 * provide a sanity check for timer (based on SYS_CLK_FREQ)
 * @param led_p pointer to led instance
 */
void timer_check(GpoCore *led_p) {
   int i;

   for (i = 0; i < 5; i++) {
      led_p->write(0xffff);
      sleep_ms(500);
      led_p->write(0x0000);
      sleep_ms(500);
      debug("timer check - (loop #)/now: ", i, now_ms());
   }
}

/**
 * check individual led
 * @param led_p pointer to led instance
 * @param n number of led
 */
void led_check(GpoCore *led_p, int n) {
   int i;

   for (i = 0; i < n; i++) {
      led_p->write(1, i);
      sleep_ms(100);
      led_p->write(0, i);
      sleep_ms(100);
   }
}

/**
 * leds flash according to switch positions.
 * @param led_p pointer to led instance
 * @param sw_p pointer to switch instance
 */
void sw_check(GpoCore *led_p, GpiCore *sw_p) {
   int i, s;

   s = sw_p->read();
   for (i = 0; i < 30; i++) {
      led_p->write(s);
      sleep_ms(50);
      led_p->write(0);
      sleep_ms(50);
   }
}

/**
 * uart transmits test line.
 * @note uart instance is declared as global variable in chu_io_basic.h
 */
void uart_check() {
   static int loop = 0;

   uart.disp("uart test #");
   uart.disp(loop);
   uart.disp("\n\r");
   loop++;
}


void chasingLeds(GpoCore *led_p, GpiCore *sw_p){

	int s, init, speedsw, speed;
	int speedOld;

	init = sw_p->read(0);	//read bit 0 of switches for init

	while(init){
		led_p->write(1,0);		//light up the right most led when init is asserted
		init = sw_p->read(0);	//constantly check for init change
	}
	while(!init){
		init = sw_p->read(0);		//constantly check for init change
		s = sw_p->read();			//read entire input
		speedsw =  s & 0x003E;		//keep bit 1 - 5 for speed input
		speed = (speedsw + 1) * 5;	//min speed will be 5ms

		//right to left
		for (int i = 0; i < 16; i++) {

			//check for sw0 after every led
			init = sw_p->read(0);
			if(init)
				break;

			//check for change in speed after every led
			s = sw_p->read();
			speedsw =  s & 0x003E;
			speed = (speedsw + 1) * 5;

			//display if speed changed
			if(speed != speedOld){
				uart.disp("current speed:");
				uart.disp(speedsw >> 1);
				uart.disp("\n\r");
			}

			led_p->write(1, i);
			sleep_ms(speed);
			led_p->write(0, i);
			sleep_ms(speed);

			speedOld = speed;	//save current speed for comparison later
		}

		//left to right
		for (int i = 16; i > 0; i--) {

			//check for sw0 after every led
			init = sw_p->read(0);
			if(init)
				break;

			//check for change in speed after every led
			s = sw_p->read();
			speedsw =  s & 0x003E;
			speed = (speedsw + 1) * 5;

			//display if speed changed
			if(speed != speedOld){
				uart.disp("current speed:");
				uart.disp(speedsw >> 1);
				uart.disp("\n\r");
			}

			led_p->write(1, i);
			sleep_ms(speed);
			led_p->write(0, i);
			sleep_ms(speed);

			speedOld = speed;	//save current speed for comparison later
		}
	}
}

// instantiate switch, led
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));

int main() {

   while (1) {
	   chasingLeds(&led, &sw);

      //timer_check(&led);
      //led_check(&led, 16);
      //sw_check(&led, &sw);
      //uart_check();
      //debug("main - switch value / up time : ", sw.read(), now_ms());
   } //while
} //main

