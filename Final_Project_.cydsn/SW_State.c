/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "SW_State.h"


int  device_state    = STOP;
int  t_dbl_click     = 18;
int  time_now        = 0;
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
    if ((time_elapsed < LONG_PRESS) && ((device_state == START) || (device_state == STOP)))
    {
        click_count  = 0;
        device_state = CONFIGURATION_MODE;
    } else if ((time_elapsed < LONG_PRESS) && (device_state == CONFIGURATION_MODE))
    {
        click_count  = 0;
        device_state = START;
    }
     else if ((click_count == 2) && (Timer_SW_ReadCounter() > t_dbl_click ) && ((device_state == STOP)))
    {
        click_count     = 0;
        device_state = START;
    } else if ((click_count == 2) && (Timer_SW_ReadCounter() > t_dbl_click ) && ((device_state == START)))
    {
        click_count    = 0;
        device_state = STOP;
    }
    
    return device_state;
}

/* [] END OF FILE */
