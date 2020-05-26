#include <ESP8266HTTPClient.h>

#include <dummy.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>

#include <DHT.h>
#include <ESP8266WiFi.h>

#define DHTTYPE DHT11
int led1 = 0;
int led2 = 3;

int motorA = 5;
int motorB = 4;

float data[3];
boolean Cortina = true;

// Inicializamos el sensor DHT11
DHT dht(5, DHTTYPE);
const char *nombre = "FAMILIA GOMEZ";
const char *password = "  Clavijo 86088829";

void setup()
{
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);

  Serial.begin(115200);
  WiFi.begin(nombre, password);
  dht.begin();
  CheckConnetion();
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    CheckConnetion();
  }
  else
  {
    Serial.println("");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); //IP address assigned to your ESP
    ReadingSensores(true);
  }
}

void CheckConnetion()
{
  int contavilizador = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(led1, HIGH);
    delay(1000);
    contavilizador++;
    digitalWrite(led1, LOW);
    if (contavilizador > 60)
    {
      AlertConnection();
    }
  }
  digitalWrite(led1, HIGH);
  Serial.println("Conexión exitosa a WiFi");
}

void AlertConnection()
{
}

void ReadingSensores(boolean connetion)
{
  data[0] = dht.readHumidity();    // Leemos la humedad relativa
  data[1] = dht.readTemperature(); // Leemos la temperatura en grados centígrados (por defecto)
  data[2] = analogRead(A0);

  Serial.println(data[0]);
  Serial.println(data[1]);
  Serial.println(data[2]);
  Engginers(1);
  /*
    if (connetion == true)
    {
    SendData();
    }
    else
    {
    }*/
}

void SendData()
{
  String response1 = "", response2 = "";
  HTTPClient http;
  DynamicJsonDocument doc(1024);
  DynamicJsonDocument doc1(1024);
  char json[256];

  http.begin("https://agrofighterspro.herokuapp.com/get/date/");
  int httpCode = http.GET();

  Serial.println("res " + httpCode);
  if (httpCode > 0)
  {
    doc["date"] = http.getString();
    doc1["date"] = http.getString();
  }

  http.end();

  http.begin("https://agrofighterspro.herokuapp.com/get/time/");
  int httpCode1 = http.GET();

  Serial.println("res " + httpCode);
  if (httpCode1 > 0)
  {
    doc["time"] = http.getString();
    doc1["time"] = http.getString();
  }

  http.end();

  ////////////////

  doc["temperature"] = data[1];
  doc1["humedity"] = data[0];

  delay(1000);

  http.begin("https://agrofighterspro.herokuapp.com/temperature/insert/");
  http.addHeader("Content-Type", "application/json");

  String v = "{\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}";
  int httpCode2 = http.POST(v);


  http.end();
  http.begin("https://agrofighterspro.herokuapp.com/humidity/insert/");
  http.addHeader("Content-Type", "application/json");

  int httpCode3 = http.POST(doc2, &response2);

  http.end();
}

void checkReadingSensor()
{
}

void Engginers(int direccion)
{

  if (direccion == 1)
  {
    if (Cortina != true)
    {
      digitalWrite(motorA, HIGH);
      digitalWrite(motorB, LOW);
      delay(15000);
    }
  }
  else
  {
    if (Cortina != false)
    {
      digitalWrite(motorA, HIGH);
      digitalWrite(motorB, LOW);
    }
  }


}
