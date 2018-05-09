#include <ESP8266WiFi.h>
#include "Gsender.h"
#pragma region Globals

#define D0   16 //GPIO16 - WAKE UP
#define D1   5  //GPIO5
#define D2   4  //GPIO4
#define D3   0  //GPIO0
#define D4   2  //GPIO2 - TXD1
#define D5   14 //GPIO14 - HSCLK
#define D6   12 //GPIO12 - HMISO
#define D7   13 //GPIO13 - HMOSI - RXD2
#define D8   15 //GPIO15 - HCS   - TXD2
#define RX   3  //GPIO3 - RXD0 
#define TX   1  //GPIO1 - TXD0

const char* ssid = "xxxxxxxxxxx";     // Nombre de la Red
const char* password = "xxxxx";       // Clave de la Red

uint8_t connection_state = 0;            // Determina el estado de la conexion
uint16_t reconnect_interval = 10000;     // Si no se conecta espera este tiempo para volver a intentar
#pragma endregion Globals
String TramaMensajeGmail = "";

uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
    static uint16_t attempt = 0;
    Serial.print("Conectando a ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);  
        Serial.println(nSSID);
    } else {
        WiFi.begin(ssid, password);
        Serial.println(ssid);
    }

    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 50)
    {
        delay(200);
        Serial.print(".");
    }
    ++attempt;
    Serial.println("");
    if(i == 51) {
        Serial.print("Conexion: supero el limite de tiempo TIMEOUT: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Verifique si el Access Point esta disponible or verifique el SSID y el Password\r\n");
        return false;
    }
    
    Serial.println("Conexion: ESTABLECIDA");
    Serial.print("Direccion IP Leida: ");
    Serial.println(WiFi.localIP());
    return true;
}

void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state)
    {
        delay(50);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}

void setup()
{
  pinMode(D5,OUTPUT);
  pinMode(D1,OUTPUT);
    
    digitalWrite(D5,HIGH);
    digitalWrite(D1,HIGH);
  
  pinMode(A0,INPUT);
  
  Serial.begin(9600);
    
   }

void loop()
{ 
  {
        digitalWrite(D5,HIGH);
       }
  Serial.println(analogRead(A0));
    if (analogRead(A0) < 500) {
      digitalWrite(D5,LOW);
      delay (1000);
      EnviarMensajeGMAIL();
      delay(30000);
      
            if (analogRead(A0) > 500) {
        digitalWrite(D5,HIGH);
       
        EnviarMensajeGMAIL();
       }

    }   
}

void EnviarMensajeGMAIL (void)
{   
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect

    Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
    
    String subject = "MENSAJE - ALARMA HUMOS";

    TramaMensajeGmail += "<html>"; 
    TramaMensajeGmail += "<body>"; 

    TramaMensajeGmail += "<h1>ALERTA SENSOR HUMOS ACTIVADO</h1>"; 
    TramaMensajeGmail += "<br>";
    
    TramaMensajeGmail += "<p>"; 
    TramaMensajeGmail += "<b>El Sensor de Humos a detectado Humo</b>."; 
    TramaMensajeGmail += "<br>";
    TramaMensajeGmail += "<b>se requiere de atención inmediata</b>.";
     
    TramaMensajeGmail += "</p>"; 
    TramaMensajeGmail += "</body>"; 
    TramaMensajeGmail += "</html>";
    
    if(gsender->Subject(subject)->Send("Tucorreo@xxx.xxx", TramaMensajeGmail))  // Aqui va el correo al que quieres enviar el aviso
    {
       
      delay(1000);
      Serial.println("MENSAJE ENVIADO EXITOSAMENTE");
           
    } else {
        
       
        Serial.print("ERROR AL ENVIAR EL MENSAJE: ");
        Serial.println(gsender->getError());
    }
}



