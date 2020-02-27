#include <TinyGPS++.h>                                  // Tiny GPS Plus Library
#include <SoftwareSerial.h>                             // Software Serial Library so we can use other Pins for communication with the GPS module
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// CHANGE TO YOUR WIFI CREDENTIALS
const char* ssid = "Smartbridge";
const char* password = "iotgyan@sb";

// CHANGE TO YOUR DEVICE CREDENTIALS AS PER IN IBM BLUMIX
#define ORG "0wzxse"
#define DEVICE_TYPE "techy"
#define DEVICE_ID "123techy"
#define TOKEN "123456789"  //  Authentication Token OF THE DEVICE

//-------- Customise the below values --------
const char publishTopic[] = "iot-2/evt/location/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

//Instance Creations
TinyGPSPlus gps;                                      
SoftwareSerial ss(D7,D8); //rx,tx 
WiFiClient wifiClient;
PubSubClient client(server, 1883, wifiClient);


int publishInterval = 5000; // 5 seconds
long lastPublishMillis;
void publishData();
String latitude,longitude;
String indianTime,Date;
int hour1,day1,minute1,sec;

void setup() {
  Serial.begin(9600);
  Serial.println();
  ss.begin(9600);
  wifiConnect();

}

void loop() {
 smartDelay(5000); 
latitude=String(gps.location.lat(),6);

 Serial.println("latitude:");
 Serial.println(latitude);
 
 longitude=String(gps.location.lng(),6); 

 Serial.println("longitude:");
 Serial.println(longitude);
 dateSplit();
 indian_time();
    publishData();
delay(2000);
}

void wifiConnect()
{
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}



static void smartDelay(unsigned long ms)                // This custom version of delay() ensures that the gps object is being "fed".
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
//   Serial.println(ss.read());
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

void indian_time()
{
  
  hour1=gps.time.hour();
  day1=gps.date.day();
  minute1=gps.time.minute();
  sec=gps.time.second();
  
  int time1=hour1+5;
  if(time1>23){
    time1-=24;
    day1+=1;
    
  }
  minute1=minute1+30;
  if (minute1>59){
   minute1-=60;
   time1+=1; 
  }
//  Serial.print(time1);
//  Serial.print(":");
//  Serial.print(minute1);

indianTime=time1;
indianTime+=":";
indianTime+=minute1;
indianTime+=":";
indianTime+=sec;
}

void publishData()
{ if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  
  String payload = "{\"d\":{\"lat\":";
  payload += "\""+latitude+"\"";
  payload+="," "\"lon\":";
  payload += "\""+longitude+"\"";
  payload+="," "\"time\":";
  payload += "\""+indianTime+"\"";
  payload+="," "\"date\":";
  payload += "\""+Date+"\"";;
  payload += "}}";
  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);

  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }

}

void dateSplit()
{
  int date1=gps.date.value();
  Serial.println(date1);
  String date2=String(date1);
  Serial.println(date2);
  String dd,mm,yy;
  dd=date2.substring(0,2);
  mm=date2.substring(2,4);
  yy=date2.substring(4);
  Date=dd;
  Date+="/";
  Date+=mm;
  Date+="/";
  Date+=yy;
  Date+="/";
}
