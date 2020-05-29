/* ========================================
 *
 * ========================================
*/

#ifndef _INTERRUPT_ROUTINE_H
    #define _INTERRUPT_ROUTINE_H
        #include "project.h"
    
        volatile int        button_pressed;
        volatile int        click_count;
        volatile uint16_t   time_passed;
        //volatile uint8_t    ths;
        
        CY_ISR_PROTO(Switch_ISR);
        //CY_ISR_PROTO(OVR_ISR);
        CY_ISR_PROTO(TIME_ISR);
        
#endif

/* [] END OF FILE */
