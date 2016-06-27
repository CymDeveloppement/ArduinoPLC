/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "plcFunc.h"


unsigned int cycleWatchDog = 0;
unsigned int cycleMicroSecWatchDog = 0;
long unsigned int timeoutSerialCounter = 0;
long unsigned int previousMillisecCycle = 0;
long unsigned int previousMicroSecCycle = 0;
int STOP = LOW;
int MARCHE = LOW;
int DEBUG = LOW;

void Debug()
{
  DEBUG = HIGH;
}
void Start()
{
  STOP = LOW;
  MARCHE = HIGH;
}

void Stop()
{
  STOP = HIGH;
  MARCHE = LOW;
}

void InitPlc()
{
  Serial.begin(9600);
  Serial.println("START...");
  Serial.println("SERIAL CONSOLE v0.1a");
}

void debugPlc(String text)
{
  if(DEBUG) Serial.println(text);
}

void systemPlc()
{
  delay(1);
  timeCycle();
  SerialCommand();
  
}

void timeCycle()
{
  long unsigned int currentMillisecCycle = millis();
  long unsigned int currentMicrosecCycle = micros();
  if(previousMillisecCycle < (currentMillisecCycle + 1))
  {
    cycleWatchDog = currentMillisecCycle - previousMillisecCycle;
  }

  if(previousMicroSecCycle < (currentMicrosecCycle + 1))
  {
    cycleMicroSecWatchDog = currentMicrosecCycle - previousMicroSecCycle;
  }
  previousMicroSecCycle = currentMicrosecCycle;
  previousMillisecCycle = currentMillisecCycle;
  
}

void SerialCommand()
{
  
  long unsigned int timeoutserial = 500;
  
  if(timeoutSerialCounter != 0)
  {
    
    timeoutSerialCounter = timeoutSerialCounter + cycleWatchDog;
    //Serial.println(cycle);
    
  }
  
  if (Serial.available() > 0) 
  {
    
    if(timeoutSerialCounter == 0)
    {
      timeoutSerialCounter = timeoutSerialCounter + cycleWatchDog;
      
    }
    
    incomingByte = Serial.read();
    serialmessage = serialmessage + incomingByte;
    
   } else if(timeoutSerialCounter > timeoutserial) {
    
    timeoutSerialCounter = 0;
    if(serialmessage != "")
    {
      Serial.println("");
      Serial.print("COMMAND SYSTEM : ");
      Serial.println(serialmessage);
      DebugCommand(serialmessage);
      serialmessage = "";
    }
   }
   
}

void DebugCommand(String command)
{
  String commandExtract = command;
  if(commandExtract == "CYCLE")
  {
    Serial.println("************************************");
    Serial.print("Dernier cycle connu : ");
    Serial.print(cycleMicroSecWatchDog);
    Serial.print(" Microseconds / ");
    Serial.print(cycleWatchDog);
    Serial.println(" Milliseconds / ");
    Serial.println("************************************");
    
  } else if(commandExtract == "STOP")
  {
    Serial.println("************************************");
    Serial.println("ARRET GENERAL AUTOMATE : ");
    Serial.println("************************************");
    Stop();
  } else if(commandExtract == "MARCHE")
  {
    Serial.println("************************************");
    Serial.println("MARCHE GENERAL AUTOMATE : ");
    Serial.println("************************************");
    Start();
  } else {
    Serial.println("Cette commande est Inconnu");
  }
}


long unsigned int H(int hours)
{
  return (1000 * (3600 * hours));
}

long unsigned int M(int minutes)
{
  return (60000 * minutes);
}

long unsigned int S(int seconds)
{
  return (1000 * seconds);
}

IO::IO(int type, int pin, String IOname, String longName, int ton)
{
  pinMode(pin, type);
  _pin = pin;
  _IOname = IOname;
  _type = type;
  _longName = longName;
  _ton = ton;
}

void IO::updateState()
{
  if(!STOP)
  {
    int readState = LOW;

    _fm = LOW;
    _fd = LOW;

    switch (_type) {
      case INPUT:
        if(_countFilter >= _ton)
        {
            _previousState = _currentState;
            _currentState = digitalRead(_pin);
            
            printDebug();
            if(_previousState < _currentState)
            {
              _fm = HIGH;
              
            } else if(_previousState > _currentState)
            {
              _fd = HIGH;
              
            }
            _countFilter = 0;
         } else if(_previousState != digitalRead(_pin)) {
            
            _countFilter = _countFilter + cycleWatchDog;
        
         } else if(_previousState == digitalRead(_pin)) {
            _countFilter = 0;
         }
    
        break;
      case OUTPUT:
      
        if((_nextStateO || _nextStateSet))
        {
          _currentState = HIGH;
          
        } else {
          if(!_delayTof || _tof >= _delayTof) _currentState = LOW;
          if(_delayTof && _tof < _delayTof)
          {
            _currentState = HIGH;
            _tof = _tof + cycleWatchDog;
          }
          
        }
        
        _nextStateO = LOW;

        if(_currentState >  _previousState){
          _tof = 0;
          _delayTof = 0;
          _fm = HIGH;
        } else if (_currentState <  _previousState)
        {
          _countBeforeOnOff = 0;
          _fd = HIGH;
        }
        if(!STOP && !_SECURITY) digitalWrite(_pin, _currentState);
        
        
        
        printDebug();
        _previousState = _currentState;
        
        if(!_delayTon) _countBeforeOnOff = 0;
        _delayTon = 0;

        
        
        break;
      default:
        // if nothing else matches, do the default
        // default is optional
      break;
    }
  }
}

