//  28.09.2006

#define TIMER3_SECOND_VALUE		49911
#define TIMER3_200MS_VALUE		3125
#define TIMER3_MAX_INTERVAL		TIMER3_SECOND_VALUE - 3 * TIMER3_200MS_VALUE

void InitTimer0(void);
void InitTimer1(void);
void InitTimer3(void);
void InitPWM(void);
