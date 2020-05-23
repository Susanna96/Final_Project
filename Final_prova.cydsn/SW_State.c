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


#define LONG_PRESS          150
#define START               0
#define STOP                1
#define CONFIGURATION_MODE  2
int  device_state    = STOP;
int  double_click    = 0;
int  t_press         = 0;
int  t_dbl_click     = 50;
int  time_now        = 0;
int  time_elapsed    = 0;
extern volatile int button_pressed;
extern volatile int click_count;

int Switch_State(void)
{
    t_press = Timer_SW_ReadCounter();
    button_pressed = 0;
    while ((BUTTON_Read() == 0))
    {
        time_now = Timer_SW_ReadCounter();
        time_elapsed = time_now - t_press;
    }
    if ((time_elapsed > LONG_PRESS) && ((device_state == START) || (device_state == STOP)))
    {
        click_count  = 0;
        time_elapsed = 0;
        time_now     = 0;
        t_press      = 0;
        device_state = CONFIGURATION_MODE;
    } else if ((time_elapsed > LONG_PRESS) && (device_state == CONFIGURATION_MODE))
    {
        click_count  = 0;
        time_elapsed = 0;
        time_now     = 0;
        t_press      = 0;
        device_state = START;
    }
     else if ((click_count == 2) && ((Timer_SW_ReadCounter() - t_press) < t_dbl_click ) && ((device_state == STOP)))
    {
        time_now        = 0;
        time_elapsed    = 0;
        click_count     = 0;
        button_pressed  = 0;
        t_press = 0;
        device_state = START;
    } else if ((click_count == 2) && ((Timer_SW_ReadCounter() - t_press) < t_dbl_click ) && ((device_state == START)))
    {
        time_now        = 0;
        time_elapsed    = 0;
        click_count    = 0;
        button_pressed = 0;
        t_press = 0;
        device_state = STOP;
    }
    
    return device_state;
}

/* [] END OF FILE */
