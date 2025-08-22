#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// Pinos
const int rxPin      = 14;    // DATA do receptor (via divisor p/ 3.3V)
const int ledPin     = 27;     // LED do ESP32

// Config (ajuste conforme necessário)
const int ACTIVE_LEVEL = LOW; // Troque para HIGH se seu receptor ficar HIGH com sinal
const uint16_t SAMPLE_MS   = 2;   // período de amostragem em ms
const uint16_t NEED_STREAK = 20;  // nº de amostras consecutivas do nível ativo para validar

// Estado
uint16_t streak = 0;
bool latched = false;   // true = sinal estável detectado
bool sent = false;      // true = já enviamos "1" enquanto latched

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32-RX"); // nome do bluetooth
  pinMode(rxPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println();
  Serial.println("Filtro RF 433 - inicializado");
  Serial.println("0/1 no Serial, LED e envio BT quando sinal estável");
  Serial.printf("ACTIVE_LEVEL=%s, SAMPLE_MS=%u, NEED_STREAK=%u\n",
                (ACTIVE_LEVEL==LOW? "LOW":"HIGH"), SAMPLE_MS, NEED_STREAK);
}

void loop() {
  int v = digitalRead(rxPin);

  // Mostra 0/1 no serial (útil para debugar)
  Serial.println(v == HIGH ? 1 : 0);

  // Conta sequências do nível ativo
  if (v == ACTIVE_LEVEL) {
    if (streak < 0xFFFF) streak++;
  } else {
    // sinal não presente: reseta contadores e flags
    streak = 0;
    if (latched) {
      // houve sinal antes e agora acabou -> enviar "0" via BT (once)
      SerialBT.println("0");
      Serial.println("Enviado: 0");
    }
    latched = false;
    sent = false;
    digitalWrite(ledPin, LOW);
  }

  // Se atingir a estabilidade do sinal e ainda não "latched"
  if (!latched && streak >= NEED_STREAK) {
    latched = true;
    // Acende LED e envia "1" (uma vez)
    digitalWrite(ledPin, HIGH);
    if (!sent) {
      SerialBT.println("1");
      Serial.println("Enviado: 1");
      sent = true;
    }
  }

  delay(SAMPLE_MS);
}
