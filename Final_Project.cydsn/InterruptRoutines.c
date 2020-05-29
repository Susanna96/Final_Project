/* ========================================
 *
 * ========================================
*/

#include "project.h"
#include "InterruptRoutines.h"


CY_ISR(Switch_ISR)
{
    Timer_SW_ReadStatusRegister();
    button_pressed=1;
    click_count++;
    
}

//CY_ISR(OVR_ISR)
//{
//    ths=1;
//}

CY_ISR(TIME_ISR)
{
    Timer_CountTime_ReadStatusRegister();
    time_passed=time_passed+2;
}



/* [] END OF FILE */

