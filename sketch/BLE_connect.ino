#include "BluetoothSerial.h"
#include "esp_mac.h"

// Verifica se o Bluetooth está habilitado
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth não está habilitado! Por favor, execute `make menuconfig` para ativá-lo
#endif

BluetoothSerial SerialBT;

// Definição dos pinos
const int ledPin = 2;       // GPIO2 - LED embutido
const int buttonPin = 4;    // GPIO4 - Botão (configuração PULL-DOWN)

// Variáveis para o controle do botão
unsigned long buttonPressStart = 0;
bool longPressSent = false;
const int longPressDuration = 6000; // 6 segundos

void setup() {
  Serial.begin(115200);
  
  // Configuração dos pinos
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT); // PULL-DOWN externo
  
  // Inicializa o Bluetooth
  SerialBT.begin("ESP32-BT");
  
  Serial.println("Dispositivo Bluetooth iniciado!");
  Serial.println("Nome: ESP32-BT");
  
  // Obter endereço MAC
  uint8_t macBT[6];
  esp_read_mac(macBT, ESP_MAC_BT);
  Serial.printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
  
  // Sinal de inicialização
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  Serial.println("Setup iniciado! ");
}

void loop() {
  checkButton(); // Verifica o botão
  handleBluetooth(); // Processa comunicação Bluetooth
  delay(20); 
}

void checkButton() {
  int buttonState = digitalRead(buttonPin);
  
  // Lógica PULL-DOWN (HIGH quando pressionado)
  if (buttonState == HIGH) {
    if (buttonPressStart == 0) {
      buttonPressStart = millis();
      longPressSent = false;
    } 
    else if (!longPressSent && (millis() - buttonPressStart >= longPressDuration)) {
      sendLongPress();
      longPressSent = true;
    }
  } 
  else {
    buttonPressStart = 0;
  }
}

void sendLongPress() {
  SerialBT.println("1");
  Serial.println("Enviado: 1");
  
  // Feedback visual
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}

void handleBluetooth() {
  if (SerialBT.available()) {
    char receivedChar = SerialBT.read();
    Serial.print("Recebido: ");
    Serial.println(receivedChar);
    processCommand(receivedChar);
    SerialBT.write(receivedChar);
  }
  
  if (Serial.available()) {
    char toSend = Serial.read();
    SerialBT.write(toSend);
  }
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
      SerialBT.println("Comandos:");
      SerialBT.println("1 - Ligar LED");
      SerialBT.println("0 - Desligar LED");
      SerialBT.println("? - Ajuda");
      SerialBT.println("LONG_PRESS - Pressione 6s");
      break;
    default:
      if (String(command).startsWith("LONG_PRESS")) {
        Serial.println("Recebido LONG_PRESS");
      } else {
        SerialBT.print("Comando inválido: ");
        SerialBT.println(command);
      }
  }
}