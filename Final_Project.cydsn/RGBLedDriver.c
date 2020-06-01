/* ========================================
 * Source code to implement the different
 * blinking frequencies of the RGB Led 
 * in relation to the current value of
 * the acceleration data acquired.
 *
 * ========================================
*/
#include  "RGBLedDriver.h"
#include  "project.h"
#include  "SettingParameter.h"

#define G 1000     // gravitational acceleratiion in mg
#define LEVELS 3   // number of intervals used to subdivide the FSR after gravitational accel.
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
    /* Calculate the interval width in order to divide the fullscale range 
    (after the g value) into 3 intervals */
    step=(fullscale_range-(G+confidence_interval))/LEVELS;
    
    /* Verify which axis measures g acceleration (whithin a confidence interval) 
    to set the direction of the device */
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
        /* Turn off green and blue channels */
        RGBLed_Stop();
        PWM_RG_WritePeriod(255);
        PWM_RG_WriteCompare2(0);
        PWM_B_WritePeriod(255);
        PWM_B_WriteCompare(0);
        RGBLed_Start();
        if ((X_Out>=G-confidence_interval) && (X_Out<=G+confidence_interval))
        {
          /* Steady red light if the device is still and x-axis measures gravitational accel. */
          RGBLed_Stop();
          PWM_RG_WritePeriod(255);
          PWM_RG_WriteCompare1(255);
          RGBLed_Start();
        }
        
        else if((X_Out>G+confidence_interval) && (X_Out<=G+confidence_interval+step))
        {
          /* Blinking of red light with a period of 0.5 sec */  
          RGBLed_Stop();
          PWM_RG_WritePeriod(65);
          PWM_RG_WriteCompare1(32);  
          RGBLed_Start();
        }   
        else if ((X_Out>G+confidence_interval+step) && (X_Out<=G+confidence_interval+2*step))
        {
          /* Blinking of red light with a period of 0.25 sec */   
          RGBLed_Stop();
          PWM_RG_WritePeriod(32);
          PWM_RG_WriteCompare1(16);  
          RGBLed_Start();
        }  
        else if ((X_Out>G+confidence_interval+2*step) && (X_Out<=G+confidence_interval+3*step))
        {
          /* Blinking of red light with a period of 0.125 sec */   
          RGBLed_Stop();
          PWM_RG_WritePeriod(16);
          PWM_RG_WriteCompare1(8);
          RGBLed_Start();
        }  
        break;
        
        case Y:
        /* Turn off red and blue channels */
        RGBLed_Stop();
        PWM_RG_WritePeriod(255);
        PWM_RG_WriteCompare1(0);
        PWM_B_WritePeriod(255);
        PWM_B_WriteCompare(0);
        RGBLed_Start();
        if ((Y_Out>=G-confidence_interval) && (Y_Out<=G+confidence_interval))
        {
          /* Steady green light if the device is still and y-axis measures gravitational accel. */  
          RGBLed_Stop();
          PWM_RG_WritePeriod(255);
          PWM_RG_WriteCompare2(255);
          RGBLed_Start();
        }
        else if((Y_Out>G+confidence_interval) && (Y_Out<=G+confidence_interval+step))
        {
          /* Blinking of green light with a period of 0.5 sec */  
          RGBLed_Stop();
          PWM_RG_WritePeriod(65);
          PWM_RG_WriteCompare2(32);  
          RGBLed_Start();
        }   
        else if ((Y_Out>G+confidence_interval+step) && (Y_Out<=G+confidence_interval+2*step))
        {
          /* Blinking of green light with a period of 0.25 sec */  
          RGBLed_Stop();
          PWM_RG_WritePeriod(32);
          PWM_RG_WriteCompare2(16);  
          RGBLed_Start();
        }  
        else if ((Y_Out>G+confidence_interval+2*step) && (Y_Out<=G+confidence_interval+3*step))
        {
          /* Blinking of green light with a period of 0.125 sec */  
          RGBLed_Stop();
          PWM_RG_WritePeriod(16);
          PWM_RG_WriteCompare2(8); 
          RGBLed_Start();
        }  
        break;
        
        case Z:
        /* Turn off red and green channels */
        RGBLed_Stop();
        PWM_RG_WritePeriod(255);
        PWM_RG_WriteCompare1(0);
        PWM_RG_WriteCompare2(0);
        RGBLed_Start();
        if ((Z_Out>=G-confidence_interval) && (Z_Out<=G+confidence_interval))
        {
          /* Steady blue light if the device is still and z-axis measures gravitational accel. */  
          RGBLed_Stop();
          PWM_B_WritePeriod(255);
          PWM_B_WriteCompare(255);
          RGBLed_Start();
        }
        else if((Z_Out>G+confidence_interval) && (Z_Out<=G+confidence_interval+step))
        {
          /* Blinking of blue light with a period of 0.5 sec */
          RGBLed_Stop();
          PWM_B_WritePeriod(65);
          PWM_B_WriteCompare(32); 
          RGBLed_Start();
        }   
        else if ((Z_Out>G+confidence_interval+step) && (Z_Out<=G+confidence_interval+2*step))
        {
          /* Blinking of blue light with a period of 0.25 sec */  
          RGBLed_Stop();
          PWM_B_WritePeriod(32);
          PWM_B_WriteCompare(16);
          RGBLed_Start();
        }  
        else if ((Z_Out>G+confidence_interval+2*step) && (Z_Out<=G+confidence_interval+3*step))
        {
          /* Blinking of blue light with a period of 0.125 sec */  
          RGBLed_Stop();
          PWM_B_WritePeriod(16);
          PWM_B_WriteCompare(8);  
          RGBLed_Start();
        }  
        break;
    }
    
}


/* [] END OF FILE */
