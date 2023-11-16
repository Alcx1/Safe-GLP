#define BLYNK_TEMPLATE_ID           ""
#define BLYNK_TEMPLATE_NAME         "Quickstart Device"
#define BLYNK_AUTH_TOKEN            ""

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Creche Rainha da Paz";
char pass[] = "sandros4nt0";

BlynkTimer timer;

// Adicione esta linha para definir a última vez que a mangueira foi substituída
unsigned long lastReplacementTime = 0;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

//  MQ-2, o buzzer e o LED
const int mq2Pin = A0;
const int buzzerPin = D4;
const int ledPin = D5;

// Variáveis globais
float mq2Value = 0;
bool gasDetected = false;

void setup()
{
  // Inicialize o monitor serial
  Serial.begin(9600);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando à rede Wi-Fi...");
  }
  Serial.println("Conectado à rede Wi-Fi");

  // Inicialize o pino do buzzer e do LED como saída
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);

  // Inicialize a última vez que a mangueira foi substituída com o tempo atual
  lastReplacementTime = millis();
}

void loop()
{
  // Se a conexão à rede Wi-Fi estiver estabelecida
  if (WiFi.status() == WL_CONNECTED) {
    // Aqui você pode enviar comandos pelo monitor serial
    Serial.println("Conectado.");
  }

  // Verifique se passaram 5 anos desde a última substituição da mangueira
  if (millis() - lastReplacementTime >= 5 * 365 * 24 * 60 * 60 * 1000UL) {
    // Emita um aviso de substituição da mangueira
    Serial.println("Aviso: Substitua a mangueira, pois passaram 5 anos desde a última substituição.");

    // Atualize o tempo da última substituição para o atual
    lastReplacementTime = millis();
  }

  // Leitura do sensor MQ-2
  mq2Value = analogRead(mq2Pin);

  // Verifique se o valor do sensor excede um limiar (ajuste conforme necessário)
  if (mq2Value > 400) {
    // Ative o buzzer e acenda o LED
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);

    // Exiba uma mensagem no monitor serial
    Serial.println("Vazamento de gás detectado!");

    // Envie a notificação Blynk
    Blynk.logEvent("EVENTO");

    // Defina a flag de detecção de gás como verdadeira
    gasDetected = true;

    // Aguarde por um curto período (ajuste conforme necessário)
    delay(1000); // Aguarda 1 segundo após a detecção
  } else {
    // Desligue o buzzer e o LED se nenhum vazamento for detectado
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);

    // Redefina a flag de detecção de gás como falsa
    gasDetected = false;
  }
  
  Blynk.run();
  timer.run();
}
