#include "BluetoothSerial.h"
#include "esp_mac.h"
// Verifica se o Bluetooth está habilitado no build
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth não está habilitado! Por favor, execute `make menuconfig` para ativá-lo
#endif

BluetoothSerial SerialBT;

// Pino do LED embutido (ajuste conforme sua placa ESP32)
const int ledPin = 2;  // GPIO2 - LED embutido em muitas placas ESP32

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  
  // Inicializa o Bluetooth
  SerialBT.begin("ESP32-BT"); // Nome do seu dispositivo Bluetooth
  
  Serial.println("Dispositivo Bluetooth iniciado! Pronto para parear");
  Serial.println("Nome do dispositivo: ESP32-BT");
  
  // Obter e imprimir o endereço MAC (forma alternativa)
  uint8_t macBT[6];
  esp_read_mac(macBT, ESP_MAC_BT);
  Serial.printf("Endereço MAC Bluetooth: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
  
  digitalWrite(ledPin, HIGH); // Liga o LED para indicar que está pronto
  delay(1000);
  digitalWrite(ledPin, LOW);
}

void loop() {
  // Verifica se há dados recebidos via Bluetooth
  if (SerialBT.available()) {
    char receivedChar = SerialBT.read();
    Serial.print("Recebido: ");
    Serial.println(receivedChar);
    
    // Processa o comando recebido
    processCommand(receivedChar);
    
    // Ecoa o caractere de volta para o dispositivo conectado
    SerialBT.write(receivedChar);
  }
  
  // Verifica se há dados no Serial para enviar via Bluetooth
  if (Serial.available()) {
    char toSend = Serial.read();
    SerialBT.write(toSend);
  }
  
  delay(20); 
}

void processCommand(char command) {
  switch(command) {
    case '1':
      digitalWrite(ledPin, HIGH);
      SerialBT.println("LED ligado");
      Serial.println("LED ligado via Bluetooth");
      break;
    case '0':
      digitalWrite(ledPin, LOW);
      SerialBT.println("LED desligado");
      Serial.println("LED desligado via Bluetooth");
      break;
    case '?':
      SerialBT.println("Comandos disponíveis:");
      SerialBT.println("1 - Ligar LED");
      SerialBT.println("0 - Desligar LED");
      SerialBT.println("? - Ajuda");
      break;
    default:
      SerialBT.print("Comando não reconhecido: ");
      SerialBT.println(command);
      SerialBT.println("Envie ? para ajuda");
  }
}