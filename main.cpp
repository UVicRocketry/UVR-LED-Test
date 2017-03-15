/*
 *	LED serial test
 *
 *	@author	 Andres Martinez and Jordan Patterson
 *	@version 0.9b
 *	@date	 14-Mar-2017
 *
 *	Serial input program for STM Nucleo PWM-driven LEDs
 *
 *	Pin settings and dependency headers are for the Nucleo L053R8
 *
 *	Serial configuration required:
 *		- 9600 baud, 8-bit data, no parity
 *		- new line character at end of each command (no CR)
 *
 *	Written for UVic Rocketry
 */


#include "mbed.h"
#include "PwmDriver.hpp"
#include <array>

Serial pc(SERIAL_TX, SERIAL_RX);

//specify # of leds and their pin designations here
std::array<PwmDriver,3> leds{PwmDriver(D3),PwmDriver(D5),PwmDriver(D6)};

void print_help()
{
	pc.printf("\nList of commands:\n\n"
		"on # -> Turns LED # on (e.x. on 1)\n"
		"off # -> Turns LED # off (e.x. off 2)\n"
		"allon -> Turns all LEDs on\n"
		"alloff -> Turns all LEDs off\n\n"
		"duty # %% -> Sets LED # duty cycle to %% (0 to 100) (e.x. duty 1 50)\n"
		"allduty %% -> Sets all LED duty cycles to %% (0 to 100) (e.x. dutyall 25)\n\n"
		"status -> Prints LED status\n"
		"help -> Prints help menu\n\n"
		"kill -> Turns off all LEDs and ends the program\n"
		"\nNotes:\n"
		"- All command parameters must be entered as integers (no decimals)\n"
		"- LEDs are numbered 1 to %d\n"
		"- Duty percentages are numbered 0 to 100\n"
		"- Command lines with extra words/characters are invalid\n\n",leds.size());
}

void print_status()
{
	pc.printf("\nLed status:\n\n");

	int i = 1;
	for (auto & led : leds)
		pc.printf("* LED%d (pin %d): set_duty = %g%%, current_duty = %g%%, %s\n",i++,led.get_pin(),
			led.get_duty()*100.0,led.get_api_duty()*100.0,led.is_on()?"ON":"OFF");
}

bool valid_led(int led_num)
{
	if (led_num < 1 || led_num > (int)leds.size())
	{
		pc.printf("ERROR: Specified LED must be integer from 1 to %d\n",leds.size());
		return false;
	}
	return true;
}

bool valid_duty(int percent)
{
	if (percent < 0 || percent > 100)
	{
		pc.printf("ERROR: Duty percentage must be integer within [0,100]\n");
		return false;
	}
	return true;
}

void all_off()
{
	for (auto & led : leds)
		led.turn_off();
}

int main()
{
	pc.printf("\n*****************************"
			  "\nSerial interface initialized!"
			  "\n*****************************\n\n");
	print_status();
	print_help();

	while(1) {
		//iostreams and strings avoided
		char line[128];
		char token[128];
		int led_num;
		int led_duty;

		pc.printf("\n**************************************"
				  "\nEnter command ('help' to view manual):\n");
		int err = pc.scanf("%127[^\n]",line);
		if (err != 1)
		{
			pc.printf("ERROR: Empty line recieved!\n");
			continue;
		}
		line[127] = 0;
		token[0] = 0;
		if (sscanf(line,"%s",token) == EOF)
		{
			pc.printf("ERROR: line is only whitespace!\n");
			continue;
		}

		int word_count = 0;
		bool letter_found = false;
		for (int i = 0; i < (int)strlen(line); i++)
		{
			if (line[i] == ' ')
				letter_found = false;
			else
				if (!letter_found)
				{
					letter_found = true;
					word_count++;
				}
		}

		pc.printf("\nLine recieved: %s\n\n",line);

		if (strcmp(token,"allduty") == 0 && word_count == 2 && sscanf(line,"%*s%d",&led_duty) == 1)
		{
			if (!valid_duty(led_duty))
				continue;
			float percent_f = (float)led_duty/100.0;
			pc.printf("Setting all LEDs to %d%%\n",led_duty);

			for (auto & led : leds)
				led.set_duty(percent_f);
		}

		else if (strcmp(token,"duty") == 0 && word_count == 3 && sscanf(line,"%*s%d%d",&led_num,&led_duty) == 2)
		{
			if (!valid_led(led_num) || !valid_duty(led_duty))
				continue;
			float percent_f = (float)led_duty/100.0;
			pc.printf("Setting LED %d to %d%%\n",led_num,led_duty);
			leds[--led_num].set_duty(percent_f);
		}

		else if (strcmp(token, "on") == 0 && word_count == 2 && sscanf(line, "%*s%d",&led_num) == 1)
		{
			if (!valid_led(led_num))
				continue;

			pc.printf("Turning on LED %d...\n", led_num);

			leds[--led_num].turn_on();
		}

		else if (strcmp(token, "off") == 0 && word_count == 2 && sscanf(line, "%*s%d",&led_num) == 1)
		{
			if (!valid_led(led_num))
				continue;

			pc.printf("Turning off LED %d...\n", led_num);

			leds[--led_num].turn_off();
		}

		else if (strcmp(token,"allon") == 0 && word_count == 1)
		{
			pc.printf("Turning on all LEDs...\n");

			for (auto & led : leds)
				led.turn_on();
		}

		else if (strcmp(token,"alloff") == 0 && word_count == 1)
		{
			pc.printf("Turning off all LEDs...\n");

			all_off();
		}

		else if (strcmp(token,"status") == 0 && word_count == 1)
		{
			print_status();
		}

		else if (strcmp(token, "help") == 0 && word_count == 1)
		{
			print_help();
		}

		else if (strcmp(token, "kill") == 0)
		{
			pc.printf("Deactivating LEDs and killing program...\n");
			all_off();
			pc.printf("\n\n**************PROGRAM TERMINATED**************");
			return(0);
		}

		else
		{
			pc.printf("ERROR: Invalid line sent. Check your command and parameter values. Make sure there are no extra characters.\n",line);
		}

		//safety check to see if LEDs that are "off" are actually OFF!
		for (auto & led : leds)
		{
			if (!led.is_on() && led.get_api_duty() != 0.00)
			{
				pc.printf("FATAL ERROR: LED at pin %d not turned off successfully! Disconnect power!\n",led.get_pin());
				all_off();
				return -1;
			}
		}

		wait(0.1);
	}
}
