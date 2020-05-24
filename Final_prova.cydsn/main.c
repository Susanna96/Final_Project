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
CTRL_REG1[3]=LPen=0 (HR mode);
CTRL_REG1[7:4]=ODR[3:0]=0101 (100 Hz)*/
#define LIS3DH_HR_MODE_100_HZ_CTRL_REG1 0x57

//Brieg CONTROL REGISTER 3 address
#define LIS3DH_CTRL_REG3 0x22

/*Brief HEX value for CONTROL REGISTER 2: 
CTRL_REG3[1]=I1_OVERRUN=1 (FIFO overrun interrupt on INT1 enabled); */
#define LIS3DH_OVERRUN_INT_CTRL_REG3 0x02

//Brief CONTROL REGISTER 4 address
#define LIS3DH_CTRL_REG4 0x23

/*Brief HEX value for CONTROL REGISTER 4: +- 2.0 g FSR
CTRL_REG4[3]=1 (High resolution enabled)
CTRL_REG4[5:4]=FS[1:0]=00 (2.0 g FSR) */
#define LIS3DH_CTRL_REG4_2g 0x88

/*Brief HEX value for CONTROL REGISTER 4: +- 4.0 g FSR
CTRL_REG4[3]=1 (High resolution enabled)
CTRL_REG4[5:4]=FS[1:0]=01 (4.0 g FSR) */
#define LIS3DH_CTRL_REG4_4g 0x98

/*Brief HEX value for CONTROL REGISTER 4: +- 8.0 g FSR
CTRL_REG4[3]=1 (High resolution enabled)
CTRL_REG4[5:4]=FS[1:0]=10 (8.0 g FSR) */
#define LIS3DH_CTRL_REG4_8g 0xA8

/*Brief HEX value for CONTROL REGISTER 4: +- 16.0 g FSR
CTRL_REG4[3]=1 (High resolution enabled)
CTRL_REG4[5:4]=FS[1:0]=11 (16.0 g FSR) */
#define LIS3DH_CTRL_REG4_16g 0xB8

//Brief CONTROL REGISTER 5 address
#define LIS3DH_CTRL_REG5 0x24

/*Brief HEX value for CONTROL REGISTER 5:
FIFO_EN=1 (enable FIFO buffer)*/
#define LIS3DH_FIFO_ENABLED 0x40

//Brief FIFO CONTROL REGISTER address
#define LIS3DH_FIFO_CTRL_REG 0x2E

/*Brief HEX value for FIFO CONTROL REGISTER:
FM[1:0]=01 (FIFO mode)
TR=1 (Triggering signal on INT2)*/
#define LIS3DH_FIFO_MODE 0x40//0x60

/*Brief HEX value for FIFO CONTROL REGISTER:
FM[1:0]=00 (BYPASS mode)
TR=1 (Triggering signal on INT2)*/
#define LIS3DH_BYPASS_MODE 0x00 //0x20

//Brief FIFO SRC REGISTER address
#define LIS3DH_FIFO_SRC_REG 0x2F

//Brief INT2 CONFIGURATION register address
#define LIS3DH_INT2_CFG 0x34

/*Brief HEX value for IN2 CONFIGURATION register:
ZHIE=YHIE=ZHIE=1 (Interrupt request on measured accel. value lower than preset
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
#define LIS3DH_THRESHOLD_EVENTS 

//Brief INT2 DURATION register address
#define LIS3DH_INT2_DURATION 0x37 

/*Brief HEX value for INT2 DURATION register:
	TO BE DEFINED
*/
#define LIS3DH_DURATION_EVENT 

//Brief OUT_X_L register address (x-axis output LSB)
#define LIS3DH_OUT_X_L 0x28

//Brief OUT_Y_L register address (y-axis output LSB)
#define LIS3DH_OUT_Y_L 0x2A

//Brief OUT_Z_L register address (z-axis output LSB)
#define LIS3DH_OUT_Z_L 0x2C

//Brief value of sensitivity in High Resolution mode (2 mg/digit)
#define HR_SENSITIVITY 2

//Brief value of absolute value of full scale range in mg (4000 mg)
#define FS 4000

