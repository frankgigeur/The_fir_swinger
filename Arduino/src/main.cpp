/*
 * GRO 302 - Conception d'un robot mobile
 * Code de démarrage Template
 * Auteurs: Nick à place de Charles
 * date: Juin 2022 c
 */

/*------------------------------ Librairies ---------------------------------*/
#include <ArduinoJson.h> // librairie de syntaxe JSON
#include <SPI.h>         // librairie Communication SPI
#include <LibS3GRO.h>

/*------------------------------ Constantes ---------------------------------*/

#define BAUD 115200        // Frequence de transmission serielle
#define UPDATE_PERIODE 100 // Periode (ms) d'envoie d'etat general

#define MAGPIN 32 // Port numerique pour electroaimant
#define POTPIN A5 // Port analogique pour le potentiometre

#define MOTOR_PIN_PWM 5
#define MOTOR_PIN_DIR 30

#define ENCODER_SLAVE_PIN 34
#define ENCODER_FLAG_PIN A14

#define PASPARTOUR 64
#define RAPPORTVITESSE 19
#define RAYONROUE (0.1338 / 2)

#define POS_CIBLE 0.3
#define POS_DROP 0.5

#define PIN_LIMITSWITCH 10
#define PIN_SAPIN 9
#define RANGE_VITESSE_ANG_MAX 0.05
#define RANGE_VITESSE_ANG_MIN -0.05

/*---------------------------- variables globales ---------------------------*/

ArduinoX AX_;               // objet arduinoX
MegaServo servo_;           // objet servomoteur
VexQuadEncoder vexEncoder_; // objet encodeur vex
IMU9DOF imu_;               // objet imu

volatile bool shouldSend_ = false; // drapeau prêt à envoyer un message
volatile bool shouldRead_ = false; // drapeau prêt à lire un message

SoftTimer timerSendMsg_; // chronometre d'envoie de messages
SoftTimer timerPulse_;   // chronometre pour la duree d'un pulse

uint16_t pulseTime_ = 0;                                             // temps dun pulse en ms
float PWM_des_ = 0;                                                  // PWM desire pour les moteurs
float comsommation = AX_.getVoltage() * AX_.getCurrent() * millis(); // consommation = Power* temps

float Axyz[3]; // tableau pour accelerometre
float Gxyz[3]; // tableau pour giroscope
float Mxyz[3]; // tableau pour magnetometre

MotorControl moteur;

float potValue = 0;
float lastPotValue = 0;
float posValue = 0;
float vitesseAng = 0;

