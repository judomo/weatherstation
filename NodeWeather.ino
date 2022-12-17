/*
Тестировалось на Arduino IDE 1.8.5
Дата тестирования 03.10.2019г.
*/ 

#include <ESP8266WiFi.h>                                // Подключаем библиотеку ESP8266WiFi
#include <Wire.h>                                       // Подключаем библиотеку Wire
#include <Adafruit_BME280.h>                            // Подключаем библиотеку Adafruit_BME280
#include <Adafruit_Sensor.h>                            // Подключаем библиотеку Adafruit_Sensor

#define SEALEVELPRESSURE_HPA (1013.25)                  // Задаем высоту

Adafruit_BME280 bme;                                    // Установка связи по интерфейсу I2C

const char* ssid = "TP-Link_E71A";          // Название Вашей WiFi сети
const char* password = "Some_creative_Pass_2021";     // Пароль от Вашей WiFi сети

WiFiServer server(80);                                  // Указываем порт Web-сервера
String header;

/* Change the threshold value with your own reading */
#define Threshold 800

#define MQ2pin 0
int buzzer = D7;
int Flame_sensor = D5;
int Flame;
float Gas_Sensor_Value; // read analog input pin 0

String str;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);                                // Скорость передачи 115200
  bool status; 

  pinMode(buzzer, OUTPUT); 
  pinMode(Flame_sensor, INPUT);
  pinMode(MQ2pin, INPUT);

                                                       
  if (!bme.begin(0x76)) {                               // Проверка инициализации датчика
    Serial.println("Could not find a valid BME280 sensor, check wiring!"); // Печать, об ошибки инициализации.
    while (1);                                          // Зацикливаем
  }

  Serial.print("Connecting to ");                       // Отправка в Serial port 
  Serial.println(ssid);                                 // Отправка в Serial port 
  WiFi.begin(ssid, password);                           // Подключение к WiFi Сети
  while (WiFi.status() != WL_CONNECTED) {               // Проверка подключения к WiFi сети
    delay(500);                                         // Пауза
    Serial.print(".");                                  // Отправка в Serial port 
  }

  Serial.println("");                                   // Отправка в Serial port 
  Serial.println("WiFi connected.");                    // Отправка в Serial port 
  Serial.println("IP address: ");                       // Отправка в Serial port 
  Serial.println(WiFi.localIP());                       // Отправка в Serial port 
  server.begin();                                  
}

void loop(){

  WiFiClient client = server.available();               // Получаем данные, посылаемые клиентом 

  if (client) {

    Flame = digitalRead(Flame_sensor);
    Gas_Sensor_Value = analogRead(MQ2pin);

    if (Flame== HIGH)
    { 
    digitalWrite(buzzer, HIGH); 
    } 
    else
    {  
    digitalWrite(buzzer, LOW);
    }

    str =String("H= ")+String(bme.readHumidity())+String(" T= ")+String(bme.readTemperature())+String("Gas Level ") + String(Gas_Sensor_Value);
    Serial1.println(str);

    Serial.println("New Client.");                      // Отправка "Новый клиент"
    String currentLine = "";                            // Создаем строку для хранения входящих данных от клиента
    while (client.connected()) {                        // Пока есть соединение с клиентом 
      if (client.available()) {                         // Если клиент активен 
        char c = client.read();                         // Считываем посылаемую информацию в переменную "с"
        Serial.write(c);                                // Отправка в Serial port 
        header += c;
        if (c == '\n') {                                // Вывод HTML страницы 
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");          // Стандартный заголовок HT
            client.println("Content-type:text/html ");
            client.println("Connection: close");        // Соединение будет закрыто после завершения ответа
            client.println("Refresh: 10");              // Автоматическое обновление каждые 10 сек 
            client.println();
            
            client.println("<!DOCTYPE html><html>");    
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
                     
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
            client.println("table { border-collapse: collapse; width:40%; margin-left:auto; margin-right:auto; }");
            client.println("th { padding: 12px; background-color: #0043af; color: white; }");
            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: none; padding: 12px; }");
            client.println(".sensor { color:black; font-weight: bold; background-color: transparent ; padding: 1px; }");
            
            client.println("</style></head><body><h1>Weather Station on BME280 and NODEMCU</h1>");
            client.println("<table><tr><th>Name</th><th>Value</th></tr>");
            client.println("<tr><td>Temperature</td><td><span class=\"sensor\">");
            client.println(bme.readTemperature());
            client.println(" *C</span></td></tr>");    
            client.println("<tr><td>Pressure</td><td><span class=\"sensor\">");
            client.println(bme.readPressure() / 100.0F);
            client.println(" hPa</span></td></tr>");
            client.println("<tr><td>Level above the sea line</td><td><span class=\"sensor\">");
            client.println(bme.readAltitude(SEALEVELPRESSURE_HPA));
            client.println(" m</span></td></tr>"); 
            client.println("<tr><td>Humidity</td><td><span class=\"sensor\">");
            client.println(bme.readHumidity());
            client.println(" %</span></td></tr>"); 
            client.println("<tr><td>Gas Level in the Air</td><td><span class=\"sensor\">");
            client.println(Gas_Sensor_Value);
            client.println(" CO2 Level</span></td></tr>"); 
            client.println("</body></html>");
             client.println("<tr><td>Flame?</td><td><span class=\"sensor\">");
            client.println(Flame);
            client.println(" Flame_detected</span></td></tr>"); 
            client.println("</body></html>");
            
            
            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}