/* 
 * GRO 302 - Conception d'un robot mobile
 * Code de démarrage Template
 * Auteurs: Etienne Gendron     
 * date: Juin 2022
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

/*---------------------------- variables globales ---------------------------*/

ArduinoX AX_;                       // objet arduinoX
MegaServo servo_;                   // objet servomoteur
VexQuadEncoder vexEncoder_;         // objet encodeur vex
IMU9DOF imu_;                       // objet imu
PID pid_;                           // objet PID

volatile bool shouldSend_ = false;  // drapeau prêt à envoyer un message
volatile bool shouldRead_ = false;  // drapeau prêt à lire un message

int Direction_ = 0;                 // drapeau pour indiquer la direction du robot
volatile bool RunForward_ = false;  // drapeau pret à rouler en avant
volatile bool stop_ = false;        // drapeau pour arrêt du robot
volatile bool RunReverse_ = false;  // drapeau pret à rouler en arrière

SoftTimer timerSendMsg_;            // chronometre d'envoie de messages
SoftTimer timerPulse_;              // chronometre pour la duree d'un pulse

uint16_t pulseTime_ = 0;            // temps dun pulse en ms
float PWM_des_ = 0;                 // PWM desire pour les moteurs


float Axyz[3];                      // tableau pour accelerometre
float Gxyz[3];                      // tableau pour giroscope
float Mxyz[3];                      // tableau pour magnetometre

MotorControl moteur;
LS7366Counter encoder_; 
const float m_pulse = ((2*PI*0.065)/64);
const float un_tour = (2*PI*0.065);

typedef enum state_e {
INITIALISATION,
CALIBRATION,
PRISE_SAPIN,
AVANCE,
ARRET,
GO_TO,
DROP,
RETOUR
} state_t;

 state_t state;

/*------------------------- Prototypes de fonctions -------------------------*/

void timerCallback();
void forward();
void stop();
void reverse();
void sendMsg(); 
void readMsg();
void serialEvent();
void runsequence();
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
  
  // Initialisation du PID
  pid_.setGains(0.25,0.1 ,0);
  // Attache des fonctions de retour
  pid_.setEpsilon(0.001);
  pid_.setPeriod(200);
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
   /* if ()
    {
      state = CALIBRATION;
    } */
    break;
  case CALIBRATION :
    moteur.setSpeed(-0.25);
    delay(1000);
    state = PRISE_SAPIN;
    /*if (<3)
    {
      state = PRISE_SAPIN;
    }*/
    break;
  case PRISE_SAPIN :
    moteur.setSpeed(0);
    encoder_.reset();
    state = AVANCE;
    delay(10);
    /*if (<3)
    {
      state = AVANCE;
    }*/
    break;
  case AVANCE :
    moteur.setSpeed(1);
    if ((encoder_.read()) > 1216)
    {
      state = ARRET;
    }
    break;
    case ARRET :
    Serial.println(encoder_.read());
    moteur.setSpeed(-1);
    encoder_.reset();
    delay(400);  
    state = GO_TO;
   /* if (<3)
    {
      state = GO_TO;
    }*/
    break;
    case GO_TO :
    moteur.setSpeed(1);
    if ((encoder_.read()) > 1216)
    {
      state = DROP;
    }   
    /*if (<3)
    {
      state = DROP;
    }*/
    break;
  case DROP :
    moteur.setSpeed(0);
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
  
  // mise à jour du PID
  pid_.run();
}

/*---------------------------Definition de fonctions ------------------------*/

void serialEvent(){shouldRead_ = true;}

void timerCallback(){shouldSend_ = true;}

void forward(){
  /* Faire rouler le robot vers l'avant à une vitesse désirée */
  AX_.setMotorPWM(0, PWM_des_);
  AX_.setMotorPWM(1, PWM_des_);
  Direction_ = 1;
}

void stop(){
  /* Stopper le robot */
  AX_.setMotorPWM(0,0);
  AX_.setMotorPWM(1,0);
  Direction_ = 0;
}

void reverse(){
  /* Faire rouler le robot vers l'arrière à une vitesse désirée */
  AX_.setMotorPWM(0, -PWM_des_);
  AX_.setMotorPWM(1, -PWM_des_);
  Direction_ = -1;
}
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

void runSequence(){
/*Exemple de fonction pour faire bouger le robot en avant et en arrière.*/

  if(RunForward_){
    forward();
  }

  if(stop_){
    forward();
  }
  if(RunReverse_){
    reverse();
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