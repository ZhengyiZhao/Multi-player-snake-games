

#include <stdio.h>

#include "Snake.h"
#include "snktimer.h"

#define __HAVE_TIMER__
#ifdef __HAVE_TIMER__
#include <sys/time.h>
///
///
///
#include <signal.h>
/// Definition for:
///    SIGPROF
///    structure sigaction

#define PROMPT  "Time gone for 1 second\n\a"

//#define ITIMER_PROF 2
char *strPrompt = PROMPT;
unsigned short usLengthChar;
unsigned long ulCount;



/**------------------------
 * Name:     prompt_info
 * Purpose:  Following is a function to response timer interrupt
 * Date of Creating:
 * Notes
 *-------------------------**/
void prompt_info(int signo)
{
   ulCount = ulCount + 1;
#ifdef __OUT_NUMBER__
   printf("%ld ", ulCount);
   if(ulCount % 20 == 0)
   {
       printf("\n");
   }
#endif   
   if(ulCount >= 100)
   {
      printf("%s", strPrompt);
      ulCount = 0;
   }
      
}

/**------------------------
 * Name: init_sigaction
 * Purpose: Initialize the function pointer to the timer
 * Date of Creating:
 * Notes
 *-------------------------**/
void init_sigaction(void)
{
	struct sigaction act;
	act.sa_handler = snake_rtdi_rtug;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGPROF, &act, NULL);
}

/**------------------------
 * Name: init_timer
 * Purpose: Initialize the timer
 * Date of Creating:
 * Notes
 *-------------------------**/
void init_timer()
{
	struct itimerval stTimerValue;
	
	stTimerValue.it_value.tv_sec = 0;
	stTimerValue.it_value.tv_usec = __SNAKE_TIMER_USEC__;
	stTimerValue.it_interval = stTimerValue.it_value;
	setitimer(ITIMER_PROF, &stTimerValue, NULL);

}

/**------------------------
 * Name:  init_timer_for_snake
 * Purpose: Testing entrance
 * Date of Creating:
 * Notes
 *-------------------------**/
int init_timer_for_snake()
{
  init_sigaction();
  init_timer();
 
  return SNK_OK;
}
#endif // __HAVE_TIMER__

