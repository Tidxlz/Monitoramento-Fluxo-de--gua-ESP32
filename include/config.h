/*
  config.h
  Configuracoes do Sistema de Monitoramento de Fluxo de Agua.
  Todos os pinos e parametros ajustaveis ficam aqui, separados da logica.
*/

#pragma once

// ---------- Modo de operacao ----------
// 1 = simulacao no Wokwi (o ESP32 gera os pulsos do sensor)
// 0 = sensor YF-S201 real ligado ao GPIO27
#define MODO_SIMULACAO 1

// ---------- Pinos ----------
#define OLED_SDA 21   // I2C dados
#define OLED_SCL 22   // I2C clock
#define FLOW_PIN 27   // sinal do YF-S201
#define LED_PIN  25   // LED de alerta

// ---------- Display OLED ----------
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_ENDERECO 0x3C

// ---------- Medicao ----------
const float FATOR_CALIBRACAO = 7.5;      // Hz por L/min (ver docs/calibracao.md)
const float LIMITE_VAZAO     = 5.0;      // L/min para acender o LED
const unsigned long INTERVALO_MS = 1000; // intervalo entre medicoes

// ---------- Simulacao (usado apenas com MODO_SIMULACAO 1) ----------
#define SIM_OUT_PIN  26   // saida de pulsos, ligada por fio ao GPIO27
#define POT_PIN      34   // potenciometro que representa a vazao
#define SIM_FREQ_MAX 75   // Hz (75 / 7,5 = 10 L/min)
#define SIM_CANAL    0    // canal LEDC (usado apenas no core 2.x)
#define SIM_RESOL    10   // resolucao do PWM em bits
