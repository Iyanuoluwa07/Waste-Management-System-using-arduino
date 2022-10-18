#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

 
int triggerPin = 13;
int echoPin = 12;
float pingtime=0,distance=0,realdistance=0,average_distance;

int greenLed = 27;
int redLed = 14;
int buzzerPin = 19;
int i;

float max_threshold=9, min_threshold=38,waste_percentage;
const char* ssid = "iPhone";
const char* password = "GEEBEETEE";
// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
// Service API Key
String apiKey = "P1ROPKQQXCPFW4RB";
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Set timer to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Timer set to 10 seconds (10000)
unsigned long timerDelay = 1000;

 
void setup()
{
 Serial.begin(115200);
 WiFi.begin(ssid, password);
 Serial.println("Connecting");
 while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
 }
 Serial.println("");
 Serial.print("Connected to WiFi network with IP Address: ");
 Serial.println(WiFi.localIP());
 
 Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
 lcd.begin();
 lcd.backlight();
 pinMode(triggerPin,OUTPUT);
 pinMode(echoPin,INPUT);
 pinMode(greenLed,OUTPUT);
 pinMode(redLed,OUTPUT);
 pinMode(buzzerPin,OUTPUT);
 lcd.print(" Ultra sonic");
 lcd.setCursor(0,1);
 lcd.print("Distance Meter");
 delay(2000);
 lcd.clear();
 lcd.print(" Circuit Digest");
 delay(2000);
}
 
void loop()
{
 lcd.clear();
 for(i=1; i<=3; i=i+1){
 digitalWrite(triggerPin,LOW);
 delayMicroseconds(2);
 digitalWrite(triggerPin,HIGH);
 delayMicroseconds(10);
 digitalWrite(triggerPin,LOW);
 delayMicroseconds(2);
 
 pingtime = pulseIn(echoPin,HIGH);
 distance = pingtime*342./20000.;
 realdistance = realdistance + distance;
 delay(300);
 }
 average_distance = realdistance/5.;
 waste_percentage = 100.*(38.-average_distance)/29.;
 if(waste_percentage>100){
 waste_percentage = 100;
 }
 if(waste_percentage < 0){
 waste_percentage = 0;
 }
 if(waste_percentage <= 10){
  digitalWrite(redLed,LOW);
  digitalWrite(greenLed,HIGH); 
 }else{
  digitalWrite(greenLed,LOW);
 }
 if(waste_percentage>=90){
  digitalWrite(greenLed,LOW);
  digitalWrite(redLed,HIGH);
  if(waste_percentage==100){
  digitalWrite(buzzerPin,HIGH);
  delay(750);
  digitalWrite(buzzerPin,LOW);
  }
 }else{
  digitalWrite(redLed,LOW);
 }
 
 lcd.clear();
 lcd.print("Distance:");
 lcd.print(average_distance,1);
 lcd.print("cm");
 lcd.setCursor(0,1);
 lcd.print("Distance:");
 lcd.print(waste_percentage,1);
 lcd.print("%");
 //Send an HTTP POST request every 10 seconds
 if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      Serial.println("Connected");
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "api_key=" + apiKey + "&field1=" + String(waste_percentage);           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      Serial.println(waste_percentage);
      
      /*
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // JSON data to send with HTTP POST
      String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(random(40)) + "\"}";           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);*/
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
 delay(3000);
 realdistance = 0;
}
