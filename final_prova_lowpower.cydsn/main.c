/* ========================================
 * \file main.c
 *
 *
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
#include  "SettingParameter.h"

/* -------------------------------------------------
  BRIEF MACROS FOR LIS3DH REGISTERS CONFIGURATION
  ------------------------------------------------ */
//Brief slave device address
#define LIS3DH_DEVICE_ADDRESS 0x18

//Brief WHO AM I register address
#define LIS3DH_WHO_AM_I_REG_ADDR 0x0F

//Brieg CONTROL REGISTER 1 address
#define LIS3DH_CTRL_REG1 0x20

/*Brief HEX value for CONTROL REGISTER 1: High resolution mode at 100 Hz
CTRL_REG1[3]=LPen=1 (Low power mode);
CTRL_REG1[7:4]=ODR[3:0]=0101 (100 Hz)*/
#define LIS3DH_LP_MODE_100_HZ_CTRL_REG1 0x2F//0x5F

//Brieg CONTROL REGISTER 3 address
#define LIS3DH_CTRL_REG3 0x22

/*Brief HEX value for CONTROL REGISTER 2: 
CTRL_REG3[1]=I1_OVERRUN=1 (FIFO overrun interrupt on INT1 enabled); */
#define LIS3DH_OVERRUN_INT_CTRL_REG3 0x42 //0x42 if I1_IA1=1

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
FIFO_EN=1 (enable FIFO buffer)*/
#define LIS3DH_FIFO_ENABLED 0x45 //0x40

//Brief CONTROL REGISTER 6 address
#define LIS3DH_CTRL_REG6 0x25

/*Brief HEX value for CONTROL REGISTER 6:
I2_IA2=1 (Enable interrupt 2 function on INT2 pin.)*/
#define LIS3DH_I2_IA2 0x20 //0x20 if I2_IA2=1

//Brief FIFO CONTROL REGISTER address
#define LIS3DH_FIFO_CTRL_REG 0x2E

/*Brief HEX value for FIFO CONTROL REGISTER:
FM[1:0]=01 (FIFO mode)
TR=1 (Triggering signal on INT2)*/
#define LIS3DH_FIFO_MODE 0x40 //0x60 if TR=1 

/*Brief HEX value for FIFO CONTROL REGISTER:
FM[1:0]=00 (BYPASS mode)
TR=1 (Triggering signal on INT2)*/
#define LIS3DH_BYPASS_MODE 0x00 //0x20 if TR=1

//Brief FIFO SRC REGISTER address
#define LIS3DH_FIFO_SRC_REG 0x2F

//Brief INT1 SRC register address
#define LIS3DH_INT1_SRC 0x31

//Brief INT2 CONFIGURATION register address
#define LIS3DH_INT2_CFG 0x34

/*Brief HEX value for IN2 CONFIGURATION register:
ZHIE=YHIE=XHIE=1 (Interrupt request on measured accel. value higher than preset
threshold)
*/
#define LIS3DH_INT2_HIGH_EVENT 0x2A

//Brief INT2 SRC register address
#define LIS3DH_INT2_SRC 0x35

//Brief INT2 THRESHOLD register address
#define LIS3DH_INT2_THS 0x36

/*Brief HEX value for INT2 THRESHOLD register:
	TO BE DEFINED 
*/
#define LIS3DH_THRESHOLD_EVENTS 0x2F //1500g

//Brief INT2 DURATION register address
#define LIS3DH_INT2_DURATION 0x37 

/*Brief HEX value for INT2 DURATION register:
	TO BE DEFINED
*/
#define LIS3DH_DURATION_EVENT 0x03 // 3 sec

//Brief OUT_X_L register address (x-axis output LSB)
#define LIS3DH_OUT_X_L 0x28

//Brief OUT_Y_L register address (y-axis output LSB)
#define LIS3DH_OUT_Y_L 0x2A

//Brief OUT_Z_L register address (z-axis output LSB)
#define LIS3DH_OUT_Z_L 0x2C

