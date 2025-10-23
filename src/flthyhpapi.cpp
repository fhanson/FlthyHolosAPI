#include "flthyhpapi.h"
#include <Stream.h>
#include <Wire.h>

#if defined(ARDUINO) && ARDUINO >=100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <avr/pgmspace.h>
#define F(str) (str)
#endif

#ifdef ARDUINO_ARCH_AVR
#include <SoftwareSerial.h>
#endif

#ifdef ARDUINO_ARCH_PIC32
#include <SoftwareSerial.h>
#endif

/*!
  @brief   Start the Flthy API
*/
/* FlthyHP::FlthyHP(uint16_t n, byte connectiontype, int refreshRate)
: begun(false), connection(0x00), commandCache(""), refreshSpeed(0),
  state_override(0),state_musing(0),state_files(0),state_duration(0.00), deviceID(n)
{
    connection = connectiontype;
    refreshSpeed = refreshRate;
}; */

FlthyHP::FlthyHP(const uint8_t addr, TwoWire &i2c)
    : _i2caddr(addr), _i2c(&i2c), _serialBaud(400000)
{
    connectionType=0x03;
}

FlthyHP::FlthyHP(const uint8_t addr, TwoWire &i2c, int baud)
    : _i2caddr(addr), _i2c(&i2c), _serialBaud(baud)
{
    connectionType=0x03;
}

FlthyHP::FlthyHP(HardwareSerial *conn,int baud)
    : _serial(conn), _serialBaud(baud)
{
    connectionType=0x01;
}

FlthyHP::FlthyHP(SoftwareSerial *conn,int baud)
    : _softserial(conn), _serialBaud(baud)
{
    connectionType=0x02;
}

FlthyHP::FlthyHP(int rx, int tx,int baud)
    : _serialBaud(9600)
{
    #if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_PIC32)
    connectionType=0x02;
    _softserial = new SoftwareSerial(rx,tx);
    #endif
}

/*!
  @brief    Begin the Flthy Holoprojector API
*/
void FlthyHP::begin(void)
{
    begin(125); // Default 125ms Refresh
}

/*!
  @brief    Begin the Flthy Holoprojector API
*/
void FlthyHP::begin(const uint16_t refspeed)
{
    refreshSpeed = refspeed;
    switch (connectionType)
    {
        case 0x01:
#ifdef ARDUINO_ARCH_AVR
            _serial->begin(_serialBaud);
#elif defined ARDUINO_ARCH_SAM
            ((USARTClass *)_serial)->begin(_serialBaud);
#elif defined ARDUINO_ARCH_SAMD  // Arduino Zero, Zero Pro, M0 and M0 Pro
            ((HardwareSerial *)_serial)->begin(_serialBaud);
#elif defined ARDUINO_ARCH_PIC32
            _serial->begin(_serialBaud);
#else
            _serial->begin(_serialBaud);
#endif
            while (!_serial);
            _serial->println("Begin _serial");
            break;
        case 0x02:
#ifdef ARDUINO_ARCH_AVR
            ((SoftwareSerial *)_softserial)->begin(_serialBaud);
#elif defined ARDUINO_ARCH_PIC32
            ((SoftwareSerial *)_softserial)->begin(_serialBaud);
#endif
            while (!_softserial);
            _softserial->println("Begin _softserial");
            break;
        case 0x03:
            if (_i2caddr>0){
                _i2c->begin();
                _i2c->setClock(_serialBaud);
                _i2c->setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
            }
            break;
        default:
            Serial.print("Flthy: No Connection Setup Specified");
            break;
    }
}

unsigned long FlthyHPTimer = millis();
void FlthyHP::update(void)
{
    if (millis() > FlthyHPTimer && refreshSpeed > 0) {
        FlthyHPTimer = millis() + refreshSpeed;
        transmit((String)"<QD>",true);
        // Serial.print("state_override: "); Serial.println(state_override);
        // Serial.print("state_musing: "); Serial.println(state_musing);
        // Serial.print("state_files: "); Serial.println(state_files);
        // Serial.print("state_duration: "); Serial.println(state_duration);
        // Serial.print("state_chv: "); Serial.println(state_chv);
        // Serial.print("state_cha: "); Serial.println(state_cha);
        // Serial.print("state_chb: "); Serial.println(state_chb);
    }
    receive();
}

void FlthyHP::sendCommand(String command)
{
    command = flthystartMarker+commandflthy+endMarker;
    transmit(command,false);
}

void FlthyHP::transmit(String command)
{
    transmit(command,false);
}

void FlthyHP::transmit(String command, bool retry)
{
    if (command == "")
    {
        return;
    }

    switch (connectionType)
    {
        case 0x01:
            _serial->write((command + "\n").c_str());
            break;
        case 0x02:
            _softserial->write((command + "\n").c_str());
            break;
        case 0x03:
            int i2cStatus = 0;

            _i2c->beginTransmission((byte)_i2caddr);
            _i2c->write((command + "\n").c_str());
            i2cStatus = _i2c->endTransmission();

            // Serial.print(command); Serial.print("-"); Serial.print(i2cStatus); Serial.println(";");

            if (i2cStatus > 2)
            {
                _i2c->endTransmission();
                if (!retry)
                {
                    delay(100);
                    Serial.print("->");
                    transmit(command,true);
                }
            }
            break;
        default: break;
    }
}

