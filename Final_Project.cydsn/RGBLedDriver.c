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
#include  "RGBLedDriver.h"
#include "project.h"
#include  "SettingParameter.h"

#define G 1000
#define LEVELS 4
#define X 0
#define Y 1
#define Z 2

uint16_t step;
uint8_t direction;


void RGBLed_Start(void)
{
    PWM_RG_Start();
    PWM_B_Start();
}

void RGBLed_Stop(void)
{
    PWM_RG_Stop();
    PWM_B_Stop();
}



void set_PWM(int16_t X_Out,int16_t Y_Out,int16_t Z_Out)
{
    step=(fullscale_range-(G+confidence_interval))/LEVELS;
    
    if((X_Out>=G-confidence_interval) && (X_Out<=G+confidence_interval))
    {
        direction=X;   
    }
    
    else if((Y_Out>=G-confidence_interval) && (Y_Out<=G+confidence_interval))
    {
        direction=Y;
    }
    
    else if((Z_Out>=G-confidence_interval) && (Z_Out<=G+confidence_interval))
    {
        direction=Z;            
    }
   
    switch(direction)
    {
        case X:
        RGBLed_Stop();
        PWM_RG_WritePeriod(255);
        PWM_RG_WriteCompare2(0);
        PWM_B_WritePeriod(255);
        PWM_B_WriteCompare(0);
        RGBLed_Start();
        if ((X_Out>=G-confidence_interval) && (X_Out<=G+confidence_interval))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(255);
          PWM_RG_WriteCompare1(255);
          RGBLed_Start();
        }
        
        else if((X_Out>G+confidence_interval) && (X_Out<=G+confidence_interval+step))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(65);
          PWM_RG_WriteCompare1(32);  
          RGBLed_Start();
        }   
        else if ((X_Out>G+confidence_interval+step) && (X_Out<=G+confidence_interval+2*step))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(32);
          PWM_RG_WriteCompare1(16);  
          RGBLed_Start();
        }  
        else if ((X_Out>G+confidence_interval+2*step) && (X_Out<=G+confidence_interval+4*step))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(16);
          PWM_RG_WriteCompare1(8);
          RGBLed_Start();
        }  
        break;
        
        case Y:
        RGBLed_Stop();
        PWM_RG_WritePeriod(255);
        PWM_RG_WriteCompare1(0);
        PWM_B_WritePeriod(255);
        PWM_B_WriteCompare(0);
        RGBLed_Start();
        if ((Y_Out>=G-confidence_interval) && (Y_Out<=G+confidence_interval))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(255);
          PWM_RG_WriteCompare2(255);
          RGBLed_Start();
        }
        else if((Y_Out>G+confidence_interval) && (Y_Out<=G+confidence_interval+step))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(65);
          PWM_RG_WriteCompare2(32);  
          RGBLed_Start();
        }   
        else if ((Y_Out>G+confidence_interval+step) && (Y_Out<=G+confidence_interval+2*step))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(32);
          PWM_RG_WriteCompare2(16);  
          RGBLed_Start();
        }  
        else if ((Y_Out>G+confidence_interval+2*step) && (Y_Out<=G+confidence_interval+4*step))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(16);
          PWM_RG_WriteCompare2(8); 
          RGBLed_Start();
        }  
        break;
        
        case Z:
        RGBLed_Stop();
        PWM_RG_WritePeriod(255);
        PWM_RG_WriteCompare1(0);
        PWM_RG_WriteCompare2(0);
        RGBLed_Start();
        if ((Z_Out>=G-confidence_interval) && (Z_Out<=G+confidence_interval))
        {
          RGBLed_Stop();
          PWM_B_WritePeriod(255);
          PWM_B_WriteCompare(255);
          RGBLed_Start();
        }
        else if((Z_Out>G+confidence_interval) && (Z_Out<=G+confidence_interval+step))
        {
          RGBLed_Stop();
          PWM_B_WritePeriod(65);
          PWM_B_WriteCompare(32); 
          RGBLed_Start();
        }   
        else if ((Z_Out>G+confidence_interval+step) && (Z_Out<=G+confidence_interval+2*step))
        {
          RGBLed_Stop();
          PWM_B_WritePeriod(32);
          PWM_B_WriteCompare(16);
          RGBLed_Start();
        }  
        else if ((Z_Out>G+confidence_interval+2*step) && (Z_Out<=G+confidence_interval+4*step))
        {
          RGBLed_Stop();
          PWM_B_WritePeriod(16);
          PWM_B_WriteCompare(8);  
          RGBLed_Start();
        }  
        break;
    }
    
}


/* [] END OF FILE */
