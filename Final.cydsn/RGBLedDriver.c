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

#define G 1000
#define CONFIDENCE_INTERVAL 20
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
    step=(4000-(G+CONFIDENCE_INTERVAL))/LEVELS; //cambia in base al FS -> METTI FS COME INPUT
    
    if((X_Out>=G-CONFIDENCE_INTERVAL) && (X_Out<=G+CONFIDENCE_INTERVAL))
    {
        direction=X;   
    }
    
    else if((Y_Out>=G-CONFIDENCE_INTERVAL) && (Y_Out<=G+CONFIDENCE_INTERVAL))
    {
        direction=Y;
    }
    
    else if((Z_Out>=G-CONFIDENCE_INTERVAL) && (Z_Out<=G+CONFIDENCE_INTERVAL))
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
        if ((X_Out>=G-CONFIDENCE_INTERVAL) && (X_Out<=G+CONFIDENCE_INTERVAL))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(255);
          PWM_RG_WriteCompare1(255);
          RGBLed_Start();
        }
        
        else if((X_Out>G+CONFIDENCE_INTERVAL) && (X_Out<=G+CONFIDENCE_INTERVAL+step))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(65);
          PWM_RG_WriteCompare1(32);  
          RGBLed_Start();
        }   
        else if ((X_Out>G+CONFIDENCE_INTERVAL+step) && (X_Out<=G+CONFIDENCE_INTERVAL+2*step))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(32);
          PWM_RG_WriteCompare1(16);  
          RGBLed_Start();
        }  
        else if ((X_Out>G+CONFIDENCE_INTERVAL+2*step) && (X_Out<=G+CONFIDENCE_INTERVAL+4*step))
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
        if ((Y_Out>=G-CONFIDENCE_INTERVAL) && (Y_Out<=G+CONFIDENCE_INTERVAL))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(255);
          PWM_RG_WriteCompare2(255);
          RGBLed_Start();
        }
        else if((Y_Out>G+CONFIDENCE_INTERVAL) && (Y_Out<=G+CONFIDENCE_INTERVAL+step))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(65);
          PWM_RG_WriteCompare2(32);  
          RGBLed_Start();
        }   
        else if ((Y_Out>G+CONFIDENCE_INTERVAL+step) && (Y_Out<=G+CONFIDENCE_INTERVAL+2*step))
        {
          RGBLed_Stop();
          PWM_RG_WritePeriod(32);
          PWM_RG_WriteCompare2(16);  
          RGBLed_Start();
        }  
        else if ((Y_Out>G+CONFIDENCE_INTERVAL+2*step) && (Y_Out<=G+CONFIDENCE_INTERVAL+4*step))
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
        if ((Z_Out>=G-CONFIDENCE_INTERVAL) && (Z_Out<=G+CONFIDENCE_INTERVAL))
        {
          RGBLed_Stop();
          PWM_B_WritePeriod(255);
          PWM_B_WriteCompare(255);
          RGBLed_Start();
        }
        else if((Z_Out>G+CONFIDENCE_INTERVAL) && (Z_Out<=G+CONFIDENCE_INTERVAL+step))
        {
          RGBLed_Stop();
          PWM_B_WritePeriod(65);
          PWM_B_WriteCompare(32); 
          RGBLed_Start();
        }   
        else if ((Z_Out>G+CONFIDENCE_INTERVAL+step) && (Z_Out<=G+CONFIDENCE_INTERVAL+2*step))
        {
          RGBLed_Stop();
          PWM_B_WritePeriod(32);
          PWM_B_WriteCompare(16);
          RGBLed_Start();
        }  
        else if ((Z_Out>G+CONFIDENCE_INTERVAL+2*step) && (Z_Out<=G+CONFIDENCE_INTERVAL+4*step))
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
