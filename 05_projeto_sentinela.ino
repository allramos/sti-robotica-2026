/*
 * ===========================================================
 *  PROJETO FINAL — SENTINELA
 *  Minicurso de Robótica Básica com Arduino
 * ===========================================================
 *
 *  Um posto de vigilância autônomo. Fecha o ciclo completo:
 *
 *      SENSOR (LDR)  →  CONTROLADOR (Arduino)  →  ATUADORES (LED + buzzer)
 *             ▲                                        │
 *             └───────────── AMBIENTE ◀────────────────┘
 *
 *  COMPORTAMENTO ESPERADO
 *  ┌──────────────┬──────────────────────┬─────────────┬──────────┐
 *  │ Ambiente     │ LED luz (~9)         │ LED alerta  │ Buzzer   │
 *  ├──────────────┼──────────────────────┼─────────────┼──────────┤
 *  │ Claro        │ apagado              │ apagado     │ mudo     │
 *  │ Penumbra     │ brilho proporcional  │ apagado     │ mudo     │
 *  │ Escuro total │ brilho máximo        │ piscando    │ sirene   │
 *  └──────────────┴──────────────────────┴─────────────┴──────────┘
 *
 *  CIRCUITO
 *    LDR   : 5V ──[LDR]──┬── A0
 *                        └──[10 kΩ]── GND
 *    Luz   : pino ~9  ──[150Ω]── LED branco/verde ── GND   (pino PWM!)
 *    Alerta: pino 12  ──[150Ω]── LED vermelho     ── GND
 *    Buzzer: pino 8   ── (+) buzzer, (−) ── GND
 */

// ---------- PINOS ----------
const int LDR       = A0;
const int LED_LUZ   = 3;     // precisa ser um pino com ~ (PWM)
const int LED_ALERT = 2;
const int BUZZER    = 8;

// ---------- CALIBRAÇÃO ----------
// ⚠️ SUBSTITUA pelos valores que VOCÊ mediu na Prática 4!
const int CLARO         = 500;   // leitura com a luz normal da sala
const int ESCURO        = 400;   // leitura com o dedo tapando o LDR
const int LIMIAR_ALARME = 300;    // abaixo disso = "intruso detectado"

void setup() {
  pinMode(LED_LUZ,   OUTPUT);
  pinMode(LED_ALERT, OUTPUT);
  pinMode(BUZZER,    OUTPUT);

  Serial.begin(9600);
  Serial.println("=== SENTINELA ONLINE ===");
}

void loop() {
  int leitura = analogRead(LDR);

  // ===== 1. LUZ AUTOMÁTICA (PWM) =====
  // Quanto mais escuro o ambiente, mais forte o LED.
  // map() inverte a faixa: ESCURO→255 (máximo) e CLARO→0 (apagado)
  int brilho = map(leitura, ESCURO, CLARO, 255, 0);
  brilho = constrain(brilho, 0, 255);   // trava entre 0 e 255
  analogWrite(LED_LUZ, brilho);

  // ===== 2. ALARME DE INTRUSO =====
  if (leitura < LIMIAR_ALARME) {
    alarme();
  } else {
    digitalWrite(LED_ALERT, LOW);
    noTone(BUZZER);
  }

  // ===== 3. TELEMETRIA (para depurar e calibrar) =====
  Serial.print("Luz: ");
  Serial.print(leitura);
  Serial.print(" | Brilho: ");
  Serial.print(brilho);
  Serial.print(" | ");
  Serial.println(leitura < LIMIAR_ALARME ? "!! ALARME !!" : "ok");

  delay(50);
}

// ---------------------------------------------------------------
// Um ciclo de alarme: dois bipes alternados com o LED vermelho
// ---------------------------------------------------------------
void alarme() {
  digitalWrite(LED_ALERT, HIGH);
  tone(BUZZER, 880);
  delay(150);

  digitalWrite(LED_ALERT, LOW);
  tone(BUZZER, 440);
  delay(150);
}

/*
 * ===========================================================
 *  SE NÃO FUNCIONAR — CHECKLIST
 * ===========================================================
 *  • O brilho não varia?      → o LED da luz está num pino SEM ~ (PWM)
 *  • Brilho invertido?        → troque a ordem no map(): 0, 255
 *  • Alarme sempre ligado?    → recalibre! Ajuste CLARO/ESCURO/LIMIAR_ALARME
 *                               com os valores que aparecem no Serial Monitor
 *  • Leitura travada em 0 ou 1023? → confira o divisor: 5V, A0 e GND
 *  • Nada acende?             → confira o fio do GND (erro nº 1)
 *
 * ===========================================================
 *  DESAFIOS EXTRAS
 * ===========================================================
 *  1. HISTERESE — o LED treme na penumbra? Use dois limiares:
 *       liga com leitura < 80, mas só desliga com leitura > 120.
 *
 *  2. SIRENE DE VERDADE — troque os dois bipes por:
 *       for (int f = 400; f < 1200; f += 20) { tone(BUZZER, f); delay(5); }
 *
 *  3. MODO FURTIVO — alarme silencioso: só o LED vermelho pisca
 *       em código Morse de SOS ( · · ·  − − −  · · · ).
 *
 *  4. SEM delay() — veja 07_sentinela_avancado: usando millis(), o
 *       sensor continua sendo lido mesmo durante o alarme.
 */