void FlthyHP::receive(void)
{
    switch (connectionType)
    {
        case 0x01:
            if (_serial->available())
            {
                char ch = _serial->read();
                receiveData(ch);
            }
            break;
        case 0x02:
            if (_softserial->available())
            {
                char ch = _softserial->read();
                receiveData(ch);
            }
            break;
        case 0x03:
            int bytes = _i2c->requestFrom((int)_i2caddr,(int)8);
            Serial.print("i2c {");
            Serial.print(bytes);
            if (_i2c->available())
            {
                Serial.print("} receive: ");
                while (_i2c->available())
                {
                    byte ch = _i2c->read();
                    Serial.print((char)ch);
                    receiveData(ch);
                }
                Serial.println(";");
            }
            else
            {
                Serial.println("} - No Response;");
            }
            break;
        default: break;
    }
}


void FlthyHP::processCommands(char* input)
{
    Serial.println(input);
    String response = ToString(input);
    String qC = getValue(response,',',0);

    
}

//----------------------------------------------------
//----------------------------------------------------
//----------------------------------------------------
//----------------------------------------------------
//----------------------------------------------------
//----------------------------------------------------

void FlthyHP::Leia(void)
{
    String msg = "S1";
    sendCommand(msg);
}

void FlthyHP::Stop(void)
{
    // Stop all servos and lights
    String msg = "S4";
    sendCommand(msg);
}

void FlthyHP::StopLights(void) {
    String msg = "S5";
    sendCommand(msg);
}

void FlthyHP::StopServos(void) {
    String msg = "S7";
    sendCommand(msg);
}

void FlthyHP::SetHPColor(String hp, int c, int t) {
    HP = hp.toUpperCase();
    if (HP != "F" && HP != "R" && HP != "T" && HP != "A" ) return;
    if ( t < 0 ) return;
    if ( c < 0 || c > 9 ) return;
    String msg = HP + "O6" + ToString(c) + "|" + ToString(t);
    sendCommand(msg);
}

void FlthyHP::SetHPColor(String hp, int c) {
    HP = hp.toUpperCase();
    if (HP != "F" && HP != "R" && HP != "T" && HP != "A" ) return;
    if ( c < 0 || c > 9 ) return;
    String msg = HP + "O6" + ToString(c);
    sendCommand(msg);
}

void FlthyHP::SetHPColor(int c, int t) {
    if ( t < 0 ) return;
    if ( c < 0 || c > 9 ) return;
    String msg = "AO6" + ToString(c) + "|" + ToString(t);
    sendCommand(msg);
}

void FlthyHP::SetHPColor(int c) {
    HP = hp.toUpperCase();
    if ( c < 0 || c > 9 ) return;
    String msg = "AO6" + ToString(c);
    sendCommand(msg);
}

void FlthyHP::Pulse(String hp, int c, int t) {
    HP = hp.toUpperCase();
    if (HP != "F" && HP != "R" && HP != "T" && HP != "A" ) return;
    if ( t < 0 ) return;
    if ( c < 0 || c > 9 ) return;
    String msg = HP + "O3" + ToString(c) + "|" + ToString(t);
    sendCommand(msg);
}

void FlthyHP::Pulse(String hp, int c) {
    HP = hp.toUpperCase();
    if (HP != "F" && HP != "R" && HP != "T" && HP != "A" ) return;
    if ( c < 0 || c > 9 ) return;
    String msg = HP + "O3" + ToString(c);
    sendCommand(msg);
}

void FlthyHP::Pulse(int c, int t) {
    if ( t < 0 ) return;
    if ( c < 0 || c > 9 ) return;
    String msg = "AO3" + ToString(c) + "|" + ToString(t);
    sendCommand(msg);
}

void FlthyHP::Pulse(int c) {
    HP = hp.toUpperCase();
    if ( c < 0 || c > 9 ) return;
    String msg = "AO3" + ToString(c);
    sendCommand(msg);
}

void FlthyHP::Rainbow(void) {
    String msg = "A007";
    sendCommand(msg);
}

void FlthyHP::Rainbow(int t) {
    if (t < 0 ) return;
    String msg = "A007|" + ToString(t);
    sendCommand(msg);
}

void FlthyHP::Rainbow(String hp) {
    HP = hp.toUpperCase();
    if (HP != "F" && HP != "R" && HP != "T" && HP != "A" ) return;
    String msg = HP + "007";
    sendCommand(msg);
}

void FlthyHP::Rainbow(String hp, int t) {
    String HP = hp.toUpperCase();
    if (HP != "F" && HP != "R" && HP != "T" && HP != "A" ) return;
    if (t < 0 ) return;
    String msg = HP + "007," + ToString(c) + "|" + ToString(t);
    sendCommand(msg);
}