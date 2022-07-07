
#include <moteur.h>

Moteur::Moteur()
{
}

Moteur::~Moteur()
{    
}

void Moteur::init(ArduinoX arx,int motorId)
{
    _arx = arx;
    _motorId = motorId;
}

void Moteur::setSpeed(float speed)
{
    if(speed > 1.0)
        speed = 1.0;
    if(speed < -1.0)
        speed = -1.0;

    _arx.setMotorPWM(_motorId, speed);

}

void Moteur::stopMotor()
{
    _arx.setMotorPWM(_motorId, 0);
}