/* ========================================
 * \file main.c
 * ========================================
*/

// Include header files
#include "I2C_Interface.h"
#include "project.h"
#include "stdio.h"
#include "RGBLedDriver.h"
#include "InterruptRoutines.h"
#include "SW_State.h"
#include "stdlib.h"
#include <string.h>
#include "25LC256.h"
#include "SettingParameter.h"

/* -------------------------------------------------
  BRIEF MACROS FOR LIS3DH REGISTERS CONFIGURATION
  ------------------------------------------------ */
//Brief slave device address
#define LIS3DH_DEVICE_ADDRESS 0x18

//Brief WHO AM I register address
#define LIS3DH_WHO_AM_I_REG_ADDR 0x0F

//Brieg CONTROL REGISTER 1 address
#define LIS3DH_CTRL_REG1 0x20

/*Brief HEX value for CONTROL REGISTER 1: Low power mode at 50 Hz
CTRL_REG1[3]=LPen=1 (Low power mode enable);
CTRL_REG1[7:4]=ODR[3:0]=0100 (50 Hz) */
#define LIS3DH_LP_MODE_50_HZ_CTRL_REG1 0x4F

//Brieg CONTROL REGISTER 3 address
#define LIS3DH_CTRL_REG3 0x22

/*Brief HEX value for CONTROL REGISTER 3: 
CTRL_REG3[1]=I1_OVERRUN=1 (FIFO overrun interrupt on INT1 enable) */
#define LIS3DH_OVERRUN_INT_CTRL_REG3 0x02 

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

//Brief CONTROL REGISTER 5 address
#define LIS3DH_CTRL_REG5 0x24

/*Brief HEX value for CONTROL REGISTER 5:
CTRL_REG5[6]=FIFO_EN=1 (enable FIFO buffer)
CTRL_REG5[1]=LIR_INT2=1 (Latch interrupt request on INT2_SRC register)*/
#define LIS3DH_FIFO_ENABLED 0x42

//Brief CONTROL REGISTER 6 address
#define LIS3DH_CTRL_REG6 0x25

/*Brief HEX value for CONTROL REGISTER 6:
CTRL_REG6[5]=I2_IA2=1 (Enable interrupt 2 function on INT2 pin)*/
#define LIS3DH_I2_IA2 0x20 

//Brief FIFO CONTROL REGISTER address
#define LIS3DH_FIFO_CTRL_REG 0x2E

/*Brief HEX value for FIFO CONTROL REGISTER in FIFO MODE:
FIFO_CTRL_REG[7:6]=FM[1:0]=01 (FIFO mode)*/
#define LIS3DH_FIFO_MODE 0x40  

/*Brief HEX value for FIFO CONTROL REGISTER in BYPASS MODE:
FIFO_CTRL_REG[7:6]=FM[1:0]=00 (BYPASS mode)*/
#define LIS3DH_BYPASS_MODE 0x00 

//Brief FIFO SRC REGISTER address
#define LIS3DH_FIFO_SRC_REG 0x2F

//Brief INT1 SRC register address
#define LIS3DH_INT1_SRC 0x31

//Brief INT2 CONFIGURATION register address
#define LIS3DH_INT2_CFG 0x34

/*Brief HEX value for IN2 CONFIGURATION register:
INT2_CFG[5]=ZHIE=1;
INT2_CFG[3]=YHIE=1;
INT2_CFG[1]=XHIE=1 (Interrupt request on measured accel. value higher than preset
threshold)
*/
#define LIS3DH_INT2_HIGH_EVENT 0x2A

//Brief INT2 SRC register address
#define LIS3DH_INT2_SRC 0x35

//Brief INT2 THRESHOLD register address
#define LIS3DH_INT2_THS 0x36

/*Brief HEX value for INT2 THRESHOLD register for the different fullscale ranges: 
*/
#define LIS3DH_THRESHOLD_EVENTS_2g  0x5E   // about 1500g
#define LIS3DH_THRESHOLD_EVENTS_4g  0x2F   // about 1500g
#define LIS3DH_THRESHOLD_EVENTS_8g  0x18   // about 1500g
#define LIS3DH_THRESHOLD_EVENTS_16g 0x08   // about 1500g

