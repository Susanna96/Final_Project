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
#include  "SettingParameter.h"
#include "project.h"
#include "I2C_Interface.h"
#include "stdio.h"

//Brief slave device address
#define LIS3DH_DEVICE_ADDRESS 0x18
//Brief CONTROL REGISTER 4 address
#define LIS3DH_CTRL_REG4 0x23

/*Brief HEX value for CONTROL REGISTER 4: +- 2.0 g FSR
CTRL_REG4[3]=0 (High resolution disabled)
CTRL_REG4[5:4]=FS[1:0]=00 (2.0 g FSR) */
#define LIS3DH_CTRL_REG4_2g 0x80

/*Brief HEX value for CONTROL REGISTER 4: +- 4.0 g FSR
CTRL_REG4[3]=0 (High resolution disabled)
CTRL_REG4[5:4]=FS[1:0]=01 (4.0 g FSR) */
#define LIS3DH_CTRL_REG4_4g 0x90

/*Brief HEX value for CONTROL REGISTER 4: +- 8.0 g FSR
CTRL_REG4[3]=0 (High resolution disabled)
CTRL_REG4[5:4]=FS[1:0]=10 (8.0 g FSR) */
#define LIS3DH_CTRL_REG4_8g 0xA0

/*Brief HEX value for CONTROL REGISTER 4: +- 16.0 g FSR
CTRL_REG4[3]=0 (High resolution disabled)
CTRL_REG4[5:4]=FS[1:0]=11 (16.0 g FSR) */
#define LIS3DH_CTRL_REG4_16g 0xB0

//Brief INT2 THRESHOLD register address
#define LIS3DH_INT2_THS 0x36

/*Brief HEX value for INT2 THRESHOLD register for the different fullscale ranges: 
*/
#define LIS3DH_THRESHOLD_EVENTS_2g  0x5E   // about 1500g
#define LIS3DH_THRESHOLD_EVENTS_4g  0x2F   // about 1500g
#define LIS3DH_THRESHOLD_EVENTS_8g  0x18   // about 1500g
#define LIS3DH_THRESHOLD_EVENTS_16g 0x08  // about 1500g

int32 value_digit_pot;
int16 value_pot;
ErrorCode error;
uint8_t ctrl_reg4;
uint8_t int2_ths_reg;
char message[50];

void setParameter(void)
{
    value_digit_pot=ADC_DelSig_Read32();
    
    if(value_digit_pot<0) value_digit_pot=0;
    if(value_digit_pot>65535) value_digit_pot=65535;
            
    value_pot=ADC_DelSig_CountsTo_mVolts(value_digit_pot);
    
    if((value_pot>=0) && (value_pot<1250))
    {
        Parameters=0x00;
        fullscale_range=2000;
        sensitivity=16;
        confidence_interval=30;
        ctrl_reg4 = LIS3DH_CTRL_REG4_2g;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG4,
                                         ctrl_reg4);
        int2_ths_reg = LIS3DH_THRESHOLD_EVENTS_2g;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_INT2_THS,
                                         int2_ths_reg);
    }
    else if((value_pot>=1250) && (value_pot<2500))
    {
        Parameters=0x01;
        fullscale_range=4000;
        sensitivity=32;
        confidence_interval=30;
        ctrl_reg4 = LIS3DH_CTRL_REG4_4g;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG4,
                                         ctrl_reg4);
        int2_ths_reg = LIS3DH_THRESHOLD_EVENTS_4g;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_INT2_THS,
                                         int2_ths_reg);
    }
    else if((value_pot>=2500) && (value_pot<3750))
    {
        Parameters=0x02;
        fullscale_range=8000;
        sensitivity=64;
        confidence_interval=40;
        ctrl_reg4 = LIS3DH_CTRL_REG4_8g;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG4,
                                         ctrl_reg4);
        int2_ths_reg = LIS3DH_THRESHOLD_EVENTS_8g;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_INT2_THS,
                                         int2_ths_reg);
    }
    else if((value_pot>=3750) && (value_pot<=5000))
    {
        Parameters=0x03;
        fullscale_range=16000;
        sensitivity=192;
        confidence_interval=50;
        ctrl_reg4 = LIS3DH_CTRL_REG4_16g;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG4,
                                         ctrl_reg4);
        int2_ths_reg = LIS3DH_THRESHOLD_EVENTS_16g;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_INT2_THS,
                                         int2_ths_reg);
    }
}

/* [] END OF FILE */
