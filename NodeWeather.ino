
#include <ESP8266WiFi.h>                               
#include <Wire.h>                                       
#include <Adafruit_BME280.h>                           
#include <Adafruit_Sensor.h>                            

#define SEALEVELPRESSURE_HPA (1013.25)                  

Adafruit_BME280 bme;                                    

const char* ssid = "TP-Link_E71A";         
const char* password = "Some_creative_Pass_2021";     

WiFiServer server(80);                                  
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
  Serial1.begin(115200);                                
  bool status; 

  pinMode(buzzer, OUTPUT); 
  pinMode(Flame_sensor, INPUT);
  pinMode(MQ2pin, INPUT);

                                                       
  if (!bme.begin(0x76)) {                               
    Serial.println("Could not find a valid BME280 sensor, check wiring!"); 
    while (1);                                          
  }

  Serial.print("Connecting to ");                      
  Serial.println(ssid);                                
  WiFi.begin(ssid, password);                           
  while (WiFi.status() != WL_CONNECTED) {               
    delay(500);                                         
    Serial.print(".");                                
  }

  Serial.println("");                                  
  Serial.println("WiFi connected.");                   
  Serial.println("IP address: ");                       
  Serial.println(WiFi.localIP());                     
  server.begin();                                  
}

void loop(){

  WiFiClient client = server.available();               // Получаем данные, посылаемые клиентом 

  if (client) {

    Flame = digitalRead(Flame_sensor);
    Gas_Sensor_Value = analogRead(MQ2pin);

    if (Flame== HIGH || Gas_Sensor_Value > Threshold)
    { 
    digitalWrite(buzzer, HIGH); 
    } 
    else
    {  
    digitalWrite(buzzer, LOW);
    }

    str =String("H= ")+String(bme.readHumidity())+String(" T= ")+String(bme.readTemperature());
    Serial1.println(str);

    Serial.println("New Client.");                     
    String currentLine = "";                            
    while (client.connected()) {                        
      if (client.available()) {                         
        char c = client.read();                         
        Serial.write(c);                               
        header += c;
        if (c == '\n') {                               
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");          
            client.println("Content-type:text/html ");
            client.println("Connection: close");       
            client.println("Refresh: 10");             
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