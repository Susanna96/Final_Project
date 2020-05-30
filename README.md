# Final_Project

This repository contains the Final_Project code. In particular, it contains our development and realization of Project_2.

The aim of Project_2 is the developing of a system which controls the blinking frequency of a RGB Led based on the acceleration output data acquired by the LIS3DH 3-axis accelerometer (Adafruit).

# Content of the repository

The main elements of this repository are:
- **Bridge Control Panel folder**: it contains the .iic and .ini files to load in your Bridge Control Panel in case you'd want to plot the acceleration data and verify the feature of the system through which detect over-threshold events on all three axis. 

> **Note**: in order to abilitate the BCP plot, you need to uncomment the following portion of code present in the main.c file (also indicated by the proper commented line)

    // MSB (x-axis)
    OutArray[1]=(uint8_t)(X_Out_mg[i] >> 8);
    // LSB (x-axis)
    OutArray[2]=(uint8_t)(X_Out_mg[i] & 0xFF);
    // MSB (y-axis)
    OutArray[3]=(uint8_t)(Y_Out_mg[i] >> 8);
    // LSB (y-axis)
    OutArray[4]=(uint8_t)(Y_Out_mg[i] & 0xFF);
    // MSB (z-axis)
    OutArray[5]=(uint8_t)(Z_Out_mg[i] >> 8);
    // LSB (z-axis)
    OutArray[6]=(uint8_t)(Z_Out_mg[i] & 0xFF);

    // Read the value of the threshold and save both positive and negative values 
    uint8_t int2_ths;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                           LIS3DH_INT2_THS,
                           &int2_ths);
    int16_t threshold;
    threshold=int2_ths*sensitivity;
    OutArray[7]=(uint8_t)(threshold >> 8);
    OutArray[8]=(uint8_t)(threshold & 0xFF);
    OutArray[9]= -OutArray[7];
    OutArray[10]= -OutArray[8];						
    UART_PutArray(OutArray,12);
    
- **Final_Project.cydsn folder**: it contains the necessary source code and header files to run the system on the CY8CKIT-059 PSoC® 5LP.
At the very beginning of each file you'll find a brief explanation of the purpose of the code, so as to explain each feature of the system as required.








