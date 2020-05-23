/* ========================================
 *
 * ========================================
*/

#ifndef _INTERRUPT_ROUTINE_H
    #define _INTERRUPT_ROUTINE_H
        #include "project.h"
    
        volatile int        button_pressed;
        volatile int        click_count;
        
        CY_ISR_PROTO(Switch_ISR);
        
#endif

/* [] END OF FILE */
