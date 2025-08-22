// Pinos
const int receptorPin = 14;   // Pino conectado ao Data do receptor
const int ledPin = 27;         // LED onboard do ESP32 (GPIO2)

// Parâmetros do filtro
const int consecutivosNecessarios = 20; // Número de leituras de "1" seguidas para considerar válido
int contadorConsecutivos = 0;

void setup() {
  Serial.begin(115200);
  pinMode(receptorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  int valor = digitalRead(receptorPin);

  // Mostra no Serial Monitor
  Serial.println(valor);

  // Se receber "1"
  if (valor == HIGH) {
    contadorConsecutivos++;
    if (contadorConsecutivos >= consecutivosNecessarios) {
      digitalWrite(ledPin, HIGH); // Acende LED só se passar no filtro
    }
  } else {
    contadorConsecutivos = 0; // Reset se recebeu "0"
    digitalWrite(ledPin, LOW);
  }

  delay(10); // Ajuste: menor delay = mais sensível
}
