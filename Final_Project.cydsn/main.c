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

//Brief CONTROL REGISTER 3 address
#define LIS3DH_CTRL_REG3 0x22

/*Brief HEX value for CONTROL REGISTER 3:
I1_OVERRUN = 1 (enable FIFO overrun interrupt on INT1) */
#define LIS3DH_FIFO_OVERRUN 0x02

//Brief CONTROL REGISTER 4 address
#define LIS3DH_CTRL_REG4 0x23

/*Brief HEX value for CONTROL REGISTER 4: +- 4.0 g FSR
CTRL_REG4[3]=1 (High resolution enabled)
CTRL_REG4[5:4]=FS[1:0]=01 (4.0 g FSR) */
#define LIS3DH_CTRL_REG4_2g 0x98

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
#define LIS3DH_FIFO_MODE 0x60

/*Brief HEX value for FIFO CONTROL REGISTER:
FM[1:0]=00 (BYPASS mode)
TR=1 (Triggering signal on INT2)*/
#define LIS3DH_BYPASS_MODE 0x20

//Brief FIFO SRC REGISTER address
#define LIS3DH_FIFO_SRC_REG 0x2F

//Brief INT2 CONFIGURATION register address
#define LIS3DH_INT2_CFG 0x34

/*Brief HEX value for IN2 CONFIGURATION register:
ZHIE=YHIE=ZHIE=1 (Interrupt request on measured accel. value lower than preset
threshold)
*/
#define LIS3DH_INT2_HIGH_EVENT 0x2A

//Brief INT2 THRESHOLD register address
#define LIS3DH_INT2_THS 0x36

/*Brief HEX value for INT2 THRESHOLD register:

*/
#define LIS3DH_THRESHOLD_EVENTS 

//Brief INT2 DURATION register address
#define LIS3DH_INT2_DURATION 0x37 

/*Brief HEX value for INT2 DURATION register:

*/
#define LIS3DH_DURATION_EVENT 

//Brief OUT_X_L register address (x-axis output LSB)
#define LIS3DH_OUT_X_L 0x28

//Brief OUT_Y_L register address (y-axis output LSB)
#define LIS3DH_OUT_Y_L 0x2A

//Brief OUT_Z_L register address (z-axis output LSB)
#define LIS3DH_OUT_Z_L 0x2C

//Brief value of sensitivity in High Resolution mode (2 mg/digit)
#define HR_SENSITIVITY 2;


int main(void)
{
    CyGlobalIntEnable; 
    
    /* INITIALIZATION */
    
    I2C_Peripheral_Start();
    UART_Start();
    
  
    //"The boot procedure is complete about 5 milliseconds after device power-up."
    CyDelay(5); 
   
    /*READING AND WRITING OF LIS3DH REGISTERS*/
    
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
    

    if (ctrl_reg3 != LIS3DH_FIFO_OVERRUN)
    {
        ctrl_reg3 = LIS3DH_FIFO_OVERRUN;
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
            UART_PutString("Error occurred during I2C comm to set control register 1\r\n");   
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
        ctrl_reg4 = LIS3DH_CTRL_REG4_2g;
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

    
    
    int16_t X_Out;
    int16_t Y_Out;
    int16_t Z_Out;
    uint8_t header = 0xA0;
    uint8_t footer = 0xC0;
    uint8_t OutArray[8]; 
    uint8_t AccelerationData[6];
    
    
    OutArray[0] = header;
    OutArray[7] = footer;
    
    for(;;)
    {
        uint8_t fifo_src_reg;
        error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_FIFO_SRC_REG,
                                        &fifo_src_reg);
        if (error == NO_ERROR)
            {
                 // Check for overrun (FIFO buffer is full and ready to be read): OVRN_FIFO=1 
                if ((fifo_src_reg & 0x40) > 0)
                //if (INT_pin_Read())
                {
                    /* Per leggere i dati, diverse possibilità (da implementare)
                     -1 multi read da 196 bytes (poi come me li prendo per separare i valori dei 3 assi? - è la soluzione
                    che consigliano tutte le application notes per ridurre al minimo la comunicazione tra master e slave)
                    - 32 multi read da 6 bytes (eventualmente ciclo for..?)
                    - 196 read singole da 1 byte (non penso proprio)
                    */
                    
                   
                    // Read output registers - retrieval accel. data (X,Y,Z axis)
                    error=I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
												LIS3DH_OUT_X_L, 6,
												AccelerationData);
                    if (error == NO_ERROR)
                    {
                    
                        // Conversion of output data into right-justified 16 bit int (x-axis)
                        X_Out=(int16)((AccelerationData[0] | (AccelerationData[1] << 8))) >> 4;
                        //MSB (x-axis)
                        OutArray[1]=(uint8_t)(X_Out >> 8);
                        //LSB (x-axis)
                        OutArray[2]=(uint8_t)(X_Out & 0xFF);
                        
                        // Conversion of output data into right-justified 16 bit int (y-axis)
                        Y_Out=(int16)((AccelerationData[2] | (AccelerationData[3] << 8))) >> 4;
                        //MSB (x-axis)
                        OutArray[3]=(uint8_t)(Y_Out >> 8);
                        //LSB (x-axis)
                        OutArray[4]=(uint8_t)(Y_Out & 0xFF);
                        
                        // Conversion of output data into right-justified 16 bit int (z-axis)
                        Z_Out=(int16)(AccelerationData[4] | (AccelerationData[5] << 8)) >> 4;
                        //MSB (x-axis)
                        OutArray[5]=(uint8_t)(Z_Out >> 8);
                        //LSB (x-axis)
                        OutArray[6]=(uint8_t)(Z_Out & 0xFF);
                    
                        //UART_PutArray(OutArray,8);
                         sprintf(message, "Acceleration data: %d %d %d\r\n", X_Out,Y_Out,Z_Out);
                         UART_PutString(message);
                    
                    }
                    
                    
                    
                    // Enable Bypass mode
                    fifo_ctrl_reg = LIS3DH_BYPASS_MODE;
                    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                                        LIS3DH_FIFO_CTRL_REG,
                                                         fifo_ctrl_reg); 
                  
                }
                
        // Enable FIFO mode again for next reading
        fifo_ctrl_reg = LIS3DH_FIFO_MODE;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_FIFO_CTRL_REG,
                                             fifo_ctrl_reg); 
        }
    }
}
/* [] END OF FILE */