#define X_HIGH 0x42
#define Y_HIGH 0x48
#define Z_HIGH 0x60



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
    isr_OVR_StartEx(OVR_ISR);
    Timer_SW_Start();
    SPIM_Start();
    ADC_DelSig_Start();
    

    button_pressed  = 0;
    click_count=0;
    
    //"The boot procedure is complete about 5 milliseconds after device power-up."
    CyDelay(5); 
   
    /* -----------------------------------------
		READING AND WRITING OF LIS3DH REGISTERS
		---------------------------------------- */
    
    char message[50];
    char buffer_EEPROM[100];
    
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
    

    if (ctrl_reg1 != LIS3DH_LP_MODE_100_HZ_CTRL_REG1)
    {
        ctrl_reg1 = LIS3DH_LP_MODE_100_HZ_CTRL_REG1;
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
    
    //INT2 THRESHOLD REGISTER
    uint8_t int2_ths_reg;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_INT2_THS,
                                        &int2_ths_reg); 
     if (error == NO_ERROR)
    {
        sprintf(message, "INT2 THRESHOLD REGISTER: 0x%02X\r\n", int2_ths_reg);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read INT2 THRESHOLD REGISTER \r\n");   
    }
    
    
    if (int2_ths_reg != LIS3DH_THRESHOLD_EVENTS)
    {
        int2_ths_reg = LIS3DH_THRESHOLD_EVENTS;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_INT2_THS,
                                         int2_ths_reg);
        if (error == NO_ERROR)
        {
            sprintf(message, "INT2 THRESHOLD REGISTER successfully written as: 0x%02X\r\n", int2_ths_reg);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set INT2 THRESHOLD REGISTER \r\n");   
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
    uint8_t OutArray[8]; 
    uint8_t AccelerationData[32][6];
    
    #define  HEADER 	0xA0;
    #define  FOOTER 	0xC0;
    
    OutArray[0] = HEADER;
    OutArray[7] = FOOTER;
    
    uint8_t fifo_src_reg;
    uint8_t int2_src_reg;
    uint8_t ctrl_reg4;
    uint8_t int1_src_reg;
    uint8_t i=0;
    uint8_t Parameters_read;
    
    #define N_SAMPLES 6
    #define DATA_BYTES 12
    int16_t Data_read[N_SAMPLES]={0};
    int16_t data[N_SAMPLES];
    uint8_t nsamples=0;
    uint8_t overths_x=0;
    uint8_t overths_y=0;
    uint8_t overths_z=0;
    uint8 count=0;
    
    uint8_t flag;
    uint8_t old_flag;
    
    int twenty_sec=0;
    
    /* When the device is turned on the state is STOP and the FSR is read from EEPROM */
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
    }
    Parameters=(Parameters<<4)|state;
    
    for(;;)
    {
        if (Timer_SW_ReadCounter()==0)
        { 
            twenty_sec++;
        }
        if (button_pressed == 1)
        {
            state = Switch_State();
        }
        switch (state)
        {
            case (0):
            Parameters=Parameters>>4;
            Parameters=(Parameters<<4)|state;
            EEPROM_writeByte(0x0000,Parameters);
            EEPROM_waitForWriteComplete();
            Parameters_read=EEPROM_readByte(0x0000);
            PWM_ONBOARD_Stop();
            PWM_ONBOARD_WriteCompare(255);
            PWM_ONBOARD_Start();
            error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_FIFO_SRC_REG,
                                         &fifo_src_reg);    
			if ((error == NO_ERROR) && ((fifo_src_reg) & (0x04)))
			{
//				error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
//                                         LIS3DH_INT1_SRC,
//                                         &int1_src_reg);
                for(i=0;i<32;i++)
				{
					// Read output registers - retrieval accel. data (X,Y,Z axis)
					error=I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
													LIS3DH_OUT_X_L, 6,
													AccelerationData[i]);
					if (error == NO_ERROR)
					{
                
						// Conversion of output data into right-justified 16 bit int (x-axis)
						X_Out[i]=(int16)((AccelerationData[i][0] | (AccelerationData[i][1] << 8))) >> 8;
                        /*scaling to mg*/
                        X_Out_mg[i]=X_Out[i]*sensitivity;
					    /*Absolute value*/
						X_Out[i]=abs(X_Out_mg[i]);
						
												
						//MSB (x-axis)
						//OutArray[1]=(uint8_t)(X_Out[i] >> 8);
						//LSB (x-axis)
						//OutArray[2]=(uint8_t)(X_Out[i] & 0xFF);
						
						// Conversion of output data into right-justified 16 bit int (y-axis)
						Y_Out[i]=(int16)((AccelerationData[i][2] | (AccelerationData[i][3] << 8))) >> 8;
						/*scaling to mg*/
                        Y_Out_mg[i]=Y_Out[i]*sensitivity;
                        /*Absolute value*/
						Y_Out[i]=abs(Y_Out_mg[i]);
						
						 
//						//MSB (x-axis)
//						OutArray[3]=(uint8_t)(Y_Out[i] >> 8);
//						//LSB (x-axis)
//						OutArray[4]=(uint8_t)(Y_Out[i] & 0xFF);
						
						// Conversion of output data into right-justified 16 bit int (z-axis)
						Z_Out[i]=(int16)(AccelerationData[i][4] | (AccelerationData[i][5] << 8)) >> 8;
						/*scaling to mg*/
                        Z_Out_mg[i]=Z_Out[i]*sensitivity;
                        /*Absolute value*/
						Z_Out[i]=abs(Z_Out_mg[i]);
					
						
//						//MSB (x-axis)
//						OutArray[5]=(uint8_t)(Z_Out[i] >> 8);
//						//LSB (x-axis)
//						OutArray[6]=(uint8_t)(Z_Out[i] & 0xFF);
                        
                        /*Set RGB LED blinking frequency*/
                        set_PWM(X_Out[i],Y_Out[i],Z_Out[i]);
                        
                        // Verify over threshold
                        error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_INT2_SRC,
                                             &int2_src_reg);
                        sprintf(message, "INT2 SRC REG: 0x%02X\r\n", int2_src_reg);
                        UART_PutString(message); 
                        if ((error == NO_ERROR) && (int2_src_reg & (X_HIGH)))
                        {                  
                            overths_x=1;
//                            int counter_ths=Timer_SW_ReadCounter();
//                            counter_ths=(20-counter_ths)+(20*twenty_sec);
//                            data[0]=counter_ths;
                            
                            //ths=0;
                        }
                        if (overths_x)
                        {
                                data[nsamples]=X_Out_mg[i];
                                nsamples++;
                                if (nsamples > N_SAMPLES-1)
                                {
                                    nsamples=0;
                                    overths_x=0;
                                  
                                    EEPROM_writePage(0x0002,(uint8_t*)data,DATA_BYTES);
                                    EEPROM_waitForWriteComplete();                                
                                    
                                    EEPROM_readPage(0x0002,(uint8_t*)Data_read,DATA_BYTES);
                                    sprintf(message, "Over threshold data: %d %d %d %d %d %d \r\n\n", Data_read[0],Data_read[1],Data_read[2],Data_read[3],Data_read[4],Data_read[5]);
            						UART_PutString(message);
                                }                          
                        }
                        else if ((error == NO_ERROR) && (int2_src_reg & (Y_HIGH)))
                        {                  
                            overths_y=1;
//                            int counter_ths=Timer_SW_ReadCounter();
//                            counter_ths=(20-counter_ths)+(20*twenty_sec);
//                            data[0]=counter_ths;
                            
                            //ths=0;
                        }
                        if (overths_y)
                        {
                                data[nsamples]=Y_Out_mg[i];
                                nsamples++;
                                if (nsamples > N_SAMPLES-1)
                                {
                                    nsamples=0;
                                    overths_y=0;
                                  
                                    EEPROM_writePage(0x0002,(uint8_t*)data,DATA_BYTES);
                                    EEPROM_waitForWriteComplete();                                
                                    
                                    EEPROM_readPage(0x0002,(uint8_t*)Data_read,DATA_BYTES);
                                    sprintf(message, "Over threshold data: %d %d %d %d %d %d \r\n\n", Data_read[0],Data_read[1],Data_read[2],Data_read[3],Data_read[4],Data_read[5]);
            						UART_PutString(message);
                                }                          
                        }
                        
                        else if ((error == NO_ERROR) && (int2_src_reg & (Z_HIGH)))
                        {                  
                            overths_z=1;
//                            int counter_ths=Timer_SW_ReadCounter();
//                            counter_ths=(20-counter_ths)+(20*twenty_sec);
//                            data[0]=counter_ths;
                            
                            //ths=0;
                        }
                        if (overths_z)
                        {
                                data[nsamples]=Z_Out_mg[i];
                                nsamples++;
                                if (nsamples > N_SAMPLES-1)
                                {
                                    nsamples=0;
                                    overths_z=0;
                                  
                                    EEPROM_writePage(0x0002,(uint8_t*)data,DATA_BYTES);
                                    EEPROM_waitForWriteComplete();                                
                                    
                                    EEPROM_readPage(0x0002,(uint8_t*)Data_read,DATA_BYTES);
                                    sprintf(message, "Over threshold data: %d %d %d %d %d %d \r\n\n", Data_read[0],Data_read[1],Data_read[2],Data_read[3],Data_read[4],Data_read[5]);
            						UART_PutString(message);
                                }                          
                        }
                                                                       
					
						//UART_PutArray(OutArray,8);
						sprintf(message, "Acceleration data: %d %d %d \r\n Parameters: 0x%02X (0x%02X)\r\n\n", X_Out_mg[i],Y_Out_mg[i],Z_Out_mg[i], Parameters_read, Parameters);
						UART_PutString(message);
                        
						CyDelay(5);
					}
				}
			
				// Enable Bypass mode
				fifo_ctrl_reg = LIS3DH_BYPASS_MODE;
				error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
													 LIS3DH_FIFO_CTRL_REG,
													 fifo_ctrl_reg); 
        
				//CyDelay(10);
				
				// Enable FIFO mode again for next reading
				fifo_ctrl_reg = LIS3DH_FIFO_MODE;
				error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
													LIS3DH_FIFO_CTRL_REG,
													fifo_ctrl_reg);
                
			}
            break;
            
            case(1):
            Parameters=Parameters>>4;
            Parameters=(Parameters<<4)|state;
            EEPROM_writeByte(0x0000,Parameters);
            EEPROM_waitForWriteComplete();
            Parameters_read=EEPROM_readByte(0x0000);
            sprintf(buffer_EEPROM," EEPROM Read Parameters = 0x%02X (0x%02X)\r\n", Parameters_read, Parameters);
            UART_PutString(buffer_EEPROM);
            PWM_ONBOARD_Stop();
            PWM_ONBOARD_WriteCompare(0);
            PWM_ONBOARD_Start();
            
            PWM_RG_WriteCompare1(255);
            PWM_RG_WriteCompare2(0);
            PWM_B_WriteCompare(255);
            break;
            
            case(2):
            PWM_ONBOARD_Stop();
            PWM_ONBOARD_WriteCompare(127);
            PWM_ONBOARD_Start();
            
            PWM_RG_WriteCompare1(255);
            PWM_RG_WriteCompare2(0);
            PWM_B_WriteCompare(255);
            
            old_flag=EEPROM_readByte(0x0001);
            flag = FLAG_Read();
            EEPROM_writeByte(0x0001,flag);
            EEPROM_waitForWriteComplete();
            
            
            if (flag == 1)
            {
                Parameters_read=EEPROM_readByte(0x0000);
                Parameters_read=Parameters_read>>4;
                Parameters_read=(Parameters_read<<4)|state;
                
                /* Set parameters (full-scale range) with potentiometer */
                setParameter();
        
                Parameters=(Parameters<<4)|state;
                EEPROM_writeByte(0x0000,Parameters);
                EEPROM_waitForWriteComplete();
                
                if(Parameters!=Parameters_read && (old_flag!=flag))
                {
                  sprintf(buffer_EEPROM,"**ATTENTION: Position of potentiometer changed during read-only modality**\r\n\n");
                  UART_PutString(buffer_EEPROM);
                }
                
                Parameters_read=EEPROM_readByte(0x0000);
                sprintf(buffer_EEPROM," EEPROM Read Parameters = 0x%02X (0x%02X)\r\n", Parameters_read, Parameters);
                UART_PutString(buffer_EEPROM);
            } else {
                
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
