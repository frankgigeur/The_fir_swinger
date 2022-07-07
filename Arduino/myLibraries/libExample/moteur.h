#ifndef moteur_H_
#define moteur_H_


#include "LibS3GRO.h"

class Moteur:
{
    private:
    ArduinoX _arx;
    int _motorId;
    public:
    Moteur();    
    ~Moteur();
    void init(ArduinoX arx,int motorId);
    void setSpeed(float speed);
    void stopMotor();
}


#endif // moteur_H_