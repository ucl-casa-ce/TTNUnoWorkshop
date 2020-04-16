#include <TheThingsNetwork.h>
// First install "DHT sensor library" via the Library Manager
#include <DHT.h>

// Set your AppEUI and AppKey
const char *appEui = "xxx";
const char *appKey = "xxx";

#define debugSerial Serial
#define loraSerial Serial1

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

#define DHTPIN 2  //Digital Pin 2
#define DHTTYPE DHT22
#define lightPin A0

DHT dht(DHTPIN, DHTTYPE);

//Calls the constructor for the TheThingsNetwork class from the TheThingsNetwork library
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
   
void setup(){
  
  debugSerial.begin(9600);
  loraSerial.begin(57600);
 
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  //Initialise the DHT sensor
  dht.begin();
  
  ttn.showStatus();
  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
 
}


void loop(){
  debugSerial.println("-- LOOP");
  
  float ldr = 5.0*analogRead(lightPin)/1024;
  debugSerial.print("ldr voltage: ");
  debugSerial.println(ldr);

  float Rldr = (50.0/ldr) - 10;
  debugSerial.print("LDR resistance (kOhms): ");
  debugSerial.println(Rldr);

  float Lux = 111.0 - (100.0*Rldr/9.0);
  debugSerial.print("Light levels in Lux: ");
  debugSerial.println(Lux);
  uint16_t light = 100*Lux;

  uint16_t humidity = 100*dht.readHumidity(false);

  // false: Celsius (default)
  // true: Farenheit
  uint16_t temperature = 100*dht.readTemperature(false);

  // Split both words (16 bits) into 2 bytes of 8
  byte payload[6];
  payload[0] = highByte(temperature);
  payload[1] = lowByte(temperature);
  payload[2] = highByte(humidity);
  payload[3] = lowByte(humidity);
  payload[4] = highByte(light);
  payload[5] = lowByte(light);


  debugSerial.print("Temperature: ");
  debugSerial.println(temperature);
  debugSerial.print("Humidity: ");
  debugSerial.println(humidity);
  debugSerial.print("Light: ");
  debugSerial.println(light);

  ttn.sendBytes(payload, sizeof(payload));

  delay(20000);
}
