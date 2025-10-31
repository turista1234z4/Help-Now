#include <WiFi.h>
#include <HTTPClient.h>

#define RECEIVER_PIN 27

// ====== WiFi ======
const char* ssid = "Wsneto";
const char* password = "shey2007";

// ====== URL do PHP ======
String serverName = "https://darkslategray-sheep-864668.hostingersite.com/receiver.php";

// ====== Variáveis ======
unsigned long janelaInicio = 0;
const unsigned long JANELA_MS =1; // janela em ms
int contador = 0;
const int LIMITE = 1;  // mínimo de pulsos HIGH
bool enviado = false;
const unsigned long DEBOUNCE_MS = 20; // mínimo tempo de HIGH válido

void setup() {
  Serial.begin(115200);
  
  // receptor com pull-down
  pinMode(RECEIVER_PIN, INPUT_PULLDOWN);

  // conecta no WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi!");
  Serial.println("Monitorando com janela de tempo...");
}

void loop() {
  int val = digitalRead(RECEIVER_PIN);

  if (val == HIGH) {
    // debounce: só conta se estiver HIGH por pelo menos DEBOUNCE_MS
    unsigned long inicioHigh = millis();
    while (digitalRead(RECEIVER_PIN) == HIGH) {
      if (millis() - inicioHigh >= DEBOUNCE_MS) {
        contador++;
        break;
      }
    }
  }

  // verifica janela de tempo
  if (millis() - janelaInicio > JANELA_MS) {
    if (contador >= LIMITE && !enviado) {
      Serial.println(">>> ALERTA SOS DETECTADO <<<");
      enviarDados("alerta SOS");
      enviado = true;
    }

    // reinicia para próxima janela
    contador = 0;
    janelaInicio = millis();
    enviado = false;
  }

  delay(5);
}

void enviarDados(String valor) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "valor=" + valor;
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      Serial.print("Resposta do servidor: ");
      Serial.println(http.getString());
    } else {
      Serial.print("Erro HTTP: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi desconectado!");
  }
}