int IO::fm()
{
  return _fm;
}

int IO::fd()
{
  return _fd;
}

int IO::Q()
{
  return _currentState;
}

void IO::I()
{
  _nextStateO = HIGH;
}

void IO::O()
{
  _nextStateO = LOW;
}

void IO::S()
{
  _nextStateO = HIGH;
  _nextStateSet = HIGH;
}

void IO::R()
{
  _nextStateO = LOW;
  _nextStateSet = LOW;
}

void IO::ton(int delayTon)
{
  
  _delayTon = delayTon;
  
  if(_countBeforeOnOff < delayTon) _countBeforeOnOff = _countBeforeOnOff + cycleWatchDog;

  if(_countBeforeOnOff >= delayTon)  _nextStateO = HIGH;
  
}

void IO::tof(int delayTof)
{
  _delayTof = delayTof;
  _nextStateO = HIGH;
}

String IO::getName()
{
  return _IOname;
}


void IO::printDebug()
{
  String textToPrint;
  if(DEBUG && (_previousState != _currentState))
  {
    textToPrint = _IOname + " a change d'etat : " + _previousState + " --> " + _currentState;
    debugPlc(textToPrint);
  }
  
}

TEMPO::TEMPO(int tempoDelay)
{
   _delay = tempoDelay;
   _currentCount = 0;
}

void TEMPO::INIT()
{
   _currentCount = 0;
}

int TEMPO::FIN()
{
  int returnval = LOW;
  if(!_pause && (_currentCount < _delay))
  {
    _currentCount = _currentCount + cycleWatchDog;
    if(DEBUG) debugPlc(String(_currentCount));
  }
  
  _pause = LOW;
  
  if(_currentCount >= _delay) returnval = HIGH;
  
  return returnval;
}
void TEMPO::PAUSE()
{
  _pause = HIGH;
}
void TEMPO::STOP()
{
   _currentCount = _delay + 10;
}


GRAFCET::GRAFCET(String Name, int maximumStep = 0, int startP = 0, int endP = 0)
{
  _maximumstep = maximumStep;
  _step = 0;
  _entryPoint = startP;
  _breakPoint = endP;
  _name = Name;
  if(endP == 0) _breakPoint = maximumStep;
}

void GRAFCET::NEXT(String comment)
{
  String DebugText = _name + " ==> " + comment + " // passage en etape : ";
  _step = _step + 1;
  if(_step > _breakPoint) _step = _entryPoint;
  if(DEBUG) debugPlc(DebugText +  _step );
  
}

int GRAFCET::Q(int s)
{
  int returnval = LOW;
  if(_step == s) returnval = HIGH;
  return returnval;
}

void GRAFCET::GOTO(int stepPoint, String comment)
{
  String DebugText = _name + " ==> " + comment + " // passage en etape : ";
  _step = stepPoint;
  if(_step > _breakPoint) _step = _entryPoint;
  if(DEBUG) debugPlc(DebugText +  _step);
}

void GRAFCET::STOP(String comment)
{
  _step = _breakPoint + 1;
  String DebugText = _name + " ==> " + comment + " mis a l'arret utiliser RUN(); pour relancer ==> etape  ";
}

void GRAFCET::RUN(String comment)
{
  _step = 0;
}

int GRAFCET::ISRUN()
{
  int returnval = LOW;
  if(_step <= _breakPoint) returnval = HIGH;
  return returnval;
}

PULSE::PULSE(int pulseDelay)
{
 _pulseDelay = pulseDelay;
}

int PULSE::Q()
{
  
 int returnval = LOW;
 _pulseCount = _pulseCount + cycleWatchDog;
   
 if(_pulseCount >= (2*_pulseDelay))
 {
    _pulseCount = 0;
 }
 if(_pulseCount < _pulseDelay)
 {
    returnval = HIGH;
 } else if(_pulseCount >= _pulseDelay)
 {
    returnval = LOW;
 }

 return returnval;
}


