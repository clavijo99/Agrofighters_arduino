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
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); //IP address assigned to your ESP
    ReadingSensores(true);
    StartWork();
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
    if (contavilizador > 7 ) {
      ReadingSensores(false);
      StartWork();
    }
    if (contavilizador > 60)
    {
      AlertConnection(1);
    }
  }
  digitalWrite(led1, HIGH);
  Serial.println("Conexión exitosa a WiFi");
}

void AlertConnection(int i)
{
  DynamicJsonDocument alert(1024);
  String response = "";
  int res;
  HTTPClient http;

  http.begin("https://agrofighterspro.herokuapp.com/alert/update/");
  http.addHeader("Content-Type", "application/json");

  switch (i) {
    case 1:
      alert["alect_connetion"] = "true";
      deserializeJson(alert, response);
      res = http.PUT(response);
      break;
    case 2:
      alert["error_sensor_temperature"] = "true";
      deserializeJson(alert, response);
      res = http.PUT(response);
      break;
    case 3:
      alert["error_sensor_humidity"] = "true";
      deserializeJson(alert, response);
      res = http.PUT(response);
      break;
    case 4:
      alert["engine_error_1"] = "true";
      deserializeJson(alert, response);
      res = http.PUT(response);
      break;
    case 5:
      alert["engine_error_2"] = "true";
      deserializeJson(alert, response);
      res = http.PUT(response);
      break;
  }
}

void ReadingSensores(boolean connetion)
{
  data[0] = dht.readHumidity();    // Leemos la humedad relativa
  data[1] = dht.readTemperature(); // Leemos la temperatura en grados centígrados (por defecto)
  data[2] = analogRead(A0);

  if (connetion == true)
  {
    if (checkReadingSensor() == true) {
      SendData();
    }
  }
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

  if (httpCode > 0)
  {
    doc["date"] = http.getString();
    doc1["date"] = http.getString();
  }

  http.end();

  http.begin("https://agrofighterspro.herokuapp.com/get/time/");
  int httpCode1 = http.GET();
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


  deserializeJson(doc, response1);

  int httpCode2 = http.POST(response1);

  http.end();
  http.begin("https://agrofighterspro.herokuapp.com/humidity/insert/");
  http.addHeader("Content-Type", "application/json");

  deserializeJson(doc1, response2);

  int httpCode3 = http.POST(response2);

  http.end();
}

boolean checkReadingSensor()
{
  if ((isnan(data[0]) || isnan(data[1]))) {
    AlertConnection(2);
    return false;
  }
  if (isnan(data[2])) {
    AlertConnection(3);
    return false;
  }
  return true;
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
      delay(15000);
    }
  }
}

void StartWork() {
  if (data[1] > 27 && data[2] < 400) {
    if (Cortina = false) {
      Engginers(1);
    }
  } else {
    if (Cortina = true && data[2] <= 400) {
      //activar roseador
    }

  } if (data[2] > 400 && Cortina == true) {
    //cerrar roseador
  } else if (data[2] > 400 && Cortina == false) {
    //cerrar cortina
    Engginers(2);
  }

}
