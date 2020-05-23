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
    
    volatile int fullscale_range;
    volatile int sensitivity;
    void setParameter(int16 value_pot);
    
    
    
#endif

/* [] END OF FILE */
