#pragma once

#ifndef FLTHYHPAPI_F
#define FLTHYHPAPI_F

#ifdef ARDUINO
#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#include <pins_arduino.h>
#endif
#endif

#include <String.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define FLTHY_BUFFER_SIZE 32

#ifndef FLTHY_BAUD_RATE
#define FLTHY_BAUD_RATE 9600
#endif

#ifndef FLTHY_I2C_RATE
#define FLTHY_I2C_RATE 400000
#endif

#define CONN_SERIAL0 0x00 //"Serial"
#define CONN_SERIAL1 0x01 //"Serial1"
#define CONN_SERIAL2 0x02 //"Serial2"
#define CONN_SERIAL3 0x03 //"Serial3"
#define CONN_SERIAL4 0x04 //"Serial4"
#define CONN_I2C     0x05 //"I2C"
#define CONN_I2C1    0x06 //"I2C1"
#define CONN_I2C2    0x07 //"I2C2"

/*!
    @brief  Class that stores state and functions for FlthyHPs
*/
class FlthyHP
{
public:
    /**
     * @brief Configure the FlthyHP for I2C
     * 
     * @param addr the I2C Address in hexidecimal
     * @param i2c the I2C interface (aka "Wire")
     */
    FlthyHP(const uint8_t addr, TwoWire &i2c);

    /**
     * @brief Configure the FlthyHP for I2C with a baud rate
     * 
     * @param addr the I2C Address in hexidecimal
     * @param i2c the I2C interface (aka "Wire")
     * @param baud the baud rate
     */
    FlthyHP(const uint8_t addr, TwoWire &i2c, int baud);
    
    /**
     * @brief Configure the FlthyHP for Hardware Serial with a baud rate
     * 
     * @param conn the serial connection
     * @param baud the baud rate
     */
    FlthyHP(HardwareSerial *conn,int baud);

    /**
     * @brief Configure the FlthyHP for Software Serial with a baud rate
     * 
     * @param conn the serial connection
     * @param baud the baud rate
     */
    FlthyHP(SoftwareSerial *conn,int baud);

    /**
     * @brief Configure the FlthyHP for Serial with specific rx and tx pins
     * 
     * @param rx the rx pin number
     * @param tx the tx pin number
     * @param baud the baud rate
     */
    FlthyHP(int rx, int tx, int baud);

    /**
    * @brief Initializes the holoprojectors
    * 
    */
    void begin(void);

    /**
     * @brief Initializes the holoprojectors with a refresh speed
     * 
     * @param refspeed
     */
    void begin(const uint16_t refspeed);

    /**
    * @brief Triggers the Leia sequence
    * 
    */
    void Leia(void);

    /**
    * @brief Stops all movement and lights
    * 
    */ 
    void Stop(void);

    /**
    * @brief Stops whatever lights are playing
    * 
    */
    void StopLights(void);

    /**
    * @brief Stops whatever lights are playing
    * 
    */
    void StopServos(void);

    /**
     * @brief Set the Holoprojector color
     * hp - Holoprojector Selection:
     * F - Front
     * R - Rear
     * T - Top
     * A - All
     * @param hp the holoprojector (F,R,T,A)
     * 
     * C - Color Value from the list below:
     * 1 - Red
     * 2 - Yellow
     * 3 - Green
     * 4 - Cyan (Aqua)
     * 5 - Blue
     * 6 - Magenta
     * 7 - Orange
     * 8 - Purple
     * 9 - White
     * 0 - Random
     * @param c the color (0-9)
     * @param t the duration in seconds (integer)
     */
    void SetHPColor(String hp, int c,int t);
    void SetHPColor(String hp, int c);
    void SetHPColor(int c,int t);
    void SetHPColor(int c);

    /**
     * @brief Flash the projectors with a color
     * hp - Holoprojector Selection:
     * F - Front
     * R - Rear
     * T - Top
     * A - All
     * @param hp the holoprojector (F,R,T,A)
     * 
     * C - Color Value from the list below:
     * 1 - Red
     * 2 - Yellow
     * 3 - Green
     * 4 - Cyan (Aqua)
     * 5 - Blue
     * 6 - Magenta
     * 7 - Orange
     * 8 - Purple
     * 9 - White
     * 0 - Random
     * @param c the color (0-9)
     * @param t the duration in seconds (integer)
     */
    void Projector(String hp, int c,int t);
    void Projector(String hp, int c);
    void Projector(int c,int t);
    void Projector(int c);

    /**
     * @brief Set the Holoprojector to Pulse a color
     * hp - Holoprojector Selection:
     * F - Front
     * R - Rear
     * T - Top
     * A - All
     * @param hp the holoprojector (F,R,T,A)
     * 
     * C - Color Value from the list below:
     * 1 - Red
     * 2 - Yellow
     * 3 - Green
     * 4 - Cyan (Aqua)
     * 5 - Blue
     * 6 - Magenta
     * 7 - Orange
     * 8 - Purple
     * 9 - White
     * 0 - Random
     * @param c the color (0-9)
     * @param t the duration in seconds (integer)
     */
    void Pulse(String hp, int c,int t);
    void Pulse(String hp, int c);
    void Pulse(int c,int t);
    void Pulse(int c);

    /**
     * @brief Set lights to the Rainbow Sequence
     * 
     * hp - Holoprojector Selection:
     * F - Front
     * R - Rear
     * T - Top
     * A - All
     * @param hp the holoprojector (F,R,T,A)
     * @param t the duration in seconds (integer)
     */
    void Rainbow(String hp,int t);
    void Rainbow(String hp);
    void Rainbow(int t);
    void Rainbow(void);

    void ShortCircuit(void);





private:
    uint8_t _i2caddr=0;
    TwoWire *_i2c;
    HardwareSerial *_serial;
    SoftwareSerial *_softserial;
    int _serialBaud;
    int connectionType;

    void transmit(String command);
    void transmit(String command, bool retry);
    void receive(void);
    void sendCommand(String command);
    void receiveData(char ch);
    void receiveData(String data);
    void processCommands(char* input);
    String getValue(String data, char separator, int index);

protected:
    char cmdBuffer[FLTHY_BUFFER_SIZE];
    unsigned cmdPos;
    byte connection;
    int refreshSpeed;

};

#endif