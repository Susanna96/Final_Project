/* ========================================
 * Source code for the handling of interrupts
 * ========================================
*/

#include "project.h"
#include "InterruptRoutines.h"

/* ISR of the timer with clock frequency 1 Hz and period 20 sec */
CY_ISR(Switch_ISR)
{
    Timer_SW_ReadStatusRegister();
    button_pressed=1;           // set the flag button_pressed if the button is pressed
    click_count++;              // count the number of clicks
    
}

/* ISR of the timer with clock frequency 1 Hz and period 2 sec */
CY_ISR(TIME_ISR)
{
    Timer_CountTime_ReadStatusRegister();
    time_passed=time_passed+2;  // increase time_passed every 2 seconds
}

/* [] END OF FILE */

