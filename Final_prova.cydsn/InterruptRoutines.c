/* ========================================
 *
 * ========================================
*/

#include "project.h"
#include "InterruptRoutines.h"

extern volatile int button_pressed;
extern volatile int click_count;



CY_ISR(Switch_ISR)
{
    button_pressed=1;
    click_count++;
}



/* [] END OF FILE */

