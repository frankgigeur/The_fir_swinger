/* 
 * GRO 302 - Conception d'un robot mobile
 * Code de démarrage Template
 * Auteurs: Nick à place de Charles 
 * date: Juin 2022 c
*/

/*------------------------------ Librairies ---------------------------------*/
#include <ArduinoJson.h> // librairie de syntaxe JSON
#include <SPI.h> // librairie Communication SPI
#include <LibS3GRO.h>

/*------------------------------ Constantes ---------------------------------*/

#define BAUD            115200      // Frequence de transmission serielle
#define UPDATE_PERIODE  100         // Periode (ms) d'envoie d'etat general

#define MAGPIN          32          // Port numerique pour electroaimant
#define POTPIN          A5          // Port analogique pour le potentiometre

#define MOTOR_PIN_PWM   5
#define MOTOR_PIN_DIR   30

#define ENCODER_SLAVE_PIN  34
#define ENCODER_FLAG_PIN  A14

#define PASPARTOUR 64
#define RAPPORTVITESSE 19
#define RAYONROUE 0.065

#define POS_CIBLE 0.4
#define POS_DROP 0.6

#define PIN_LIMITSWITCH 10
#define PIN_SAPIN 9
#define RANGE_VITESSE_ANG_MAX  0.008
#define RANGE_VITESSE_ANG_MIN -0.008


/*---------------------------- variables globales ---------------------------*/

ArduinoX AX_;                       // objet arduinoX
MegaServo servo_;                   // objet servomoteur
VexQuadEncoder vexEncoder_;         // objet encodeur vex
IMU9DOF imu_;                       // objet imu

volatile bool shouldSend_ = false;  // drapeau prêt à envoyer un message
volatile bool shouldRead_ = false;  // drapeau prêt à lire un message

SoftTimer timerSendMsg_;            // chronometre d'envoie de messages
SoftTimer timerPulse_;              // chronometre pour la duree d'un pulse

uint16_t pulseTime_ = 0;            // temps dun pulse en ms
float PWM_des_ = 0;                 // PWM desire pour les moteurs
float comsommation = AX_.getVoltage() * AX_.getCurrent() * millis(); //consommation = Power* temps


float Axyz[3];                      // tableau pour accelerometre
float Gxyz[3];                      // tableau pour giroscope
float Mxyz[3];                      // tableau pour magnetometre

MotorControl moteur;
LS7366Counter encoder_; 

float potValue = 0;
float lastPotValue = 0;
float posValue = 0;
float vitesseAng = 0;

typedef enum state_e {
INITIALISATION,
CALIBRATION,
PRISE_SAPIN,
GO_TO,
DECELERATION,
STABILISATION,
DROP,
RETOUR
} state_t;

 state_t state;

float cmdVitesse = -0.10;
unsigned long lastTimeMili = 0;


/*------------------------- Prototypes de fonctions -------------------------*/

void timerCallback();
void sendMsg(); 
void readMsg();
void serialEvent();
void activePrehenseur();
void deactivePrehenseur();

/*---------------------------- fonctions "Main" -----------------------------*/

void setup() {
  Serial.begin(BAUD);               // initialisation de la communication serielle
  AX_.init();                       // initialisation de la carte ArduinoX 
  imu_.init();                      // initialisation de la centrale inertielle
  vexEncoder_.init(2,3);            // initialisation de l'encodeur VEX
  // attache de l'interruption pour encodeur vex
  attachInterrupt(vexEncoder_.getPinInt(), []{vexEncoder_.isr();}, FALLING);
  
  // Chronometre envoie message
  timerSendMsg_.setDelay(UPDATE_PERIODE);
  timerSendMsg_.setCallback(timerCallback);
  timerSendMsg_.enable();
  
  state = INITIALISATION;
  moteur.init(MOTOR_PIN_PWM,MOTOR_PIN_DIR);
  encoder_.init(ENCODER_SLAVE_PIN, ENCODER_FLAG_PIN);



  delay(3000);
}
  
