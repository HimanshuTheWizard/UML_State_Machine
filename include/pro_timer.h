#include <stdio.h>
#include <stdint.h>

/*================States==============*/
typedef enum
{
	IDLE,
	TIME_SET,
	PAUSE,
	COUNTDOWN,
	STAT
}States;

/*================Signals==============*/
typedef enum
{
	INC_TIME,
	DEC_TIME,
	ABRT,
	START_PAUSE,
	TIME_TICK,
	ENTRY,
	EXIT
}Signals;

/*================event handing==============*/
typedef enum
{
	EVENT_HANDLED,
	EVENT_IGNORED,
	EVENT_TRANSITION
}Events_Status;

/*================Data Structure==============*/
typedef struct
{
	uint8_t c_time;			//current time
	uint8_t p_time;			//pro time
	uint8_t e_time;			//elapsed time
	States active_state;
}protimer_t;

/*================event==============*/
typedef struct
{
	uint8_t sig;
}user_event_t;

typedef struct
{
	uint8_t sig;
	uint8_t ss;
}tick_event_t;

/*================function declaration==============*/
void disp_msg(char * str);
void disp_time(uint8_t p_time);
void disp_clr(void);
void do_beep(void);
void protimer_init(protimer_t *protimer, user_event_t *user_evet, tick_event_t *tick_event);
Events_Status protimer_state_handler_idle(protimer_t *protimer, tick_event_t *tick_event, user_event_t *user_event);
Events_Status protimer_state_handler_time_set(protimer_t *protimer, user_event_t *user_event);
Events_Status protimer_state_handler_pause(protimer_t *protimer, user_event_t *user_event);
Events_Status protimer_state_handler_countdown(protimer_t *protimer, tick_event_t *tick_event, user_event_t *user_event);
Events_Status protimer_state_handler_stat(protimer_t *protimer, tick_event_t *tick_event, user_event_t *user_event);
Events_Status protimer_state_machine(protimer_t *protimer, tick_event_t *tick_event, user_event_t *user_event);
Events_Status Event_dispatcher(protimer_t *protimer, tick_event_t *tick_event, user_event_t *user_event);


