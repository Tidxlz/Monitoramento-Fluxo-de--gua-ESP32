/*
  Sistema de Monitoramento de Fluxo de Agua com ESP32
  ESP32 + YF-S201 + OLED SSD1306 + LED

  O ESP32 conta os pulsos do sensor por interrupcao, calcula a
  frequencia e a vazao a cada intervalo e mostra os dados no OLED.
  Pinos e parametros ficam em include/config.h.
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

// Detecta a versao do core do ESP32 (a API do LEDC mudou na versao 3)
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  #define CORE_V3 1
#else
  #define CORE_V3 0
#endif

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

volatile unsigned long pulses = 0;
unsigned long lastMeasure = 0;

// ---------- Interrupcao: chamada a cada pulso do sensor ----------
void IRAM_ATTR countPulse() {
  pulses++;
}

#if MODO_SIMULACAO
// ---------- Gerador de pulsos para simular o YF-S201 ----------
void simIniciar() {
#if CORE_V3
  ledcAttach(SIM_OUT_PIN, 10, SIM_RESOL);
#else
  ledcSetup(SIM_CANAL, 10, SIM_RESOL);
  ledcAttachPin(SIM_OUT_PIN, SIM_CANAL);
#endif
}

void simDuty(uint32_t duty) {
#if CORE_V3
  ledcWrite(SIM_OUT_PIN, duty);
#else
  ledcWrite(SIM_CANAL, duty);
#endif
}

void simFrequencia(uint32_t freq) {
#if CORE_V3
  ledcChangeFrequency(SIM_OUT_PIN, freq, SIM_RESOL);
#else
  ledcChangeFrequency(SIM_CANAL, freq, SIM_RESOL);
#endif
}

// Le o potenciometro e ajusta a frequencia dos pulsos no GPIO26
void atualizaSimulacao() {
  static uint32_t freqAtual = 0xFFFFFFFF;
  uint32_t freq = map(analogRead(POT_PIN), 0, 4095, 0, SIM_FREQ_MAX);

  if (freq == freqAtual) return;   // so reconfigura quando muda
  freqAtual = freq;

  if (freq == 0) {
    simDuty(0);                    // sem fluxo = sem pulsos
  } else {
    simFrequencia(freq);
    simDuty(512);                  // onda quadrada 50%
  }
}
#endif

// ---------- Exibe os dados no OLED ----------
void atualizaDisplay(unsigned long p, float frequency, float flow) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("MONITOR DE FLUXO");
  display.println();

  display.print("Pulsos: ");
  display.println(p);

  display.print("Freq.: ");
  display.print(frequency, 1);
  display.println(" Hz");

  display.print("Vazao: ");
  display.print(flow, 2);
  display.println(" L/min");

  display.println();
  display.print("Alerta: ");
  display.println(flow > LIMITE_VAZAO ? "SIM" : "nao");

  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(FLOW_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ENDERECO)) {
    Serial.println("Erro: OLED nao encontrado");
    while (true) {
      delay(1000);
    }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Iniciando...");
  display.display();

#if MODO_SIMULACAO
  simIniciar();
  simDuty(0);
#endif

  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), countPulse, RISING);

  lastMeasure = millis();
  Serial.println("Sistema iniciado");
}

void loop() {
#if MODO_SIMULACAO
  atualizaSimulacao();
#endif

  unsigned long agora = millis();

  if (agora - lastMeasure >= INTERVALO_MS) {
    unsigned long dt = agora - lastMeasure;  // tempo real decorrido
    lastMeasure = agora;

    // Copia e zera o contador com seguranca
    noInterrupts();
    unsigned long p = pulses;
    pulses = 0;
    interrupts();

    // Frequencia corrigida pelo tempo real (caso passe um pouco de 1 s)
    float frequency = p * 1000.0 / dt;
    float flow = frequency / FATOR_CALIBRACAO;

    digitalWrite(LED_PIN, flow > LIMITE_VAZAO ? HIGH : LOW);
    atualizaDisplay(p, frequency, flow);

    Serial.printf("Pulsos: %lu | Freq: %.1f Hz | Vazao: %.2f L/min\n",
                  p, frequency, flow);
  }
}
