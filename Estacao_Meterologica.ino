//ELIDO R. GONZALEZ
//TIA: 20502702
//OBJETOS CONECTADOS
//PROJ. ESTAÇÃO METEREOLÓGICA

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define WIFI_SSID "ERG" // Substitua pelo nome da rede WiFi
#define WIFI_PASSWORD "e01234567890" // Substitua pela senha da rede WiFi
#define MQTT_SERVER "192.168.15.8" // Substitua pelo endereço do servidor MQTT
#define MQTT_PORT 1883 // Porta do servidor MQTT
#define MQTT_USERNAME "gonzae" // Substitua pelo usuário do servidor MQTT (se necessário)
#define MQTT_PASSWORD "mack" // Substitua pela senha do servidor MQTT (se necessário)
#define MQTT_TOPIC "temperatura" // Tópico MQTT para publicar as leituras


#define DHTPIN 15 // Pino de dados do sensor DHT11
#define DHTTYPE DHT11 // Tipo do sensor DHT11

DHT dht(DHTPIN, DHTTYPE); // Cria um objeto do sensor DHT
WiFiClient wifiClient; // Cria um objeto WiFiClient para a conexão MQTT
PubSubClient mqttClient(wifiClient); // Cria um objeto PubSubClient para a comunicação MQTT

//Define contador para o ID da mensagem
int i = 0;

void setup() {
  Serial.begin(115200); // Inicia a comunicação serial
  delay(1000);
  
  // Conecta-se à rede WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando-se à rede WiFi ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado!");
  
  // Conecta-se ao servidor MQTT
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  while (!mqttClient.connected()) {
    Serial.print("Conectando-se ao servidor MQTT ");
    Serial.println(MQTT_SERVER);
    if (mqttClient.connect("ESP32", MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Conectado ao servidor MQTT!");
    } else {
      Serial.print("Falha ao se conectar ao servidor MQTT! Código de erro: ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
  
  // Inicia o sensor DHT11
  dht.begin();
}

void loop() {
  //incrementa 1 ao final do loop no ID da mensagem
  i++;

  delay(4000); // Espera 4 segundos entre cada leitura
  
  // Lê a temperatura e a umidade do sensor DHT11
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  
// Verifica se as leituras foram bem-sucedidas e exibe na porta serial
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha ao ler o sensor DHT!");
  } else {
    Serial.print("ID: ");
    Serial.print(i);
    Serial.print("\t");
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C");
    Serial.print("\t");
    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.println(" %");
  }

  // Formata os dados para envio via MQTT
  String message = "ID : "+ String(i) + "\nTemperatura:" + String(temperatura) + "\nUmidade: " + String(umidade);

  // Publica os dados no tópico MQTT
  mqttClient.publish(MQTT_TOPIC, message.c_str());
  
  // Processa as mensagens recebidas do servidor MQTT
  mqttClient.loop();
  
}

