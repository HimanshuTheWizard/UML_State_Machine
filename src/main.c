
#include <stdio.h>
#include <stdlib.h>
#include "diag/trace.h"
#include "pro_timer.h"
#include "timer.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

#define PROCESSOR_CLOCK			(16000000)				//16MHz
#define PROCESSOR_CLOCK_MS		(PROCESSOR_CLOCK/1000)

#define B1 (1)
#define B2 (2)
#define B3 (3)

#define INCREMENT_BTN (1)
#define DECREMENT_BTN (2)
#define START_PAUSE_BTN (4)
#define ABRT_BTN (3)

protimer_t protimer_obj;
user_event_t user_event;
tick_event_t tick_event;

uint8_t ButtonRead(uint8_t button);
void init_button(void);
void init_buzzer(void);
void init_LCD(void);
uint8_t process_for_debouncing(uint8_t btn_status);
void SysTick_TimeTick_Callback(void);

int main(int argc, char* argv[])
{
	uint8_t B1_status;
	uint8_t B2_status;
	uint8_t B3_status;
	uint8_t combine_button_status;

	uint32_t time_unit = 1000;							//ms

	init_button();
	init_buzzer();
	init_LCD();

	protimer_init(&protimer_obj, &user_event, &tick_event);

	//Callback registering
	Timer_Registering_Function(&SysTick_TimeTick_Callback);

	//Configure systick timer
	SysTick_Timer_Configuration(time_unit*PROCESSOR_CLOCK_MS);

	while(1)
	{
		//Read the button status
		B1_status = ButtonRead(B1);
		B2_status = ButtonRead(B2);
		B3_status = ButtonRead(B3);

		//set the event
		combine_button_status = (B1_status)|(B2_status<<1)|(B3_status<<2);
		combine_button_status = process_for_debouncing(combine_button_status);

		if(combine_button_status)
		{
			if(combine_button_status == INCREMENT_BTN)
			{
				user_event.sig = INC_TIME;
			}
			else if(combine_button_status == DECREMENT_BTN)
			{
				user_event.sig = DEC_TIME;
			}
			else if(combine_button_status == ABRT_BTN)
			{
				user_event.sig = ABRT;
			}
			else if(combine_button_status == START_PAUSE_BTN)
			{
				user_event.sig = START_PAUSE;
			}
			else
			{
				//do nothing
			}

			//call event dispatcher
			Event_dispatcher(&protimer_obj, &tick_event, &user_event);
		}
	}
}

uint8_t ButtonRead(uint8_t button)
{

}
void init_button(void)
{

}
void init_buzzer(void)
{

}
void init_LCD(void)
{

}
void SysTick_TimeTick_Callback(void)
{
	tick_event.sig = TIME_TICK;
	tick_event.ss++;
	if(tick_event.ss == 10)
	{
		tick_event.ss = 0;
	}
	Event_dispatcher(&protimer_obj, &tick_event, &user_event);
}
uint8_t process_for_debouncing(uint8_t btn_status)
{

}

#pragma GCC diagnostic pop

