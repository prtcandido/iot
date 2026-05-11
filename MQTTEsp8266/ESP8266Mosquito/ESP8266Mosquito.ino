// 1. Instalação Necessária
// No seu Arduino IDE, vá em Library Manager (Gerenciador de Bibliotecas) e instale:
// 1.  PubSubClient (por Nick O'Leary).
// 2.  (Opcional) ESP8266WiFi (já vem no pacote de placas ESP8266).


// 2. Código Exemplo (C++)

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Configurações de Rede e Broker
const char* ssid = "prtcandido";
const char* password = "Abc123Xyz";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
int value = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando em ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setSocketTimeout(15); // Aumenta o tempo de espera para 15 segundos
  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  // Loop até reconectar
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Criar um ID de cliente único
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Tentativa de conectar
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Mantém a conexão ativa

  unsigned long now = millis();
  // Envia uma mensagem a cada 5 segundos
  if (now - lastMsg > 5000) {
    lastMsg = now;
    value++;
    
    String payload = "Temperatura: " + String(random(20, 30)) + "C (Msg: " + String(value) + ")";
    
    Serial.print("Publicando mensagem: ");
    Serial.println(payload);
    
    // Publicando no tópico definido anteriormente
    client.publish("fatecpg/temperatura", payload.c_str());
  }
}


// 3. Detalhes Importantes da Implementação

// * client.loop(): Esta função é vital. Ela processa pacotes de entrada e mantém o "Keep Alive" com o broker. Sem ela, o broker desconectará o ESP por inatividade.
// * ID do Cliente Único: O Mosquitto não permite dois clientes com o mesmo ID. Se você tiver dois ESPs com o mesmo nome, um derrubará a conexão do outro continuamente. No código, usamos `random()` para evitar isso.
// * Tópico: Monitorar por meio do container docker:
//     docker run --rm --name=assinante -it efrecon/mqtt-client sub -h test.mosquitto.org -t "fatecpg/temperatura"

//Em um cenário real, evite usar `delay()` no `loop`. A estrutura de `millis()` usada acima é a correta, pois permite que o ESP continue processando outras tarefas (como ler sensores ou manter o stack de rede) enquanto espera o tempo de envio.

