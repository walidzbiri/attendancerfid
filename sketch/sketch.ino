#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


constexpr uint8_t RST_PIN = 5;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 4;     // Configurable, see typical pin layout above

const int ledRED = 0;
const int ledGREEN = 2;

const char* ssid = "SSID of your WIDI";
const char* password = "PASSWORD of your WIFI";

const char* mqtt_server = "Raspberry PI ip";

WiFiClient espClient;
PubSubClient client(espClient);
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;//create a MIFARE_Key struct named 'key', which will hold the card information

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic home/office/esp1/gpio2, you check if the message is either 1 or 0. Turns the ESP GPIO according to the message
  if(topic=="/esp8266/0"){
      Serial.print("Changing GPIO 0 to ");
      if(messageTemp == "1"){
        digitalWrite(ledRED, HIGH);
        Serial.print("On");
        delay(3000);
        digitalWrite(ledRED, LOW);
      }
      else if(messageTemp == "0"){
        digitalWrite(ledRED, LOW);
        Serial.print("Off");
      }
  }
  if(topic=="/esp8266/2"){
      Serial.print("Changing GPIO 2 to ");
      if(messageTemp == "1"){
        digitalWrite(ledGREEN, HIGH);
        Serial.print("On");
        delay(3000);
        digitalWrite(ledGREEN, LOW);
      }
      else if(messageTemp == "0"){
        digitalWrite(ledGREEN, LOW);
        Serial.print("Off");
      }
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("/esp8266/0");
      client.subscribe("/esp8266/2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void setup() { 
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  for (byte i = 0; i < 6; i++) {
   key.keyByte[i] = 0xFF;
   }
  pinMode(ledRED, OUTPUT);
  pinMode(ledGREEN, OUTPUT);
  digitalWrite(ledRED, LOW);
  digitalWrite(ledGREEN, LOW);  
}
 void loop() 
{
  if (!client.connected()) {
    reconnect();
  }
   if(!client.loop())
   client.connect("ESP8266Client");
   
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! rfid.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.println("");
  rfid.PICC_HaltA();
// Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  Serial.print(F("UID tag :"));
  String content= "";
  byte letter;
  for (byte i = 0; i < rfid.uid.size; i++) 
  {
     Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(rfid.uid.uidByte[i], HEX);
     content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  Serial.println(""); 
  Serial.print(F("Message : "));
  content.toUpperCase();
  if (content.substring(1) == "71 15 3F D5") 
  {
    client.publish("/esp8266/uid",content.substring(1).c_str());
    client.publish("/esp8266/access", "1");
    Serial.println(F("Authorized access"));
  }
 
 else{
  Serial.println(F(" Access denied"));
    client.publish("/esp8266/uid",content.substring(1).c_str());
    client.publish("/esp8266/access", "0");
  }
  delay(2000);
}