/* Boucle principale (infinie)*/
void loop() {
  //Serial.println(analogRead(POTPIN));
  switch (state)
  {
  case INITIALISATION :
    state = CALIBRATION;
   /* if ()
    {
      cmdVitesse = -0.25;
      state = CALIBRATION;
    } */
    break;
  case CALIBRATION :
    moteur.setSpeed(cmdVitesse);
    if ( digitalRead(PIN_LIMITSWITCH) == HIGH )
    {
      activePrehenseur();
      cmdVitesse = 0;
      state = PRISE_SAPIN;
    }
    break;
  case PRISE_SAPIN :
    moteur.setSpeed(cmdVitesse);
    encoder_.reset();
    posValue = 0;
    if ( digitalRead(PIN_SAPIN) )
    state = GO_TO;
    
    break;
    case GO_TO :
    moteur.setSpeed(1);
    if ( (posValue) >= POS_CIBLE )
    {
      state = DECELERATION;
    }   
    break;
    case DECELERATION :
    cmdVitesse -= 0.01;
    moteur.setSpeed(cmdVitesse);
    if ( (posValue) >= POS_DROP )
    {
      state = STABILISATION;
    }   
    break;
    case STABILISATION :
    vitesseAng = (potValue-lastPotValue)/((millis()-lastTimeMili)/1000);

    lastPotValue = potValue;
    lastTimeMili = millis(); 

    cmdVitesse = potValue/85;
    moteur.setSpeed(cmdVitesse);
    if (vitesseAng <= RANGE_VITESSE_ANG_MAX && vitesseAng >= RANGE_VITESSE_ANG_MIN) // doit avoir une position aussi!!
    {
      state = DROP;
      cmdVitesse = 0;
      moteur.setSpeed(cmdVitesse);
    }
    break;
  case DROP :
    deactivePrehenseur();
   /*if (<3)
    {
      state = RETOUR;
    }*/
    break;
  case RETOUR :

    /*if (<3)
    {
      state = INITIAISATION;
    }*/
    break;

   /* default:
    state = INITIALISATION;
    break;*/
  }
  if(shouldRead_){
    readMsg();
  }
  if(shouldSend_){
    sendMsg();
  }
  

  // mise a jour des chronometres
  timerSendMsg_.update();
  timerPulse_.update();

  double deltaP = -1*((double)(AX_.readResetEncoder(0) * 2 * PI * RAYONROUE) / (double)(PASPARTOUR * RAPPORTVITESSE));
  posValue += deltaP;

  potValue = map(analogRead(POTPIN), 170, 850, -85, 85);

  Serial.print("CMD: ");
  Serial.println(cmdVitesse);


}

/*---------------------------Definition de fonctions ------------------------*/

void serialEvent(){shouldRead_ = true;}

void timerCallback(){shouldSend_ = true;}


void sendMsg(){
  /* Envoit du message Json sur le port seriel */
  StaticJsonDocument<500> doc;
  // Elements du message

  doc["time"] = millis();
  doc["potVex"] = potValue;
  doc["voltage"] = AX_.getVoltage();
  doc["current"] = AX_.getCurrent(); 

  //doc["vitesse"] = deltaP;
  doc["position"] = posValue;
  doc["cmdVitesse"] =cmdVitesse;
  //doc["Consommation"] = consommation;



  // Serialisation
  serializeJson(doc, Serial);
  // Envoit
  Serial.println("test");
  shouldSend_ = true;
}

void readMsg(){
  // Lecture du message Json
  StaticJsonDocument<500> doc;
  JsonVariant parse_msg;

  // Lecture sur le port Seriel
  DeserializationError error = deserializeJson(doc, Serial);
  shouldRead_ = false;

  // Si erreur dans le message
  if (error) {
    Serial.print("deserialize() failed: ");
    Serial.println(error.c_str());
    return;
  }
  
  // Analyse des éléments du message message
  parse_msg = doc["PWM_des"];
  if(!parse_msg.isNull()){
     PWM_des_ = doc["pulse_des"].as<float>();
  }

  parse_msg = doc["voltage"];
  if(!parse_msg.isNull()){
     AX_.getVoltage();
  }

  parse_msg = doc["current"];
  if(!parse_msg.isNull()){
     AX_.getCurrent();
  }

  parse_msg = doc["position"];
  if(!parse_msg.isNull()){
     posValue = doc["position"].as<float>();
  }

  parse_msg = doc["cmdVitesse"];
  if(!parse_msg.isNull()){
     cmdVitesse = doc["cmdVitesse"].as<float>();
  }

}

void activePrehenseur()
{
  digitalWrite(MAGPIN, HIGH);
}

void deactivePrehenseur()
{
  digitalWrite(MAGPIN, LOW);
}
