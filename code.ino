#include <WiFiManager.h> 
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>


#define SS_PIN  5 
#define RST_PIN 27

MFRC522 rfid(SS_PIN, RST_PIN);


const char* mqtt_server = "test.mosquitto.org"; 
int mqtt_port = 1883;

 
WiFiClient espClient;
PubSubClient client(espClient);

 
 
void setup() { 
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    client.setServer(mqtt_server, mqtt_port);
    WiFiManager wm;
    bool res;
    res = wm.autoConnect("AutoConnectAP","password"); 
    if(!res) {
        Serial.println("Failed to connect");
    } 
    else {
        Serial.println("connected.)");
    }
 SPI.begin();
 rfid.PCD_Init();  
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 if (client.connect("ESP32Client")) {
 
 Serial.println("connected");
 } 
 else {
 Serial.print("failed, rc=");
 Serial.print(client.state());
 Serial.println(" try again in 5 seconds");
 delay(5000);
 }
 }
}
void loop() {
 if (!client.connected()) {
 reconnect();
 }
 client.loop();
 rfidloop();
}

void rfidloop() {
  if (rfid.PICC_IsNewCardPresent()) { 
    if (rfid.PICC_ReadCardSerial()) {
      String rfid_uid = "";
      for (int i = 0; i < rfid.uid.size; i++) {
        String uid_part = String(rfid.uid.uidByte[i], HEX); 
        rfid_uid += uid_part;
    }
    Serial.println(rfid_uid);
    String comsg;
    comsg=rfid_uid;
    client.publish("sustenancetest", (char*) comsg.c_str());
    rfid.PICC_HaltA(); 
    rfid.PCD_StopCrypto1(); 
    }
  }
}
