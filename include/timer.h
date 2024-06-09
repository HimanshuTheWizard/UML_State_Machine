#ifndef TIMER_H_
#define TIMER_H_

void SysTick_Timer_Configuration(uint32_t ticks);
void Start_Timer(void);
void Timer_Registering_Function(void (*callback)(void));

#endif // TIMER_H_