//Brief INT2 DURATION register address
#define LIS3DH_INT2_DURATION 0x37 

/*Brief HEX value for INT2 DURATION register:
    0.3 sec -> 0x03 
*/
#define LIS3DH_DURATION_EVENT 0x03         

//Brief OUT_X_L register address (x-axis output LSB)
#define LIS3DH_OUT_X_L 0x28

//Brief OUT_Y_L register address (y-axis output LSB)
#define LIS3DH_OUT_Y_L 0x2A

//Brief OUT_Z_L register address (z-axis output LSB)
#define LIS3DH_OUT_Z_L 0x2C

/*Brief HEX value for over threshold data (INT2_SRC register):
INT2_SRC[6]=IA=1 (an interrupt has been generated)
INT2_SRC[1]=XH=1 if measured acc. is higher that threshold
INT2_SRC[3]=YH=1 if measured acc. is higher that threshold
INT2_SRC[5]=ZH=1 if measured acc. is higher that threshold
*/
#define X_HIGH 0x42
#define Y_HIGH 0x48
#define Z_HIGH 0x60

/*Mask for the OVR_FIFO bit in the FIFO_SRC register:
FIFO_SRC[6]=OVR_FIFO=1 */
#define OVR_FIFO 0x04



int main(void)
{
    CyGlobalIntEnable; 
    
    /* -------------------------------------
	           INITIALIZATION
	---------------------------------------- */
    
    
    I2C_Peripheral_Start();
    UART_Start();
    
    RGBLed_Start();
    PWM_ONBOARD_Start();
   
    ISR_SW_StartEx(Switch_ISR);
    isr_TimePassed_StartEx(TIME_ISR);
    
    Timer_SW_Start();
    Timer_CountTime_Start();
    
    SPIM_Start();
    ADC_DelSig_Start();
    
    // Initialization of the variables
    button_pressed  = 0;
    click_count     = 0;
    time_passed     = 0;
    
    //"The boot procedure is complete about 5 milliseconds after device power-up."
    CyDelay(5); 
   
    /* -----------------------------------------
		READING AND WRITING OF LIS3DH REGISTERS
		---------------------------------------- */
    
    char message[50];
    
    // CONTROL REGISTER 1
    uint8_t ctrl_reg1; 
    ErrorCode error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &ctrl_reg1);
     if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 1: 0x%02X\r\n", ctrl_reg1);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read control register 1\r\n");   
    }
    

    if (ctrl_reg1 != LIS3DH_LP_MODE_50_HZ_CTRL_REG1)
    {
        ctrl_reg1 = LIS3DH_LP_MODE_50_HZ_CTRL_REG1;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_CTRL_REG1,
                                             ctrl_reg1);  
        
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 1 successfully written as: 0x%02X\r\n", ctrl_reg1);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set control register 1\r\n");   
        }
    }
    
    // CONTROL REGISTER 3
    uint8_t ctrl_reg3; 
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG3,
                                        &ctrl_reg3);
     if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 3: 0x%02X\r\n", ctrl_reg3);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read control register 3\r\n");   
    }
    

    if (ctrl_reg3 != LIS3DH_OVERRUN_INT_CTRL_REG3)
    {
        ctrl_reg3 = LIS3DH_OVERRUN_INT_CTRL_REG3;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_CTRL_REG3,
                                             ctrl_reg3);  
        
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 3 successfully written as: 0x%02X\r\n", ctrl_reg3);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set control register 3\r\n");   
        }
    }
    
    //CONTROL REGISTER 5  
    uint8_t ctrl_reg5;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG5,
                                        &ctrl_reg5); 
     if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 5: 0x%02X\r\n", ctrl_reg5);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read control register 5\r\n");   
    }
    
    
    if (ctrl_reg5 != LIS3DH_FIFO_ENABLED)
    {
        ctrl_reg5 = LIS3DH_FIFO_ENABLED;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG5,
                                         ctrl_reg5);
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 5 successfully written as: 0x%02X\r\n", ctrl_reg5);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set control register 5\r\n");   
        }
    }
    
    //CONTROL REGISTER 6  
    uint8_t ctrl_reg6;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG6,
                                        &ctrl_reg6); 
     if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 6: 0x%02X\r\n", ctrl_reg6);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read control register 6\r\n");   
    }
    
    
    if (ctrl_reg6 != LIS3DH_I2_IA2)
    {
        ctrl_reg6 = LIS3DH_I2_IA2;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG6,
                                         ctrl_reg6);
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 6 successfully written as: 0x%02X\r\n", ctrl_reg6);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set control register 6\r\n");   
        }
    }
    
    //FIFO CONTROL REGISTER
    uint8_t fifo_ctrl_reg;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_FIFO_CTRL_REG,
                                        &fifo_ctrl_reg); 
     if (error == NO_ERROR)
    {
        sprintf(message, "FIFO CONTROL REGISTER: 0x%02X\r\n", fifo_ctrl_reg);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read FIFO control register \r\n");   
    }
    
    
    if (fifo_ctrl_reg != LIS3DH_FIFO_MODE)
    {
        fifo_ctrl_reg = LIS3DH_FIFO_MODE;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_FIFO_CTRL_REG,
                                         fifo_ctrl_reg);
        if (error == NO_ERROR)
        {
            sprintf(message, "FIFO CONTROL REGISTER successfully written as: 0x%02X\r\n", fifo_ctrl_reg);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set FIFO control register \r\n");   
        }
    }
    
	//INT2 CONFIGURATION REGISTER
    uint8_t int2__cfg_reg;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_INT2_CFG,
                                        &int2__cfg_reg); 
     if (error == NO_ERROR)
    {
        sprintf(message, "INT2 CONFIGURATION REGISTER: 0x%02X\r\n", int2__cfg_reg);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read INT2 CONFIGURATION REGISTER \r\n");   
    }
    
    
    if (int2__cfg_reg != LIS3DH_INT2_HIGH_EVENT)
    {
        int2__cfg_reg = LIS3DH_INT2_HIGH_EVENT;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_INT2_CFG,
                                         int2__cfg_reg);
        if (error == NO_ERROR)
        {
            sprintf(message, "INT2 CONFIGURATION REGISTER successfully written as: 0x%02X\r\n", int2__cfg_reg);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set INT2 CONFIGURATION REGISTER \r\n");   
        }
    }
    
    
    //INT2 DURATION REGISTER
    uint8_t int2_duration_reg;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_INT2_DURATION,
                                        &int2_duration_reg); 
     if (error == NO_ERROR)
    {
        sprintf(message, "INT2 DURATION REGISTER: 0x%02X\r\n", int2_duration_reg);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read INT2 DURATION REGISTER \r\n");   
    }
    
    
    if (int2_duration_reg != LIS3DH_DURATION_EVENT)
    {
        int2_duration_reg = LIS3DH_DURATION_EVENT;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_INT2_DURATION,
                                         int2_duration_reg);
        if (error == NO_ERROR)
        {
            sprintf(message, "INT2 DURATION REGISTER successfully written as: 0x%02X\r\n", int2_duration_reg);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set INT2 DURATION REGISTER \r\n");   
        }
    }

    /* ----------------------------------------
			BRIEF VARIABLES FOR ACQUISITION
            OF ACCEL. OUTPUT 
		--------------------------------------- */
    
    int16_t X_Out[32];
    int16_t Y_Out[32];
    int16_t Z_Out[32];
    int16_t X_Out_mg[32];
    int16_t Y_Out_mg[32];
    int16_t Z_Out_mg[32];
    uint8_t OutArray[12]; 
    uint8_t AccelerationData[32][6];
    
    #define  HEADER 	0xA0;
    #define  FOOTER 	0xC0;
    
    OutArray[0] = HEADER;
    OutArray[11] = FOOTER;
    
    uint8_t fifo_src_reg;
    uint8_t int2_src_reg;
    uint8_t ctrl_reg4;
    uint8_t int2_ths_reg;
    
    uint8_t i=0;
    uint8_t Parameters_read;
    char buffer_EEPROM[100];
    
    #define N_SAMPLES 6
    #define DATA_BYTES 12
    int16_t Data_read[N_SAMPLES]={0};
    int16_t data[N_SAMPLES];
    uint8_t nsamples  = 0;
    uint8_t overths_x = 0;
    uint8_t overths_y = 0;
    uint8_t overths_z = 0;
    uint8_t count     = 0;
    
    uint8_t flag;
    uint8_t old_flag;
    

    
    /* When the device is turned on the state is STOP and the parameters are read from EEPROM */
    int state=STOP;
    Parameters=EEPROM_readByte(0x0000);
    Parameters=Parameters>>4;
    if (Parameters==0) 
    {
        sensitivity=16;
        fullscale_range=2000;
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
    else if (Parameters==1) 
    {
        sensitivity=32;
        fullscale_range=4000;
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
    else if (Parameters==2) 
    {
        sensitivity=64;
        fullscale_range=8000;
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
    else if (Parameters==3)
    {
        sensitivity=192;
        fullscale_range=16000;
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
    Parameters=(Parameters<<4)|state;
    
    for(;;)
    {                       
        if (button_pressed == 1)
        {
            /* Call to the function that determines the state of the device*/
            state = Switch_State();
        }
        
        switch (state)
        {
            /* START */
            case (0):
            /* Update parameters and state and save in memory */
            Parameters=Parameters>>4;
            Parameters=(Parameters<<4)|state;
            EEPROM_writeByte(0x0000,Parameters);
            EEPROM_waitForWriteComplete();
            Parameters_read=EEPROM_readByte(0x0000);
            
            /* Turn on on-board LED */
            PWM_ONBOARD_Stop();
            PWM_ONBOARD_WriteCompare(255);
            PWM_ONBOARD_Start();
            
            /* Read FIFO_SRC registers to check for overrun event */
            error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_FIFO_SRC_REG,
                                         &fifo_src_reg); 
            
			if ((error == NO_ERROR) && ((fifo_src_reg) & OVR_FIFO))
			{

                for(i=0;i<32;i++)
				{
					/* Read output registers - retrieval accel. data (X,Y,Z axis) */
					error=I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
													LIS3DH_OUT_X_L, 6,
													AccelerationData[i]);
					if (error == NO_ERROR)
					{
                
						/* Conversion of output data into right-justified 16 bit int (x-axis) */
						X_Out[i]=(int16)((AccelerationData[i][0] | (AccelerationData[i][1] << 8))) >> 8;
                        // Scaling to mg
                        X_Out_mg[i]=X_Out[i]*sensitivity;
					    // Absolute value
						X_Out[i]=abs(X_Out_mg[i]);
																		
						
						/* Conversion of output data into right-justified 16 bit int (y-axis) */
						Y_Out[i]=(int16)((AccelerationData[i][2] | (AccelerationData[i][3] << 8))) >> 8;
						// Scaling to mg
                        Y_Out_mg[i]=Y_Out[i]*sensitivity;
                        // Absolute value
						Y_Out[i]=abs(Y_Out_mg[i]);
				
						
						/* Conversion of output data into right-justified 16 bit int (z-axis) */
						Z_Out[i]=(int16)(AccelerationData[i][4] | (AccelerationData[i][5] << 8)) >> 8;
						// Scaling to mg
                        Z_Out_mg[i]=Z_Out[i]*sensitivity;
                        // Absolute value
						Z_Out[i]=abs(Z_Out_mg[i]);
					
                        
                        
                        /* Set RGB LED blinking frequency */
                        set_PWM(X_Out[i],Y_Out[i],Z_Out[i]);
                        
                        
                        /* Verify over threshold events in the 3 axis */
                        error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_INT2_SRC,
                                             &int2_src_reg);
                        
                        // Over-threshold in x-axis
                        if ((error == NO_ERROR) && (int2_src_reg & (X_HIGH)))
                        {                  
                            // Set the flag overths_x
                            overths_x=1;
                        }
                        if (overths_x)
                        { 
                            // Save 5 samples and timestamp of the event in memory
                            data[nsamples]=X_Out_mg[i];
                            nsamples++;
                            if (nsamples > N_SAMPLES-2)
                                {
                                    data[5]=time_passed;
                                    nsamples=0;
                                    overths_x=0;
                                  
                                    EEPROM_writePage(0x0002,(uint8_t*)data,DATA_BYTES);
                                    EEPROM_waitForWriteComplete();                                
                                    
                                    EEPROM_readPage(0x0002,(uint8_t*)Data_read,DATA_BYTES);
                                    
                                    /* Print 5 overthreshold samples and timestamp */
                                    sprintf(message, "Over threshold data: %d %d %d %d %d Timestamp: %d seconds\r\n", Data_read[0],Data_read[1],Data_read[2],Data_read[3],Data_read[4],Data_read[5]);
            						UART_PutString(message);
                                }                          
                        }
                        // Over-threshold in y-axis
                        else if ((error == NO_ERROR) && (int2_src_reg & (Y_HIGH)))
                        {                  
                            // Set the flag overths_y
                            overths_y=1;
                        }
                        if (overths_y)
                        {
                            // Save 5 samples and timestamp of the event in memory
                            data[nsamples]=Y_Out_mg[i];
                            nsamples++;
                            if (nsamples > N_SAMPLES-2)
                                {
                                    data[5]=time_passed;
                                    nsamples=0;
                                    overths_y=0;
                                  
                                    EEPROM_writePage(0x0002,(uint8_t*)data,DATA_BYTES);
                                    EEPROM_waitForWriteComplete();                                
                                    
                                    EEPROM_readPage(0x0002,(uint8_t*)Data_read,DATA_BYTES);
                                    
                                    /* Print 5 overthreshold samples and timestamp */
                                    sprintf(message, "Over threshold data: %d %d %d %d %d Timestamp: %d seconds\r\n", Data_read[0],Data_read[1],Data_read[2],Data_read[3],Data_read[4],Data_read[5]);
            						UART_PutString(message);
                                }                          
                        }
                        // Over-threshold in z-axis
                        else if ((error == NO_ERROR) && (int2_src_reg & (Z_HIGH)))
                        {                  
                            // Set the flag overths_z
                            overths_z=1;
                        }
                        if (overths_z)
                        {
                            // Save 5 samples and timestamp of the event in memory
                            data[nsamples]=Z_Out_mg[i];
                            nsamples++;
                            if (nsamples > N_SAMPLES-2)
                                {
                                    data[5]=time_passed;
                                    nsamples=0;
                                    overths_z=0;
                                  
                                    EEPROM_writePage(0x0002,(uint8_t*)data,DATA_BYTES);
                                    EEPROM_waitForWriteComplete();                                
                                    
                                    EEPROM_readPage(0x0002,(uint8_t*)Data_read,DATA_BYTES);
                                    
                                    /* Print 5 overthreshold samples and timestamp */
                                    sprintf(message, "Over threshold data: %d %d %d %d %d Timestamp: %d seconds\r\n", Data_read[0],Data_read[1],Data_read[2],Data_read[3],Data_read[4],Data_read[5]);
            						UART_PutString(message);
                                }                          
                        }
                                                                       
					/* Code to plot data on the Bridge Control Panel*/
//                      MSB (x-axis)
//						OutArray[1]=(uint8_t)(X_Out_mg[i] >> 8);
//						LSB (x-axis)
//						OutArray[2]=(uint8_t)(X_Out_mg[i] & 0xFF);
// 					    MSB (y-axis)
//						OutArray[3]=(uint8_t)(Y_Out_mg[i] >> 8);
//						LSB (y-axis)
//					    OutArray[4]=(uint8_t)(Y_Out_mg[i] & 0xFF);
//						MSB (z-axis)
//						OutArray[5]=(uint8_t)(Z_Out_mg[i] >> 8);
//						LSB (z-axis)
//						OutArray[6]=(uint8_t)(Z_Out_mg[i] & 0xFF);
//                        
//                      // Read the value of the threshold and save both positive and negative values
//                      uint8_t int2_ths;
//                      error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
//                                             LIS3DH_INT2_THS,
//                                             &int2_ths);
//                      int16_t threshold;
//                      threshold=int2_ths*sensitivity;
//						OutArray[7]=(uint8_t)(threshold >> 8);
//						OutArray[8]=(uint8_t)(threshold & 0xFF);
//                      OutArray[9]= -OutArray[7];
//                      OutArray[10]= -OutArray[8];
//						
//                      UART_PutArray(OutArray,12);
						
                        /* Print acceleration data and current parameters */
                        sprintf(message, "Acceleration data: %d %d %d \r\n Parameters: 0x%02X (0x%02X)\r\n\n", X_Out_mg[i],Y_Out_mg[i],Z_Out_mg[i], Parameters_read, Parameters);
						UART_PutString(message);
                        
						CyDelay(5);
					}
				}
			
				/* Enable Bypass mode */
				fifo_ctrl_reg = LIS3DH_BYPASS_MODE;
				error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
													 LIS3DH_FIFO_CTRL_REG,
													 fifo_ctrl_reg); 
        
				
				/* Enable FIFO mode again for next reading */
				fifo_ctrl_reg = LIS3DH_FIFO_MODE;
				error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
													LIS3DH_FIFO_CTRL_REG,
													fifo_ctrl_reg);             
			}
            break;
            
            /* STOP */
            case(1):
            /* Update parameters and state and save in memory */
            Parameters=Parameters>>4;
            Parameters=(Parameters<<4)|state;
            EEPROM_writeByte(0x0000,Parameters);
            EEPROM_waitForWriteComplete();
            Parameters_read=EEPROM_readByte(0x0000);
            
            /* Print current parameters */
            sprintf(buffer_EEPROM," EEPROM Read Parameters = 0x%02X (0x%02X)\r\n", Parameters_read, Parameters);
            UART_PutString(buffer_EEPROM);
            
            /* Turn off on-board LED */
            PWM_ONBOARD_Stop();
            PWM_ONBOARD_WriteCompare(0);
            PWM_ONBOARD_Start();
            
            PWM_RG_WriteCompare1(255);
            PWM_RG_WriteCompare2(0);
            PWM_B_WriteCompare(255);

            break;
            
            /* CONFIGURATION MODE */
            case(2):
            /* Blinking of the on-board LED */
            PWM_ONBOARD_Stop();
            PWM_ONBOARD_WriteCompare(127);
            PWM_ONBOARD_Start();
            
            PWM_RG_WriteCompare1(255);
            PWM_RG_WriteCompare2(0);
            PWM_B_WriteCompare(255);
            
            /* Read from memory the value of the previous flag */
            old_flag=EEPROM_readByte(0x0001);
            
            /* Toggle the pin FLAG to know the value of the flag and save it in memory */
            flag = FLAG_Read();
            EEPROM_writeByte(0x0001,flag);
            EEPROM_waitForWriteComplete();
            
            
            if (flag == 1)
            {
                /* Read values of parameters saved in memory */
                Parameters_read=EEPROM_readByte(0x0000);
                Parameters_read=Parameters_read>>4;
                Parameters_read=(Parameters_read<<4)|state;
                
                /* Set parameters (full-scale range) with potentiometer */
                setParameter();
                
                /* Update parameters and state */
                Parameters=(Parameters<<4)|state;
                EEPROM_writeByte(0x0000,Parameters);
                EEPROM_waitForWriteComplete();
                
                /*Check if potentiometer has been moved during read-only modality */
                if(Parameters!=Parameters_read && (old_flag!=flag))
                {
                  sprintf(buffer_EEPROM,"**ATTENTION: Position of potentiometer changed during read-only modality**\r\n\n");
                  UART_PutString(buffer_EEPROM);
                }
                
                /* Save new parameters in memory and print */
                Parameters_read=EEPROM_readByte(0x0000);
                sprintf(buffer_EEPROM," EEPROM Read Parameters = 0x%02X (0x%02X)\r\n", Parameters_read, Parameters);
                UART_PutString(buffer_EEPROM);
            } else {
                
                /* Update parameters and state and save in memory */
                Parameters_read=EEPROM_readByte(0x0000);
                Parameters_read=Parameters_read>>4;
                Parameters_read=(Parameters_read<<4)|state;

                sprintf(message, "****WARNING: configuration menu is in read-only modality****\r\n");
                UART_PutString(message);
                sprintf(buffer_EEPROM," EEPROM Read Parameters = 0x%02X\r\n\n", Parameters_read);
                UART_PutString(buffer_EEPROM); 
            }
            break;
        }
    }
}
/* [] END OF FILE */
