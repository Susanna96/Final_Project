# Final_Project

This repository contains the Final_Project code. In particular, it contains our development and realization of Project_2 (variant 1).

The aim of Project_2 is the developing of a system which controls the blinking frequency of a RGB LED based on the acceleration output data acquired by the LIS3DH 3-axis accelerometer (Adafruit).

In order to achieve this goal, we first configured the LIS3DH using high-speed I2C (400 kHz), low-power mode and 50 Hz data rate. We set the UART baud rate at 57600 bps. 
We decided to use the potentiometer to change the full-scale range. When the pin 3.2 is connected to Vdd, the variable "Flag" is set to 1 and the device is in configuration mode (on-board blue LED is blinking). In this state, moving the potentiometer it is possible to change the full-scale range. 
This happens through ranges of values: 
- **[0V- 1.25V)**: the FSR is [-2g, +2g]; 
- **[1.25V – 2.5V)**: the FSR is [-4g, +4g];
- **[2.5V – 3.75V)**: the FSR is [-8g, +8g];
- **[3.75V – 5V]**: the FSR is [-16g, +16g]

The value of the FSR and the current state of the device are stored in a uint8 variable called "Parameters", where the bits [7:4] encode for the FSR (0000 (2g), 0001 (4g), 0010 (8g), 0011 (16g)) and the bits [3:0] encode for the state (0000 (Start), 0001 (Stop), 0010 (Configuration mode)). 
In the function setParameter() we first set the value of the FSR according to the potentiometer signal; then in the main function we shift 4 bits to the left and do an OR operation with the value stored in the variable "state". When the state changes, we perform a 4-bit right shift in order to remove the old state, then a 4-bit left shift to keep the value of the FSR and an OR operation with the current state. In this function we also set the value of other variables such as the full-scale range, the sensitivity and the content of CTRL_REG4 and INT2_THS registers in order to set the FSR and the threshold.
When "Flag" is set to 0 (pin 3.2 connected to GND) a warning message sent over UART informs that the device is in read-only modality, and moving the potentiometer does not change the current parameters. When the device is back to configuration mode, we check if the potentiometer changed its position during read-only modality and if so, a warning message informs the user that the potentiometer has been moved and consequently the parameters change according to the new position of the potentiometer. The value of the parameters are always sent over UART in each state in order to inform the user.
For the over-threshold events, we chose the threshold to be about 1500 mg for all the four full-scale ranges, and the duration equal to 0.3 seconds. After the recognition of the over-threshold event, 5 samples and the timestamp are stored in the EEPROM (@0x0002). 
The function set_PWM() is used to turn-on the RGB LED channels and change the blinking frequency according to the value of measured acceleration. If the device is still and the z-axis is in the direction of gravitational acceleration, a steady blue light is on; if the y-axis measures gravitational acceleration the light is green; if the x-axis measures gravitational acceleration the light is red. If the device is moving, the LED starts to blink. 

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








