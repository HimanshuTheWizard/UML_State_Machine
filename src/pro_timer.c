#include "pro_timer.h"

void disp_msg(char * str)
{

}

void disp_time(uint8_t p_time)
{

}

void disp_clr(void)
{

}

void do_beep(void)
{

}

void protimer_init(protimer_t *protimer, user_event_t *user_event, tick_event_t *tick_event)
{
	user_event->sig = ENTRY;
	protimer->active_state = IDLE;
	protimer->p_time = 0;
	protimer_state_machine(protimer, tick_event, user_event);
}

Events_Status protimer_state_handler_idle(protimer_t *protimer, tick_event_t *tick_event, user_event_t *user_event)
{
	switch(user_event->sig)
	{
		case ENTRY:
		{
			protimer->c_time = 0;
			protimer->e_time = 0;
			disp_msg("Set Time:");
			disp_time(protimer->c_time);
			return EVENT_HANDLED;
		}
		case EXIT:
		{
			disp_clr();
			return EVENT_HANDLED;
		}
		case INC_TIME:
		{
			protimer->c_time += 60;
			protimer->active_state = TIME_SET;
			return EVENT_HANDLED;
		}
		case START_PAUSE:
		{
			protimer->active_state = STAT;
			return EVENT_TRANSITION;
		}
		case TIME_TICK:
		{
			if(tick_event->ss == 5)
			{
				do_beep();
				return EVENT_TRANSITION;
			}
			return EVENT_IGNORED;
		}
	}
	return EVENT_IGNORED;
}

Events_Status protimer_state_handler_time_set(protimer_t *protimer, user_event_t *user_event)
{
	switch(user_event->sig)
	{
		case ENTRY:
		{
			disp_time(protimer->c_time);
			return EVENT_HANDLED;
		}
		case EXIT:
		{
			disp_clr();
			return EVENT_HANDLED;
		}
		case INC_TIME:
		{
			protimer->c_time += 60;
			disp_time(protimer->c_time);
			return EVENT_HANDLED;
		}
		case DEC_TIME:
		{
			protimer->c_time -= 60;
			disp_time(protimer->c_time);
			return EVENT_HANDLED;
		}
		case ABRT:
		{
			protimer->active_state = IDLE;
			return EVENT_TRANSITION;
		}
		case START_PAUSE:
		{
			if(protimer->c_time >= 60)
			{
				protimer->active_state = COUNTDOWN;
				return EVENT_TRANSITION;
			}
			return EVENT_IGNORED;
		}
	}
	return EVENT_IGNORED;
}

Events_Status protimer_state_handler_pause(protimer_t *protimer, user_event_t *user_event)
{
	switch(user_event->sig)
	{
		case ENTRY:
		{
			disp_msg("PAUSED");
			return EVENT_HANDLED;
		}
		case EXIT:
		{
			disp_clr();
			return EVENT_HANDLED;
		}
		case INC_TIME:
		{
			protimer->c_time += 60;
			protimer->active_state = TIME_SET;
			return EVENT_TRANSITION;
		}
		case DEC_TIME:
		{
			if(protimer->c_time >= 60)
			{
				protimer->c_time -= 60;
				protimer->active_state = TIME_SET;
				return EVENT_TRANSITION;
			}
			return EVENT_IGNORED;
		}
		case ABRT:
		{
			protimer->active_state = IDLE;
			return EVENT_TRANSITION;
		}
		case START_PAUSE:
		{
			protimer->active_state = COUNTDOWN;
			return EVENT_TRANSITION;
		}
	}
	return EVENT_IGNORED;
}

Events_Status protimer_state_handler_countdown(protimer_t *protimer, tick_event_t *tick_event, user_event_t *user_event)
{
	switch(user_event->sig)
	{
		case START_PAUSE:
		{
			protimer->active_state = PAUSE;
			return EVENT_TRANSITION;
		}
		case ABRT:
		{
			protimer->active_state = IDLE;
			return EVENT_TRANSITION;
		}
		case TIME_TICK:
		{
			if(tick_event->ss == 10)
			{
				--(protimer->c_time);
				++(protimer->e_time);
				disp_time(protimer->c_time);
				if(protimer->c_time == 0)
				{
					protimer->active_state = IDLE;
					return EVENT_TRANSITION;
				}
				else
				{
					return EVENT_HANDLED;
				}
			}
			return EVENT_IGNORED;
		}
		case EXIT:
			protimer->p_time += protimer->e_time;
			protimer->e_time = 0;
			return EVENT_HANDLED;
	}
	return EVENT_IGNORED;
}

Events_Status protimer_state_handler_stat(protimer_t *protimer, tick_event_t *tick_event, user_event_t *user_event)
{
	switch(user_event->sig)
	{
		case ENTRY:
			disp_time(protimer->p_time);
			disp_msg("Productive Time");
			return EVENT_HANDLED;
		case EXIT:
			disp_clr();
			return EVENT_HANDLED;
		case TIME_TICK:
			if(tick_event->ss == 10)
			{
				protimer->active_state = IDLE;
				return EVENT_TRANSITION;
			}
			return EVENT_IGNORED;
	}
	return EVENT_IGNORED;
}

Events_Status protimer_state_machine(protimer_t *protimer, tick_event_t *tick_event, user_event_t *user_event)
{
	Events_Status event = EVENT_IGNORED;
	switch(protimer->active_state)
	{
		case IDLE:
		{
			event = protimer_state_handler_idle(protimer, tick_event, user_event);
			return event;
		}
		case TIME_SET:
		{
			event = protimer_state_handler_time_set(protimer, user_event);
			return event;
		}
		case PAUSE:
		{
			event = protimer_state_handler_pause(protimer, user_event);
			return event;
		}
		case COUNTDOWN:
		{
			event = protimer_state_handler_countdown(protimer, tick_event, user_event);
			return event;
		}
		case STAT:
		{
			event = protimer_state_handler_stat(protimer, tick_event, user_event);
			return event;
		}
		return event;
	}
	return event;
}

Events_Status Event_dispatcher(protimer_t *protimer, tick_event_t *tick_event, user_event_t *user_event)
{
	Events_Status event;
	States src_state, target_state;
	src_state = protimer->active_state;

	event = protimer_state_machine(protimer, tick_event, user_event);
	if(event == EVENT_TRANSITION)
	{
		target_state = protimer->active_state;

		//run the exit action for source state
		protimer->active_state = src_state;
		user_event->sig = EXIT;
		protimer_state_machine(protimer, tick_event, user_event);

		//run the entry action for the target state
		protimer->active_state = target_state;
		user_event->sig = ENTRY;
		protimer_state_machine(protimer, tick_event, user_event);
	}
}




