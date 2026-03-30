#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// Configurações do Wi-Fi
const char* ssid = "prtcandido";
const char* password = "Abc123Xyz";

// Configurações da API
const String serverPath = "https://prtctec.com.br/api/iot_leituras";

// Simula leituras do sensor DHT11
float temperatura = 25.5;
float umidade = 60.0;

// Usado para concatenar valores para o atributo Leitura do JSON.
char buffer[60];

void setup() {
  Serial.begin(9600);
  delay(10);
  
  // Conexão Wi-Fi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  
  if (WiFi.status() == WL_CONNECTED) {
    
    // =========================================================
    // 1. CRIAÇÃO DO JSON
    // =========================================================
    
    // Define o tamanho da memória dinâmica (Document Size) para o JSON
    // O valor 200 é geralmente suficiente para payloads pequenos
    DynamicJsonDocument doc(200);

    // Monta String com os dados simulados de temperatura e umidade
    // %.2f significa: use um float (f) com 2 casas decimais (2)
    snprintf(buffer, sizeof(buffer), "Temp=%.2f;Hum=%.2f", temperatura, umidade);
    // Converte para objeto String do Arduino
    String vLeitura = buffer;

    // Monta o objeto JSON
    doc["leitura"] = vLeitura;
    doc["discriminador"] = "DHT11";
  
    // Converte o objeto JSON para uma string formatada
    String jsonPayload;
    serializeJson(doc, jsonPayload);

    Serial.println("JSON Payload sendo enviado:");
    Serial.println(jsonPayload);
    
    // =========================================================
    // 2. REQUISIÇÃO HTTP POST
    // =========================================================
    
    WiFiClientSecure client;
    HTTPClient http;

    client.setInsecure();

    http.begin(client, serverPath);

    // ************* HEADERS ESSENCIAIS *************
    // Informa ao servidor que o corpo da requisição é JSON
    http.addHeader("Content-Type", "application/json"); 
    // **********************************************
    
    // Envia a requisição POST com o JSON como corpo (payload)
    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      Serial.print("Código de Resposta HTTP: ");
      Serial.println(httpResponseCode);
      
      // Imprime o retorno do servidor (ex: um JSON de confirmação)
      String payload = http.getString();
      Serial.println("Resposta do Servidor:");
      Serial.println(payload);
    } else {
      Serial.print("Erro na requisição HTTP POST: ");
      Serial.println(httpResponseCode);
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    
    http.end();
  } else {
    Serial.println("WiFi desconectado. Tentando reconectar...");
    WiFi.begin(ssid, password);
  }
  
  // Envia a cada 10 segundos
  delay(10000); 
}