typedef enum state_e
{
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
unsigned long lastTimeMili = 0;
bool calibrationOn = false;
bool run = false;
char strState[50];
int enccooooooooo = 0;

/*------------------------- Prototypes de fonctions -------------------------*/

void timerCallback();
void sendMsg();
void readMsg();
void serialEvent();
void activePrehenseur();
void deactivePrehenseur();

/*---------------------------- fonctions "Main" -----------------------------*/

void setup()
{
  Serial.begin(BAUD);     // initialisation de la communication serielle
  AX_.init();             // initialisation de la carte ArduinoX
  imu_.init();            // initialisation de la centrale inertielle
  vexEncoder_.init(2, 3); // initialisation de l'encodeur VEX
  // attache de l'interruption pour encodeur vex
  attachInterrupt(
      vexEncoder_.getPinInt(), []
      { vexEncoder_.isr(); },
      FALLING);

  // Chronometre envoie message
  timerSendMsg_.setDelay(UPDATE_PERIODE);
  timerSendMsg_.setCallback(timerCallback);
  timerSendMsg_.enable();

  state = INITIALISATION;
  moteur.init(MOTOR_PIN_PWM, MOTOR_PIN_DIR);

  pinMode(PIN_LIMITSWITCH, INPUT);

  delay(3000);
}

/* Boucle principale (infinie)*/
void loop()
{
  // Serial.println(analogRead(POTPIN));
  switch (state)
  {
  case INITIALISATION:
    sprintf(strState, "INITIALISATION");
    moteur.setSpeed(0);
    deactivePrehenseur();
    cmdVitesse = 0;
    delay(100); // Important!! S'assure que la roue est arrêtée avant de reset
    posValue = 0;
    AX_.resetEncoder(0);
    run = false;
    calibrationOn = false;
    state = CALIBRATION;
    /* if ()
     {
       cmdVitesse = -0.25;
       state = CALIBRATION;
     } */
    break;
  case CALIBRATION:
    sprintf(strState, "CALIBRATION");
    if (calibrationOn)
    {
      cmdVitesse = -0.15;
      moteur.setSpeed(cmdVitesse);
      if (digitalRead(PIN_LIMITSWITCH) == HIGH)
      {
        activePrehenseur();
        cmdVitesse = 0;
        moteur.setSpeed(cmdVitesse);
        delay(200);
        AX_.resetEncoder(0);
        posValue = 0;
        enccooooooooo = 0;
        state = PRISE_SAPIN;
      }
    }
    break;
  case PRISE_SAPIN:
    sprintf(strState, "PRISE_SAPIN");

    calibrationOn = false;
    if (run)
    {
      run = false;
      state = GO_TO;
    }
    break;
  case GO_TO:
    sprintf(strState, "GO_TO");
    moteur.setSpeed(1);
    if ((posValue) >= POS_CIBLE)
    {
      state = DECELERATION;
    }
    break;
  case DECELERATION:
    sprintf(strState, "DECELERATION");
    cmdVitesse = 0.3;
    moteur.setSpeed(cmdVitesse);
    if ((posValue) >= POS_DROP)
    {
      state = STABILISATION;
    }
    break;
  case STABILISATION:
    sprintf(strState, "STABILISATION");


    cmdVitesse = potValue / 85;
    moteur.setSpeed(cmdVitesse);
    if (vitesseAng <= RANGE_VITESSE_ANG_MAX && vitesseAng >= RANGE_VITESSE_ANG_MIN) // doit avoir une position aussi!! non
    {
      state = DROP;
      cmdVitesse = 0;
      moteur.setSpeed(cmdVitesse);
    }
    break;
  case DROP:
    sprintf(strState, "DROP");
    deactivePrehenseur();
    /*if (<3)
     {
       state = RETOUR;
     }*/
    break;
  case RETOUR:
    sprintf(strState, "RETOUR");
    /*if (<3)
    {
      state = INITIAISATION;
    }*/
    break;

    /* default:
     state = INITIALISATION;
     break;*/
  }
  if (shouldRead_)
  {
    readMsg();
  }
  if (shouldSend_)
  {
    sendMsg();
  }

  // mise a jour des chronometres
  timerSendMsg_.update();
  timerPulse_.update();
  int encod = AX_.readEncoder(0);
  double deltaP = ((double)(encod * 2 * PI * RAYONROUE) / (double)(RAPPORTVITESSE * PASPARTOUR));
  enccooooooooo += encod;
  AX_.resetEncoder(0);
  posValue += deltaP;

  potValue = map(analogRead(POTPIN), 170, 850, -850, 850);
  potValue /= 10;

  float dtVitAng = millis() - lastTimeMili ;
  vitesseAng = (potValue - lastPotValue) / dtVitAng;
  lastPotValue = potValue;
  lastTimeMili = millis();
  /*
  Serial.print("CMD: ");
  Serial.println(cmdVitesse);
  */
}

/*---------------------------Definition de fonctions ------------------------*/

void serialEvent() { shouldRead_ = true; }

void timerCallback() { shouldSend_ = true; }

void sendMsg()
{
  /* Envoit du message Json sur le port seriel */
  StaticJsonDocument<500> doc;
  // Elements du message

  doc["time"] = millis();
  
  doc["voltage"] = AX_.getVoltage();
  doc["current"] = AX_.getCurrent();

  // doc["vitesse"] = deltaP;
  doc["position"] = posValue;
  doc["cmdVitesse"] = cmdVitesse;
  doc["state"] = strState;
  doc["encodeur"] = enccooooooooo;
  doc["vitesseang"] = vitesseAng;
  doc["potVex"] = potValue;

  // doc["Consommation"] = consommation;

  // Serialisation
  serializeJson(doc, Serial);
  // Envoit
  Serial.println();
  shouldSend_ = false;
}

void readMsg()
{
  // Lecture du message Json
  StaticJsonDocument<500> doc;
  JsonVariant parse_msg;

  // Lecture sur le port Seriel
  DeserializationError error = deserializeJson(doc, Serial);
  shouldRead_ = false;

  // Si erreur dans le message
  if (error)
  {
    Serial.print("deserialize() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Analyse des éléments du message message
  parse_msg = doc["PWM_des"];
  if (!parse_msg.isNull())
  {
    PWM_des_ = doc["pulse_des"].as<float>();
  }

  parse_msg = doc["voltage"];
  if (!parse_msg.isNull())
  {
    AX_.getVoltage();
  }

  parse_msg = doc["current"];
  if (!parse_msg.isNull())
  {
    AX_.getCurrent();
  }

  parse_msg = doc["cmdVitesse"];
  if (!parse_msg.isNull())
  {
    cmdVitesse = doc["cmdVitesse"].as<float>();
  }

  parse_msg = doc["calibrer"];
  if (!parse_msg.isNull())
  {
    calibrationOn = doc["calibrer"].as<bool>();
  }

  parse_msg = doc["run"];
  if (!parse_msg.isNull())
  {
    run = doc["run"].as<bool>();
  }

  parse_msg = doc["reInit"];
  if (!parse_msg.isNull())
  {
    if (doc["reInit"].as<bool>() == true)
      state = INITIALISATION;
  }

  parse_msg = doc["prehenseur"];
  if (!parse_msg.isNull())
  {
    if (doc["prehenseur"].as<bool>() == true)
      activePrehenseur();
    if (doc["prehenseur"].as<bool>() == false)
      deactivePrehenseur();
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
