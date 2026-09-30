# Sistema de Monitoramento de Fluxo de Água com ESP32

Protótipo que mede quanta água está passando por um cano usando um **ESP32**, um sensor de fluxo **YF-S201**, um display **OLED SSD1306** e um **LED de alerta**. O projeto foi desenvolvido e testado no simulador [Wokwi](https://wokwi.com).

<!-- Quando tiver um print da simulação, salve em imagens/ e remova os marcadores de comentário:
![Circuito no Wokwi](imagens/circuito-wokwi.png)
-->

<!-- Link do projeto no Wokwi:
**Simulação online:** https://wokwi.com/projects/SEU_ID_AQUI
-->

---

## A ideia em uma frase

> A água faz girar uma pequena hélice dentro do sensor. Cada volta gera pulsos elétricos. O ESP32 conta esses pulsos, calcula a vazão e mostra o resultado na tela. Se a vazão passar de um limite, o LED acende.

Esse é o mesmo princípio de qualquer sistema de monitoramento:

```
Fenômeno físico → Sensor → Sinal elétrico → Processamento → Informação
   (água)       (YF-S201)    (pulsos)         (ESP32)      (OLED + LED)
```

---

## Como funciona

### 1. O sensor transforma água em pulsos

O YF-S201 tem um rotor interno. Quando a água passa, o rotor gira e o sensor envia um pulso elétrico a cada fração de volta:

```
____|‾‾|____|‾‾|____|‾‾|____|‾‾|____
```

O ESP32 **não recebe** "a vazão é 4 L/min". Ele recebe apenas pulsos, algo como "chegaram 30 pulsos neste segundo". Todo o resto é cálculo.

### 2. A regra é simples: mais água, mais pulsos

| Situação | Rotor | Pulsos por segundo | Vazão calculada |
|---|---|---|---|
| Pouca água | Gira devagar | Poucos | Baixa |
| Muita água | Gira rápido | Muitos | Alta |

### 3. O ESP32 faz as contas

```mermaid
flowchart TD
    A[Água passa pelo YF-S201] --> B[Rotor gira e gera pulsos]
    B --> C[GPIO27 detecta cada pulso]
    C --> D[Interrupção: pulses++]
    D --> E{Passou 1 segundo?}
    E -- Não --> D
    E -- Sim --> F[Copia e zera o contador]
    F --> G[Frequência = pulsos / tempo]
    G --> H[Vazão = frequência / 7,5]
    H --> I{Vazão > 5 L/min?}
    I -- Sim --> J[LED aceso]
    I -- Não --> K[LED apagado]
    J --> L[Atualiza o OLED]
    K --> L
    L --> E
```

### 4. A matemática

| Grandeza | O que é | Unidade | Como é obtida |
|---|---|---|---|
| **Pulsos** | O que o sensor entrega | pulsos | Contados pela interrupção |
| **Frequência** | Pulsos por segundo | Hz | `pulsos ÷ tempo` |
| **Vazão** | Volume de água por minuto | L/min | `frequência ÷ 7,5` |

**Exemplo:** 30 pulsos em 1 segundo → 30 Hz → 30 ÷ 7,5 = **4 L/min**

> **Importante:** O valor **7,5** é uma referência comum para o YF-S201, não uma constante exata. Cada sensor físico deve ser calibrado (veja [Calibração](#calibração)).

---

## Componentes

| Componente | Função no projeto |
|---|---|
| ESP32 DevKit | O "cérebro": conta pulsos, calcula e controla tudo |
| Sensor YF-S201 | Transforma o fluxo de água em pulsos elétricos |
| OLED SSD1306 0,96" (I2C) | Mostra pulsos, frequência e vazão |
| LED + resistor 220 Ω | Alerta visual quando a vazão passa do limite |
| Protoboard e jumpers | Montagem sem solda |
| Cabo USB | Alimentação e programação |

---

## Ligações

| Componente | Pino do componente | ESP32 |
|---|---|---|
| OLED | VCC | 3V3 |
| OLED | GND | GND |
| OLED | SDA | GPIO 21 |
| OLED | SCL | GPIO 22 |
| YF-S201 | Sinal (amarelo) | GPIO 27 |
| LED | Ânodo (via resistor 220 Ω) | GPIO 25 |
| LED | Cátodo | GND |

**Apenas na simulação (Wokwi):**

| Componente | Ligação | Função |
|---|---|---|
| Fio de jumper | GPIO 26 → GPIO 27 | Leva os pulsos simulados até a entrada do sensor |
| Potenciômetro | SIG → GPIO 34, VCC → 3V3, GND → GND | Funciona como uma "torneira" virtual |

> **Atenção na montagem física:** o YF-S201 é alimentado com **5 V** e seu sinal pode chegar a 5 V, mas os GPIOs do ESP32 suportam apenas **3,3 V**. Use um divisor de tensão no fio de sinal antes de ligá-lo ao GPIO 27.

---

## Simulação no Wokwi

O Wokwi não possui o sensor YF-S201. Como o ESP32 só precisa enxergar **pulsos**, o próprio ESP32 gera esses pulsos no GPIO 26, e um fio os leva até o GPIO 27. Para o programa, é como se o sensor real estivesse conectado.

```
Montagem real:    YF-S201 ──────────────→ GPIO27 → interrupção → contador
Simulação:        Potenciômetro → GPIO26 → GPIO27 → interrupção → contador
```

Girando o potenciômetro, a frequência varia de 0 a 75 Hz, o que equivale a uma vazão de 0 a 10 L/min:

| Posição do potenciômetro | Frequência | Vazão | LED |
|---|---|---|---|
| Mínimo | 0 Hz | 0 L/min | Apagado |
| Metade | ~37 Hz | ~5 L/min | Limite |
| Máximo | 75 Hz | 10 L/min | Aceso |

A parte de simulação é controlada por uma única linha no código:

```cpp
#define MODO_SIMULACAO 1   // 1 = Wokwi | 0 = sensor real
```

### Como rodar

1. Acesse [wokwi.com](https://wokwi.com) e crie um novo projeto **ESP32**.
2. Copie o conteúdo de `sketch.ino` para a aba **sketch.ino**.
3. Copie o conteúdo de `diagram.json` para a aba **diagram.json**. O circuito aparece montado.
4. Crie a aba **libraries.txt** e copie o conteúdo do arquivo de mesmo nome.
5. Clique no botão verde de iniciar e gire o potenciômetro.

> **Dica:** Se aparecer a mensagem *"Build Servers Busy"*, não é erro no projeto. É fila nos servidores gratuitos do Wokwi. Feche o aviso e tente novamente em alguns instantes.

### O que aparece no display

```
MONITOR DE FLUXO

Pulsos: 30
Freq.: 30.0 Hz
Vazao: 4.00 L/min

Alerta: nao
```

---

## Estrutura do código

| Parte | O que faz |
|---|---|
| `countPulse()` | Função de interrupção: soma 1 ao contador a cada pulso |
| `setup()` | Roda uma vez: configura pinos, OLED e interrupção |
| `loop()` | Roda sempre: a cada 1 s calcula a vazão, atualiza o LED e o display |
| `atualizaSimulacao()` | Só no Wokwi: lê o potenciômetro e ajusta os pulsos gerados |

**Por que usar interrupção?** Em vez de o ESP32 ficar perguntando o tempo todo "chegou pulso?", o próprio hardware avisa quando um pulso chega. Assim nenhum pulso se perde, mesmo enquanto o display está sendo atualizado.

**Por que `millis()` e não `delay()`?** O `delay()` congela o programa. Com `millis()`, o ESP32 apenas verifica se já passou 1 segundo e continua livre para fazer outras tarefas.

### Parâmetros ajustáveis

```cpp
const float FATOR_CALIBRACAO = 7.5;      // Hz por L/min
const float LIMITE_VAZAO     = 5.0;      // L/min para acender o LED
const unsigned long INTERVALO_MS = 1000; // intervalo de medição
```

### Bibliotecas

- `Wire.h`: comunicação I2C (já vem com o ESP32)
- `Adafruit GFX Library`: funções gráficas e de texto
- `Adafruit SSD1306`: controle do display OLED

---

## Calibração

A relação entre pulsos e litros varia de sensor para sensor. Para calibrar o seu:

1. Coloque a saída do sensor sobre um recipiente com marcação de volume.
2. Deixe passar exatamente **1 litro** de água.
3. Anote quantos pulsos foram contados.
4. Esse número é o total de **pulsos por litro** do seu sensor.
5. Ajuste o `FATOR_CALIBRACAO` com a fórmula: `fator = pulsos por litro ÷ 60`.

**Exemplo:** 450 pulsos por litro → 450 ÷ 60 = **7,5**

---

## Limitações atuais

Estas limitações são conhecidas e fazem parte da evolução planejada:

- **Não mede nível de rio.** O YF-S201 mede apenas a água que passa por dentro dele, não a altura da água.
- **Não prevê transbordamento.** Para isso seria preciso medir o nível e sua velocidade de subida.
- **Vazão não é velocidade.** Vazão é volume por tempo (L/min). Velocidade é distância por tempo (m/s). Elas se relacionam por `Q = A × v`, onde A é a área da seção.
- **Um cano não representa um rio.** Um rio tem seção muito maior e velocidades diferentes em cada ponto.
- **Protoboard é só para protótipo.** Uma instalação real exige solda, caixa vedada e proteção contra umidade.

---

## Roadmap

- [x] Documentação do projeto
- [x] Circuito base no Wokwi (ESP32 + OLED + LED)
- [x] Contagem de pulsos por interrupção
- [x] Cálculo de frequência e vazão
- [x] Simulação do YF-S201 com potenciômetro
- [ ] Calibração do sensor
- [ ] Cálculo de volume acumulado
- [ ] Sensor de nível (ultrassônico HC-SR04)
- [ ] Estimativa de tempo até o transbordamento
- [ ] Registro de dados ao longo do tempo
- [ ] Envio dos dados via Wi-Fi (dashboard / servidor)
- [ ] Montagem física com o sensor real

---

## Estrutura do repositório

```
├── README.md          ← você está aqui
├── sketch.ino         ← código do ESP32
├── diagram.json       ← circuito do Wokwi
├── libraries.txt      ← bibliotecas usadas
├── docs/              ← documentação detalhada
└── imagens/           ← prints e fotos do projeto
```

---

## Registro de evolução

| Data | Etapa | Descrição |
|---|---|---|
| (preencher) | Início | Documentação, circuito base e medição de vazão simulada no Wokwi |
