/* ========================================
 *
 * ========================================
*/

#include "project.h"
#include "InterruptRoutines.h"

//extern volatile int button_pressed;
//extern volatile int click_count;



CY_ISR(Switch_ISR)
{
    Timer_SW_ReadStatusRegister();
    button_pressed=1;
    click_count++;
    
}

CY_ISR(OVR_ISR)
{
    ths=1;
}



/* [] END OF FILE */

