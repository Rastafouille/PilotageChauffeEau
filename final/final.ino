
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/*Put your SSID & Password*/
const char* ssid = "Maiz";  // Enter SSID here
const char* password = "rastafouille";  //Enter Password here

ESP8266WebServer server(80);

//"server.arg("consigne");".
//http://192.168.1.50/?consigne=500

String st = "";
unsigned int myConsigne = 0;

const int PinPWM = 15; 

#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
/* Use software serial for the PZEM
 * Pin 12 Rx (Connects to the Tx pin on the PZEM) --> D6
 * Pin 13 Tx (Connects to the Rx pin on the PZEM)  --> D7
*/
#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 12
#define PZEM_TX_PIN 13
#endif
SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSWSerial);
float voltage = 0.0;
float current = 0.0;
float power = 0.0;




void setup() {
  Serial.begin(115200);
  delay(100);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);

  display.print("Connecting to ");  display.println(ssid);  display.display(); 

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  }
  display.print("IP  ");  display.println(WiFi.localIP());  display.display(); 


  server.on("/set", handle_Set);
  server.on("/update", handle_Update);
  server.onNotFound(handle_NotFound);

  server.begin();
  display.println("HTTP server started");  display.display(); 

}
void loop() {
  server.handleClient();

//   // Read the data from the sensor
   voltage = pzem.voltage();
   current = pzem.current();
   power = pzem.power();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.print("Connected to ");  display.println(ssid);  display.display(); 
  display.print("IP  ");  display.println(WiFi.localIP());  display.display();
  display.print("Consigne : ");  display.println(myConsigne); display.display(); 
  display.print("Puissance : ");  display.print(power); display.print("W");display.display(); 

 delay(1000);
 ecart=myConsigne-power
 
 

}
void handle_Set() {
  st = server.arg("consigne");
  myConsigne = st.toInt();
  //analogWrite(PinPWM, myConsigne);
  server.send(200, "text/plain", "");
}

void handle_Update() {
  server.send(200, "text/plain", String (power));
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
