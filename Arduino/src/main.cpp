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

#define PASPARTOUR      64          // Nombre de pas par tour du moteur
#define RAPPORTVITESSE  50          // Rapport de vitesse du moteur

#define MOTOR_PIN_PWM   5
#define MOTOR_PIN_DIR   30

#define ENCODER_SLAVE_PIN  34
#define ENCODER_FLAG_PIN  A14

#define PASPARTOUR 64
#define RAPPORTVITESSE 19
#define RAYONROUE 0.065

#define POS_CIBLE 0.90
#define POS_DROP 1.20

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


float Axyz[3];                      // tableau pour accelerometre
float Gxyz[3];                      // tableau pour giroscope
float Mxyz[3];                      // tableau pour magnetometre

MotorControl moteur;
LS7366Counter encoder_; 

float potValue = 0;
float posValue = 0;

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

float cmdVitesse = -0.25;

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
  activePrehenseur();
  delay(3000);
}
  
/* Boucle principale (infinie)*/
void loop() {
  //Serial.println(analogRead(POTPIN));
  switch (state)
  {
  case INITIALISATION :
    state = CALIBRATION;
    if (/*limit switch*/)
    {
      cmdVitesse = -0.25;
      state = CALIBRATION;
    } 
    break;
  case CALIBRATION :
    moteur.setSpeed(cmdVitesse);
    delay(1000);
    state = PRISE_SAPIN;
    /*if ( limite switch )
    {
      cmdVitesse = 0;
      state = PRISE_SAPIN;
    }*/
    break;
  case PRISE_SAPIN :
    moteur.setSpeed(/*cmdVitesse*/0);
    encoder_.reset();
    state = GO_TO;
    delay(10);
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
    
   /*if (<3)
    {
      state = DROP;
    }*/
    break;
  case DROP :
    
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

  double deltaP = ((double)(AX_.readResetEncoder(1) * 2 * PI * RAYONROUE) / (double)(PASPARTOUR * RAPPORTVITESSE));
  posValue += deltaP;
  potValue = map(analogRead(POTPIN), 77, 950, -85, 85);


}

/*---------------------------Definition de fonctions ------------------------*/

void serialEvent(){shouldRead_ = true;}

void timerCallback(){shouldSend_ = true;}


void sendMsg(){
  /* Envoit du message Json sur le port seriel */
  StaticJsonDocument<500> doc;
  // Elements du message

  doc["time"] = millis();
  doc["potVex"] = map(analogRead(POTPIN), 77, 950, -85, 85);
  doc["encVex"] = vexEncoder_.getCount();
  doc["goal"] = pid_.getGoal();
  doc["voltage"] = AX_.getVoltage();
  doc["current"] = AX_.getCurrent(); 
  doc["PWM_des"] = PWM_des_;
  doc["Etat_robot"] = Direction_;
  doc["accelX"] = imu_.getAccelX();
  doc["accelY"] = imu_.getAccelY();
  doc["accelZ"] = imu_.getAccelZ();
  doc["gyroX"] = imu_.getGyroX();
  doc["gyroY"] = imu_.getGyroY();
  doc["gyroZ"] = imu_.getGyroZ();
  doc["isGoal"] = pid_.isAtGoal();
  doc["actualTime"] = pid_.getActualDt();

  // Serialisation
  serializeJson(doc, Serial);
  // Envoit
  Serial.println();
  shouldSend_ = false;
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
     PWM_des_ = doc["pulsePWM"].as<float>();
  }

   parse_msg = doc["RunForward"];
  if(!parse_msg.isNull()){
     RunForward_ = doc["RunForward"];
  }

  parse_msg = doc["setGoal"];
  if(!parse_msg.isNull()){
    pid_.disable();
    pid_.setGains(doc["setGoal"][0], doc["setGoal"][1], doc["setGoal"][2]);
    pid_.setEpsilon(doc["setGoal"][3]);
    pid_.setGoal(doc["setGoal"][4]);
    pid_.enable();
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
