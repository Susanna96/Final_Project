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
#ifndef __SETTING_PARAMETER_H__
   #define __SETTING_PARAMETER_H__
    
   #include "cytypes.h"
    
    volatile int16 fullscale_range;
    volatile uint8_t sensitivity;
    volatile uint8_t confidence_interval;
    uint8 Parameters;
    void setParameter(void);
    
    
    
#endif

/* [] END OF FILE */
