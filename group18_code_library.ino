#include <DHT.h>
#include "Adafruit_VEML7700.h"
#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h"
#include <AnalogRead.h>


Adafruit_VEML7700 veml = Adafruit_VEML7700();

// Use this file to store all of the private credentials 
// and connection details

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
String myStatus = "";


#define DHTPIN 2 
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

float hum = 0; //Stores humidity value
float temp; //Stores temperature
float moisture = 0; //Stores soil moisture
float luxx;

#define moist A0
Sensor* origin = NULL;

int fivedelay = 5000;
int thirtydelay = 30000;
unsigned long time_now =0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  if (!veml.begin()) {
  Serial.println("Light sensor not found");
  while (1);
  }
  Serial.println("Light sensor found");
    veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_800MS);

  Serial.print(F("Gain: "));
  switch (veml.getGain()) {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
  }

  Serial.print(F("Integration Time (ms): "));
  switch (veml.getIntegrationTime()) {
    case VEML7700_IT_25MS: Serial.println("25"); break;
    case VEML7700_IT_50MS: Serial.println("50"); break;
    case VEML7700_IT_100MS: Serial.println("100"); break;
    case VEML7700_IT_200MS: Serial.println("200"); break;
    case VEML7700_IT_400MS: Serial.println("400"); break;
    case VEML7700_IT_800MS: Serial.println("800"); break;
  }

  //veml.powerSaveEnable(true);
  //veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);

  veml.setLowThreshold(10000);
  veml.setHighThreshold(20000);
  veml.interruptEnable(true);

  origin = new AnalogRead(moist);


  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
    
  ThingSpeak.begin(client);  //Initialize ThingSpeak
}

void loop() {
  unsigned long curtime = millis();
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      while(millis()< curtime+fivedelay){
      //wait 5s
      
      }
      }
    
    Serial.println("\nConnected.");
  }
  
  //DHT22
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.print(" Celsius,");
  
  
  //Moisture Sensor V2
  Serial.print(" Moisture level: ");
  moisture = origin->read();
  Serial.print(moisture);
  
  
  //VEML7700
  luxx = veml.readLux();
  Serial.print(", Lux: "); Serial.print(veml.readLux());
  Serial.print(", White: "); Serial.println(veml.readWhite());
  Serial.print(", Raw ALS: "); Serial.println(veml.readALS());

  uint16_t irq = veml.interruptStatus();
  if (irq & VEML7700_INTERRUPT_LOW) {
    Serial.println("** Low threshold"); 
  }
  if (irq & VEML7700_INTERRUPT_HIGH) {
    Serial.println("** High threshold"); 
  }
// set the fields with the values
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, hum);
  ThingSpeak.setField(3, moisture);
  ThingSpeak.setField(4, luxx);

  myStatus = String("sent new data");
  
  
  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  while(millis() < curtime + thirtydelay){
    
  }


}
