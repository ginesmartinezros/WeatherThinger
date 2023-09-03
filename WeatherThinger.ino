// Define Sensores

#include <DHT.h> // Incluimos modulo DHT
#define DHTPIN 3 // Pin DHT
#define DHTTYPE DHT11 // Tipo DHT
DHT dht(DHTPIN, DHTTYPE); //Iniciamos objeto DHT

const int sensorLluvia = 2; // Pin FC-37
const int sensorSuelo = 4; // Pin LM393

// Define Thinger

#define THINGER_SERIAL_DEBUG
#define DEBUG_MEMORY
#define _DEBUG_ // Habilitamos los logs en consola
#include <SoftwareSerial.h> // Modulo para controlar la el modulo GSM
#define TINY_GSM_MODEM_SIM800 // Tipo de modulo GSM
#include <TinyGsmClient.h>
#include <ThingerTinyGSM.h> // Modulos para conexión con Thinger.io

#define USERNAME "" // Configuración de red y credenciales de Thinger.io. Reemplaza con tu nombre de usuario en Thinger.io
#define DEVICE_ID "" // Reemplaza con el ID de tu dispositivo en Thinger.io
#define DEVICE_CREDENTIAL "" // Reemplaza con la clave secreta de tu dispositivo en Thinger.io

#define APN_NAME "" // Configuraciones APN de la red GSM usada. Completar con los valores de la teleoperadora usada
#define APN_USER ""
#define APN_PSWD ""

SoftwareSerial SerialAT(10,11); //Iniciamos el objeto SoftwareSerial del modulo GSM
ThingerTinyGSM thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL, SerialAT); //Iniciamos el objeto ThingerTinyGSM de la conexión con nuestra cuenta de Thinger.io

//setup

void setup() 
{ 
  Serial.begin(9600); //Iniciamos conexion serial
  
  Serial.println("Iniciamos sensores");
  dht.begin(); // Iniciamos DHT, sensor lluvia y sensor suelo
  pinMode(sensorLluvia,INPUT);
  pinMode(sensorSuelo,INPUT); 
  
  SerialAT.begin(9600); //Iniciamos conexión con modulo GSM
  Serial.println("Iniciamos red");
  delay(1000);
  Serial.println("Set APN");
  thing.setAPN(APN_NAME, APN_USER, APN_PSWD); // Establecemos el APN
  Serial.println("AT-OK Test");
  SerialAT.println("AT"); // Hacemos una prueba. Madamos el comando AT al modulo GSM
  delay(1000);
  while (SerialAT.available()) {
  Serial.write(SerialAT.read()); //Leemos el output del modulo GSM. Deberiamos leer un OK
  }

  Serial.println("Data delivery"); // Envío de datos a Thinger.io
  thing["WeatherThinger"] >> [](pson &out){ 
  out["humedadAire"] =dht.readHumidity();
  out["temperatura"] =dht.readTemperature();
  out["sueloMojado"] = ! digitalRead(sensorSuelo);
  out["lluvia"] = ! digitalRead(sensorLluvia);
  };
}

// loop

void loop() {
  
  int frecuenciaEnvioMillis=2000;  // Frecuencia de envío
  if (dht.readHumidity()> 1){ // Checkeo de que el DHT funciona bien
    Serial.println("thing.handle()");
    thing.handle();     // Si lo hacem envía datos
  } else {
    Serial.println("DHT mal conectado"); // Si no, me lo hace saber por consola
  }
  
  while (SerialAT.available()) {
  Serial.write(SerialAT.read()); //Lectura de outputs del GSM
  }
  
  delay(frecuenciaEnvioMillis); // Delay
  
}
