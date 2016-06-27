/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#include "Arduino.h"


#ifndef PLCf_h
#define PLCf_h

void Debug();
void InitPlc();
void debugPlc(String text);
void systemPlc();
void timeCycle();
void SerialCommand();
void DebugCommand(String command);
void Start();
void Stop();



static char incomingByte ; 
static String serialmessage;

class IO
{
  public:
    IO(int type, int pin, String IOname, String longName, int ton = 0);
    void updateState();
    int fm();
    int fd();
    int Q();
    void I();
    void O();
    void S();
    void R();
    void ton(int delayTon);
    void tof(int delayTof);
    String getName();
  private:
    void printDebug();
    int _pin;
    int _type;
    String _IOname;
    String _longName;
    int _ton;
    int _tof;
    int _delayTon;
    int _delayTof;
    int _previousState = LOW;
    int _currentState = LOW;
    int _nextStateO = LOW;
    int _nextStateSet = LOW;
    int _forceState = LOW;
    int _forceValue = LOW;
    int _S = LOW;
    int _fm;
    int _fd;
    int _countFilter = 0;
    int _filterState = LOW;
    int _countBeforeOnOff;
    int _SECURITY = LOW;
};

class MEMORY
{
  public:
    MEMORY(String IOname, String longName, int &remanence);
    void S();
    void R();
    int getVal();
    String getName();
  private:
    void printDebug();
    String _IOname;
    String _longName;
};

class TEMPO
{
  public:
    TEMPO(int tempoDelay);
    void INIT();
    int FIN();
    void PAUSE();
    void STOP();
    long unsigned int _currentCount;
  private:
    int _pause = LOW;
    long unsigned int _delay;
    long unsigned int _previousMillisec;
    
};

class PULSE
{
  public:
    PULSE(int pulseDelay);
    int Q();
  private:
    int _pulseCount = 0;
    int _pulseDelay;
};

class GRAFCET
{
  public:
    GRAFCET(String Name, int maximumStep, int startP, int endP);
    void NEXT(String comment = "");
    void GOTO(int stepPoint, String comment = "");
    void STOP(String comment = "");
    void PAUSE(String comment = "");
    void RUN(String comment = "");
    int ISRUN();
    int Q(int s);
  private:
    int _step = 0;
    int _maximumstep = 0;
    int _statut = 0;
    int _breakPoint = 10000;
    int _entryPoint = 0;
    int _debug = LOW;
    String _name;
};

#endif
