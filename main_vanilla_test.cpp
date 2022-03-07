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

void chasingLEDS(GpoCore *led_p, GpiCore *sw_p){
	int s, init, slideSwitches, speed;
	int speedOld;

	init = sw_p->read(0);		//read bit 0 of switch input to get value of "initialize" sw
	s = sw_p->read();			//read values of sw's
	slideSwitches = s & 0x003E;	//cast to isolate bits 1 - 5 and which equate to sw's 1 - 5
	speed = slideSwitches * 5; 	//after testing want min speed to be 10 ms = 2(min value inputted) * 5

	if(init)
		led_p->write(1,0); //light up led in rightmost position while sw0 is 1
	else{

		if(speed != speedOld){	//if value of speed changed display it
			uart.disp("current speed:");
			uart.disp(slideSwitches >> 1);
			uart.disp("\n\r");
		}
		//right to left
		for(int i = 0; i < 16; i++){
			init = sw_p->read(0);	//check init value after every led jump
			if(init)
				break;
			led_p->write(1, i);
			sleep_ms(speed);
			led_p->write(0, i);
			sleep_ms(speed);
		}
		speedOld = speed;
	}

	if(init)
			led_p->write(1,0); //light up led in rightmost position while sw0 is 1
	else{
		if(speed != speedOld){	//if value of speed changed display it
			uart.disp("current speed:");
			uart.disp(slideSwitches >> 1);
			uart.disp("\n\r");
		}
		//left to right
		for(int i = 16; i > 0; i--){
			init = sw_p->read(0);	//check init value after every led jump
			if(init)
				break;
			led_p->write(1, i);
			sleep_ms(speed);
			led_p->write(0, i);
			sleep_ms(speed);
		}
		speedOld = speed;
	}
}

// instantiate switch, led
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));

int main() {

   while (1) {
	   chasingLEDS(&led, &sw);

      //timer_check(&led);
      //led_check(&led, 16);
      //sw_check(&led, &sw);
      //uart_check();
      //debug("main - switch value / up time : ", sw.read(), now_ms());
   } //while
} //main
