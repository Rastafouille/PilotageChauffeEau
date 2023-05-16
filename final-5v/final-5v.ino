
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Variable pour l'écran OLED connectée en I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// Bibliotheque ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// paramètres de connection au wifi local
const char* ssid = "XXX";  // Enter SSID here
const char* password = "XXX";  //Enter Password here

ESP8266WebServer server(80);

//"server.arg("consigne");".
//http://192.168.1.48/set?consigne=255

// variable pour le pilotage en puissance. la consigne est la valeur de puissance demandée, et la commande la valeur entre 0 et 255 de pilotage du régulateur
// un étalonnage est necessaire pour connaitre le lien entre les deux
String st = "";
unsigned int myConsigne = 0;
float commande = 255.0;
float ecart = 0.0;

// pin PWM de pilotage du régulateur de tension
const int PinPWM = 15; 


// Paramétrage voltmètre
// permet de connaitre la valeur de puissance réelle instantanée
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

  analogWrite(PinPWM, 255);
  
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
  display.print("Consigne : ");  display.print(myConsigne); display.println("W"); display.display();
  display.print("Commande : ");  display.println(commande); display.display(); 
  display.print("Puissance : ");  display.print(power); display.print("W");display.display(); 


// Pilotage du régulateur de tension
 ecart=myConsigne-power;
 if (ecart > 20)
  {commande-=2;} 
 if (ecart < -20)
  {commande+=2;} 
 if (commande > 214) // valeurs a etalonner
  {commande=255;}
 if (commande < 0)
  {commande=0;}
 analogWrite(PinPWM, commande);
 delay(2000);
}

// si une consigne de puissance est récue
void handle_Set() {
  st = server.arg("consigne");
  myConsigne = st.toInt();
  commande=int((3000-myConsigne)/14); // valeurs a etalonner
  analogWrite(PinPWM, commande);
  //server.send(200, "text/plain", String (myConsigne));
  server.send(200, "text/plain", "");
}

// si une demande de retour puissance est demandée
void handle_Update() {
  server.send(200, "text/plain", String (power));
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
