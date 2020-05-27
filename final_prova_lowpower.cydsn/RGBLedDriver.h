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
#ifndef __RGB_LED_DRIVER_H__
   #define __RGB_LED_DRIVER_H__
    
   #include "cytypes.h"
    #include "stdlib.h"
    
    
    
    void  RGBLed_Start(void);
    void  RGBLed_Stop(void);
    
    void set_PWM(int16_t X_Out,int16_t Y_Out,int16_t Z_Out);
    
    
    
#endif

/* [] END OF FILE */
