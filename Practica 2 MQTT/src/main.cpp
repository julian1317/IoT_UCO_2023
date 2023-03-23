#include <sstream>
#include <string>
#include <iostream>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "*****";                // Enter your WiFi name
const char *password = "*****";         // Enter WiFi password// MQTT Broker
const char *mqtt_broker = "*******"; // Enter your WiFi or Ethernet IP
const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

void conectionCLient()
{
    if (!client.connected())
    {
        client.connect("esp8266-client-");
    }
}

void handleOutput(String topic, String output)
{
    conectionCLient();
    Serial.println(output);
    client.publish(topic.c_str(), output.c_str());
}
String jsonStatus(String json)
{
    client.publish("JsonStatus", "iniciando armado de JSON... ");
    String output = "";
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);

    int day = doc["day_of_week"];
    String time = doc["datetime"];
    String month = "";
    month += time[5];
    month += time[6];
    switch (day)
    {
    case 1:
        output += "Lunes";
        break;

    case 2:
        output += "Martes";
        break;

    case 3:
        output += "Miercoles";
        break;

    case 4:
        output += "Jueves";
        break;

    case 5:
        output += "Viernes";
        break;

    case 6:
        output += "Sabado";
        break;

    case 7:
        output += "Domingo";
        break;

    default:
        output += "Desonocido";
        break;
    }

    output += ", ";
    output += time[8];
    output += time[9];
    output += " de ";

    if (month == "01")
    {
        output += "Enero";
    }

    if (month == "02")
    {
        output += "Febrero";
    }

    if (month == "03")
    {
        output += "Marzo";
    }

    if (month == "04")
    {
        output += "Abril";
    }

    if (month == "05")
    {
        output += "Mayo";
    }

    if (month == "06")
        output += "Junio";
    {
    }

    if (month == "07")
    {
        output += "Julio";
    }

    if (month == "08")
    {
        output += "Agosto";
    }

    if (month == "09")
    {
        output += "Septiembre";
    }

    if (month == "10")
    {
        output += "Octubre";
    }

    if (month == "11")
    {
        output += "Noviembre";
    }

    if (month == "12")
    {
        output += "Diciembre";
    }
    output += " de ";
    output += time[0];
    output += time[1];
    output += time[2];
    output += time[3];
    output += " -- ";
    output += time[11];
    output += time[12];
    output += time[13];
    output += time[14];
    output += time[15];
    client.publish("JsonStatus", "Json completado");
    return output;
}

String handleHttpRequest(String input)
{
    String url = "http://worldtimeapi.org/api/timezone" + input;
    Serial.println("URL: " + url);
    HTTPClient http;
    http.begin(espClient, url);

    int httpCode = http.GET();
    handleOutput("StatusRequest", std::to_string(httpCode).c_str());

    if (httpCode == HTTP_CODE_OK)
    {
        handleOutput("StatusRequest", "Peticion exitosa");
    }
    else
    {
        handleOutput("StatusRequest", httpCode == 404 ? "No se ha podido encontrar la zona horaria que buscabas" : http.errorToString(httpCode));
    }
    Serial.println(httpCode);

    return (http.getString());
    http.end();
}
void callback(char *topic, byte *payload, int length)
{
    Serial.println(" - - - - - - - - - - - -");
    Serial.println("Input recibida correctamente ");
    Serial.println("Input: ");
    String inputText = "";

    for (int i = 0; i < length; i++)
    {
        inputText += (char)payload[i];
    }

    Serial.println(inputText);
    String httpResponse = handleHttpRequest(inputText);
    String parsedJson = jsonStatus(httpResponse);
    handleOutput("output", parsedJson);
    Serial.println(" - - - - - - - - - - - -");
}

void conectarWifi()
{
    // connecting to a WiFi network
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            Serial.println("Connecting to WiFi..");
        }
        Serial.println("Connected to the WiFi network");
    }
}
void setup()
{
    // Set software serial baud to 115200;
    Serial.begin(115200);

    conectarWifi();

    // connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    if (!client.connected())
    {

        while (!client.connected())
        {
            String client_id = "esp8266-client-";
            client_id += String(WiFi.macAddress());

            Serial.printf("Realizando conexion de cliente %s \n", client_id.c_str());
            if (client.connect(client_id.c_str()))
            {
                Serial.println("El cliente se ha conectado correctamente");
                client.publish("alive", "El cliente se ha conectado de forma exitosa");
                client.subscribe("input");
                client.publish("alive", "Cliente activo en canales");
            }
            else
            {
                Serial.print("failed with state ");
                Serial.print(client.state());
                delay(2000);
            }
        }
    }
    else
    {
        client.subscribe("input");
    }
}

void loop()
{

    setup();
    delay(1000);

    client.loop();
}