int state = 1;

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
    Timer_SW_Start();
    

    button_pressed  = 0;
    click_count=0;
    
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
    

    if (ctrl_reg1 != LIS3DH_HR_MODE_100_HZ_CTRL_REG1)
    {
        ctrl_reg1 = LIS3DH_HR_MODE_100_HZ_CTRL_REG1;
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
   
    //CONTROL REGISTER 4   
    uint8_t ctrl_reg4;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG4,
                                        &ctrl_reg4);
     if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 4: 0x%02X\r\n", ctrl_reg4);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read control register 4\r\n");   
    }
    
   
    if (ctrl_reg4 != LIS3DH_CTRL_REG4_2g)
    {
        ctrl_reg4 = LIS3DH_CTRL_REG4_4g;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG4,
                                         ctrl_reg4);
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 4 successfully written as: 0x%02X\r\n", ctrl_reg4);
            UART_PutString(message); 
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to set control register 4\r\n");   
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

    /* ----------------------------------------
			BRIEF VARIABLES FOR ACQUISITION
            OF ACCEL. OUTPUT 
		--------------------------------------- */
    
    int16_t X_Out[32];
    int16_t Y_Out[32];
    int16_t Z_Out[32];
    uint8_t OutArray[8]; 
    uint8_t AccelerationData[32][6];
    
    uint8_t fifo_src_reg;
    uint8 i=0;
	
    #define  HEADER 	0xA0;
    #define  FOOTER 	0xC0;
    
    OutArray[0] = HEADER;
    OutArray[7] = FOOTER;
    
    for(;;)
    {
        if (button_pressed == 1)
        {
            state = Switch_State();
        }
        switch (state)
        {
            case (0):
            PWM_ONBOARD_Stop();
            PWM_ONBOARD_WriteCompare(255);
            PWM_ONBOARD_Start();
            error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_FIFO_SRC_REG,
                                         &fifo_src_reg);    
			if ((error == NO_ERROR) && ((fifo_src_reg) & (0x04)))
			{
				for(i=0;i<32;i++)
				{
					// Read output registers - retrieval accel. data (X,Y,Z axis)
					error=I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
													LIS3DH_OUT_X_L, 6,
													AccelerationData[i]);
					if (error == NO_ERROR)
					{
                
						// Conversion of output data into right-justified 16 bit int (x-axis)
						X_Out[i]=(int16)((AccelerationData[i][0] | (AccelerationData[i][1] << 8))) >> 4;
						
						/*scaling to mg (sensitivity=2mg/digit)*/
						X_Out[i]=abs(X_Out[i]*HR_SENSITIVITY);
						
						
						
						//MSB (x-axis)
						OutArray[1]=(uint8_t)(X_Out[i] >> 8);
						//LSB (x-axis)
						OutArray[2]=(uint8_t)(X_Out[i] & 0xFF);
						
						// Conversion of output data into right-justified 16 bit int (y-axis)
						Y_Out[i]=(int16)((AccelerationData[i][2] | (AccelerationData[i][3] << 8))) >> 4;
						
						/*scaling to mg (sensitivity=2mg/digit)*/
						Y_Out[i]=abs(Y_Out[i]*HR_SENSITIVITY);
						
						 
						
						//MSB (x-axis)
						OutArray[3]=(uint8_t)(Y_Out[i] >> 8);
						//LSB (x-axis)
						OutArray[4]=(uint8_t)(Y_Out[i] & 0xFF);
						
						// Conversion of output data into right-justified 16 bit int (z-axis)
						Z_Out[i]=(int16)(AccelerationData[i][4] | (AccelerationData[i][5] << 8)) >> 4;
						
						/*scaling to mg (sensitivity=2mg/digit)*/
						Z_Out[i]=abs(Z_Out[i]*HR_SENSITIVITY);
						
						set_PWM(X_Out[i],Y_Out[i],Z_Out[i]);
						
						//MSB (x-axis)
						OutArray[5]=(uint8_t)(Z_Out[i] >> 8);
						//LSB (x-axis)
						OutArray[6]=(uint8_t)(Z_Out[i] & 0xFF);
					
						//UART_PutArray(OutArray,8);
						sprintf(message, "Acceleration data: %d %d %d\r\n", X_Out[i],Y_Out[i],Z_Out[i]);
						UART_PutString(message);
						CyDelay(5);
                
					}
				}
			
				// Enable Bypass mode
				fifo_ctrl_reg = LIS3DH_BYPASS_MODE;
				error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
													 LIS3DH_FIFO_CTRL_REG,
													 fifo_ctrl_reg); 
        
				CyDelay(10);
				
				// Enable FIFO mode again for next reading
				fifo_ctrl_reg = LIS3DH_FIFO_MODE;
				error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
													LIS3DH_FIFO_CTRL_REG,
													fifo_ctrl_reg);
                
			}
            break;
            
            case(1):
            PWM_ONBOARD_Stop();
            PWM_ONBOARD_WriteCompare(0);
            PWM_ONBOARD_Start();
            //RGBLed_Stop();
            //PWM_RG_WritePeriod(255);
            PWM_RG_WriteCompare1(255);
            PWM_RG_WriteCompare2(0);
            //PWM_B_WritePeriod(255);
            PWM_B_WriteCompare(255);
           // RGBLed_Start();
            break;
            
            case(2):
            PWM_ONBOARD_Stop();
            PWM_ONBOARD_WriteCompare(127);
            PWM_ONBOARD_Start();
            //RGBLed_Stop();
            //PWM_RG_WritePeriod(255);
            PWM_RG_WriteCompare1(255);
            PWM_RG_WriteCompare2(0);
            //PWM_B_WritePeriod(255);
            PWM_B_WriteCompare(255);
            //RGBLed_Start();
            //CODICE POTENZIOMETRO
            break;
        }
    }
}
/* [] END OF FILE */
