#define sensor 34
#define rele 13

#include <WiFi.h>
#include <esp_now.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

const char *ssid = "<YOUR_WIFI_NAME>";
const char *password = "<YOUR_WIFI_PASSWORD>"; 
uint8_t broadcastAddress[] = {0x08, 0x3A, 0xF2, 0xAC, 0x7F, 0x94}; // use the broadcast address of your arduino

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3 * 3600;
const int daylightOffset_sec = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec);

typedef struct struct_message {
  char a[32];
  int temperatura;
  int umidadeSolo;
  int umidadeIrrigacao;
  char datetime[20];
} struct_message;

typedef struct humidity_message {
  int humidityIrrigation;
} humidity_message;

struct_message myData;
humidity_message myHumidityIrrigation;
esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *broadcastAddress, esp_now_send_status_t status) {
  Serial.print("Status de envio: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sucesso" : "Falha");
}

void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *incomingData, int len) {
  
  //MAPPING
  memcpy(&myHumidityIrrigation, incomingData, sizeof(myHumidityIrrigation));

  // Serial.print("Bytes received: ");
  // Serial.println(len);
  
  // Serial.print("Sender MAC Address: ");
  // for (int i = 0; i < 6; i++) {
  //   Serial.printf("%02X", recv_info->src_addr[i]);
  // }
  // Serial.println();

  Serial.print("TESTE: ");
  Serial.println(myHumidityIrrigation.humidityIrrigation);
}

void setup() {
  Serial.begin(115200);
  pinMode(sensor, INPUT);
  pinMode(rele, OUTPUT);
  digitalWrite(rele, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  Serial.println("Aguardando sincronização NTP...");
  while (!timeClient.update()) {
    timeClient.forceUpdate();
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTempo sincronizado!");

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

}

void loop() {
  
  int humidityValue = analogRead(sensor);
  int _moisture = map(humidityValue,2704, 1023, 0, 100);
  _moisture = constrain(_moisture, 0, 100);

  if (_moisture <= myHumidityIrrigation.humidityIrrigation) {
    Serial.println("TA REGANDO!!!!!!!!");
    updateLastIrrigation();
  } 

  strcpy(myData.a, "Mensagem do Back para Front");
  myData.temperatura = 23;
  myData.umidadeSolo = getGroundHumidityValue();
  myData.umidadeIrrigacao = myHumidityIrrigation.humidityIrrigation;
  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // }
  // else {
  //   Serial.println("Error sending the data");
  // }

  delay(2000);
}

void plant_monitor() {
  int dryValue = 130;
  int humidityValue = analogRead(sensor);
  int _moisture = map(humidityValue,2704, 1023, 0, 100);
  _moisture = constrain(_moisture, 0, 100);
  
  Serial.print("Valor de umidade: ");
  Serial.print(humidityValue);
  Serial.println(" - " + (String)_moisture + "%");

  if (_moisture <= 40) {
    updateLastIrrigation();
    Serial.println("Estou com sede!");
    digitalWrite(rele, LOW);
    delay(100);
    digitalWrite(rele, HIGH);
  } else {
    Serial.println("Estou me afogando!");
    digitalWrite(rele, HIGH);
  }
  
  delay(1000);
}

int getGroundHumidityValue() {
  int humidityValue = analogRead(sensor);
  int _moisture = map(humidityValue,2704, 1023, 0, 100);
  return constrain(_moisture, 0, 100);
}

void updateLastIrrigation() {
  timeClient.update();
  
  time_t now;
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  snprintf(myData.datetime, sizeof(myData.datetime), "%04d-%02d-%02d %02d:%02d:%02d",
           timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, 
           timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}


