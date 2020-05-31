/* ========================================
 * This file includes all the required source 
 * code to determine the state of the 
 * device according to the different actions
 * performed on the on board button:
 * - double-click
 * - long press
 *
 * ========================================
*/
#include "project.h"
#include "SW_State.h"


int  device_state    = STOP;
int  t_dbl_click     = 18;
int  time_elapsed    = 0;


int Switch_State(void)
{
    /* Reset timer each time the button is pressed */
    Timer_SW_Stop();
    Timer_SW_WriteCounter(20);
    Timer_SW_Start();
    
    /* Set the flag to 0 each time the button is pressed */
    button_pressed = 0;
    
    while ((BUTTON_Read() == 0))
    {
        /* Count for how much time the button is pressed */
        time_elapsed = Timer_SW_ReadCounter();
    }
    /* Enter CONFIGURATION MODE when the button is pressed for long (more than 2 sec) */
    if ((time_elapsed < LONG_PRESS) && ((device_state == START) || (device_state == STOP)))
    {
        click_count  = 0;
        device_state = CONFIGURATION_MODE;
    } 
    /* Exit CONFIGURATION MODE when the button is pressed for long again (more than 2 sec) */
    else if ((time_elapsed < LONG_PRESS) && (device_state == CONFIGURATION_MODE))
    {
        click_count  = 0;
        device_state = START;
    }
    /* Enter START with double click and button pressed for short time (less that 2 sec) */
     else if ((click_count == 2) && (Timer_SW_ReadCounter() > t_dbl_click ) && ((device_state == STOP)))
    {
        click_count     = 0;
        device_state = START;
    }
    /* Enter STOP with double click and button pressed for short time (less that 2 sec) */
    else if ((click_count == 2) && (Timer_SW_ReadCounter() > t_dbl_click ) && ((device_state == START)))
    {
        click_count    = 0;
        device_state = STOP;
    }
    
    return device_state;
}

/* [] END OF FILE */
