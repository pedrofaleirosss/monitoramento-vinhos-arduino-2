// Bibliotecas necessárias
#include <LiquidCrystal_I2C.h> // Controle do display LCD com interface I2C
#include "DHT.h"               // Sensor de temperatura e umidade
#include <RTClib.h>            // Relógio de tempo real (RTC)
#include <EEPROM.h>            // Memória EEPROM

// --- Configurações gerais ---
#define LOG_OPTION 1
#define SERIAL_OPTION 1
#define UTC_OFFSET -3          // Fuso horário (-3 para Brasília)

#define DHTPIN 2               // Pino onde o DHT22 está conectado
#define DHTTYPE DHT22          // Tipo do sensor DHT
DHT dht(DHTPIN, DHTTYPE);      // Instancia o sensor

const int ldrPin = A0;         // Pino do LDR (sensor de luz)

// Inicializa o display LCD 16x2 no endereço I2C 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 RTC;                // Relógio de tempo real

// --- LEDs, buzzer e botões ---
const int ledVerde = 8;
const int ledAmarelo = 7;
const int ledVermelho = 6;
const int buzzer = 13;

// --- Variáveis para EEPROM e configuração ---
int endereco = 5; // Endereço atual de gravação na EEPROM
const int numMedicoes = 10;           // Número de amostras para média
int tempoEntreRegistros = 10000;      // Intervalo entre registros em milissegundos
int menu = 0; // Menu inicial

// --- Caracteres personalizados (animações e alerta) ---
// (Cada conjunto define um frame de animação no LCD)

// --- Quadro 1 da animação ---
byte image17a[8] = {B01100, B01000, B10000, B00000, B00000, B00000, B00000, B00000};
byte image01a[8] = {B00000, B00000, B00001, B00010, B00010, B00100, B00100, B00100};
byte image02a[8] = {B00111, B11000, B00100, B00010, B00001, B00010, B00100, B00000};
byte image03a[8] = {B00000, B11000, B00100, B00010, B00010, B00001, B00000, B00000};
byte image04a[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B10000, B01000};
byte image20a[8] = {B11000, B01000, B10000, B00000, B00000, B00000, B00000, B00000};
byte image19a[8] = {B10101, B01010, B00000, B01111, B11000, B10000, B00000, B00000};
byte image18a[8] = {B00100, B00011, B00000, B00000, B00000, B00001, B00001, B00001};

// --- Quadro 2 da animação ---
byte image17b[8] = {B01100, B01000, B10000, B00000, B00000, B00000, B00000, B00000};
byte image01b[8] = {B00000, B00000, B00001, B00010, B00010, B00100, B00100, B00100};
byte image02b[8] = {B00111, B11000, B00000, B01110, B10101, B01110, B00000, B00000};
byte image03b[8] = {B00000, B11000, B00100, B00010, B00010, B00001, B00000, B00000};
byte image04b[8] = {B00100, B00100, B00100, B00000, B00100, B00000, B10000, B01000};
byte image20b[8] = {B11000, B01000, B10000, B00000, B00000, B00000, B00000, B00000};
byte image19b[8] = {B10101, B01010, B00000, B01111, B11000, B10000, B00000, B00000};
byte image18b[8] = {B00100, B00011, B00000, B00000, B00000, B00001, B00001, B00001};

// Símbolo Alerta
byte image31[8] = {B00100, B00100, B00000, B00100, B00000, B00000, B11111, B00000};
byte image30[8] = {B00100, B00100, B01000, B01000, B10000, B10000, B01111, B00000};
byte image32[8] = {B00100, B00100, B00010, B00010, B00001, B00001, B11110, B00000};
byte image14[8] = {B00000, B00000, B00000, B00000, B00000, B00001, B00001, B00001};
byte image16[8] = {B00000, B00000, B00000, B00000, B00000, B10000, B10000, B10000};
byte image15[8] = {B00000, B01110, B10001, B10001, B00100, B00100, B00100, B00100};

// Função que ícone de alerta no LCD
void alertImage() {
  lcd.createChar(0, image31);
  lcd.createChar(1, image30);
  lcd.createChar(2, image32);
  lcd.createChar(3, image14);
  lcd.createChar(4, image16);
  lcd.createChar(5, image15);

  lcd.setCursor(14, 1);
  lcd.write(byte(0));
  lcd.setCursor(13, 1);
  lcd.write(byte(1));
  lcd.setCursor(15, 1);
  lcd.write(byte(2));
  lcd.setCursor(13, 0);
  lcd.write(byte(3));
  lcd.setCursor(15, 0);
  lcd.write(byte(4));
  lcd.setCursor(14, 0);
  lcd.write(byte(5));
}

// --- Função para exibir o quadro atual ---
void desenhaLogoFrame(byte* f0, byte* f1, byte* f2, byte* f3, byte* f4, byte* f5, byte* f6, byte* f7) {
  lcd.createChar(0, f0);
  lcd.createChar(1, f1);
  lcd.createChar(2, f2);
  lcd.createChar(3, f3);
  lcd.createChar(4, f4);
  lcd.createChar(5, f5);
  lcd.createChar(6, f6);
  lcd.createChar(7, f7);

  lcd.setCursor(0, 1); lcd.write(byte(0));
  lcd.setCursor(0, 0); lcd.write(byte(1));
  lcd.setCursor(1, 0); lcd.write(byte(2));
  lcd.setCursor(2, 0); lcd.write(byte(3));
  lcd.setCursor(3, 0); lcd.write(byte(4));
  lcd.setCursor(3, 1); lcd.write(byte(5));
  lcd.setCursor(2, 1); lcd.write(byte(6));
  lcd.setCursor(1, 1); lcd.write(byte(7));
}

unsigned long ultimoLog = 0; // Guarda o tempo da última gravação

// Exibe os registros gravados na EEPROM via Serial
void historicoDados() {
  Serial.println("================= Histórico de Dados ===================");
  for (int address = 5; address < endereco; address += 7) {
    // Exemplo de leitura dos primeiros 6 bytes
    unsigned long timestamp;
    EEPROM.get(address, timestamp);
    // Lê od dados dos sensores
    byte tempLida = EEPROM.read(address + 4);
    byte umidLida = EEPROM.read(address + 5);
    byte luzLida = EEPROM.read(address + 6);

    DateTime dt(timestamp); // Converte timestamp para data/hora

    // Imprime no formato dd/mm/yyyy hh:mm:ss
    Serial.print(dt.day()); 
    Serial.print("/");
    Serial.print(dt.month()); 
    Serial.print("/");
    Serial.print(dt.year());
    Serial.print(" ");
    Serial.print(dt.hour() < 10 ? "0" : ""); // Adiciona zero à esquerda se hora for menor que 10
    Serial.print(dt.hour()); 
    Serial.print(":");
    Serial.print(dt.minute() < 10 ? "0" : ""); // Adiciona zero à esquerda se minuto for menor que 10
    Serial.print(dt.minute()); 
    Serial.print(":");
    Serial.print(dt.second() < 10 ? "0" : ""); // Adiciona zero à esquerda se segundo for menor que 10
    Serial.print(dt.second()); 
    Serial.print(" | ");

    Serial.print("Temperatura: ");
    Serial.print(tempLida);
    Serial.print(" C | ");

    Serial.print("Umidade: ");
    Serial.print(umidLida);
    Serial.print("% | ");

    Serial.print("Luz: ");
    Serial.print(luzLida);
    Serial.println("%");
  }

  Serial.print("\n");
}

// Exibe mensagem de alerta no LCD
void exibeAlerta(String linha1, String linha2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linha1);
  lcd.setCursor(0, 1);
  lcd.print(linha2);
  alertImage();
  delay(3000);
}

// ----- Setup -----
void setup() {
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.init();   // Inicialização do LCD
  lcd.clear();
  lcd.backlight();  // Ligando o backlight do LCD

  // --- Mostrar quadro 1 ---
  desenhaLogoFrame(image17a, image01a, image02a, image03a, image04a, image20a, image19a, image18a);
  delay(500);

  // --- Mostrar quadro 2 ---
  desenhaLogoFrame(image17b, image01b, image02b, image03b, image04b, image20b, image19b, image18b);
  delay(500);

  // Mensagem ao lado do desenho
  lcd.setCursor(5, 0);
  lcd.print("D");
  delay(200);
  lcd.print("e");
  delay(200);
  lcd.print("v");
  delay(200);
  lcd.print("Z");
  delay(200);
  lcd.print("i");
  delay(200);
  lcd.print("l");
  delay(200);
  lcd.print("l");
  delay(200);
  lcd.print("a");
  delay(200);

  // --- Mostrar quadro 1 ---
  desenhaLogoFrame(image17a, image01a, image02a, image03a, image04a, image20a, image19a, image18a);
  delay(500);

  // --- Mostrar quadro 2 ---
  desenhaLogoFrame(image17b, image01b, image02b, image03b, image04b, image20b, image19b, image18b);
  delay(500);

  lcd.setCursor(5, 1);
  lcd.print("B");
  delay(200);
  lcd.print("e");
  delay(200);
  lcd.print("m");
  delay(200);
  lcd.print("-");
  delay(200);
  lcd.print("V");
  delay(200);
  lcd.print("i");
  delay(200);
  lcd.print("n");
  delay(200);
  lcd.print("d");
  delay(200);
  lcd.print("o");
  delay(200);
  lcd.print("!");
  delay(200);

  // --- Mostrar quadro 1 ---
  desenhaLogoFrame(image17a, image01a, image02a, image03a, image04a, image20a, image19a, image18a);
  delay(500);

  // --- Mostrar quadro 2 ---
  desenhaLogoFrame(image17b, image01b, image02b, image03b, image04b, image20b, image19b, image18b);
  delay(500);

  // Inicializa Serial, sensor DHT, RTC
  Serial.begin(9600);
  dht.begin();
  RTC.begin();
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Recupera configurações da EEPROM
  EEPROM.get(0, endereco);
  EEPROM.get(1, tempoEntreRegistros);

  // Validação dos valores
  if (endereco < 5 || endereco >= EEPROM.length() - 7) {
    endereco = 5;
  }
  if (tempoEntreRegistros = 0) {
    tempoEntreRegistros = 10000;
  }

  // Exibe menu inicial no Serial
  Serial.println("============ Menu ============");
  Serial.print("[0] Menu - [1] Registros - [2] - Histórico - [3] - Configurar o tempo");
  Serial.print("\n");
  Serial.print("\n");

  delay(3000);
}


// --- Loop principal ---
void loop() {
  DateTime now = RTC.now();

  // Calculando o deslocamento do fuso horário
  int offsetSeconds = UTC_OFFSET * 3600; // Convertendo horas para segundos
  now = now.unixtime() + offsetSeconds; // Adicionando o deslocamento ao tempo atual

  int somaLDR = 0;
  float somaTemp = 0;
  float somaUmi = 0;

  // Faz médias de 10 leituras
  for (int i = 0; i < numMedicoes; i++) {
    somaLDR += analogRead(ldrPin);
    somaTemp += dht.readTemperature();
    somaUmi += dht.readHumidity();
    delay(100);
  }

  // Calcula médias
  int mediaLDR = somaLDR / numMedicoes;
  float temp = somaTemp / numMedicoes;
  float umid = somaUmi / numMedicoes;
  int luzPercent = map(mediaLDR, 8, 1015, 100, 0);

  // Status de cada variável
  String statusTemp = (temp >= 10 && temp <= 16) ? "OK" : (temp < 10 ? "FRIO" : "QUENTE");
  String statusUmi = (umid >= 60 && umid <= 80) ? "OK" : (umid < 60 ? "SECA" : "UMIDA");
  String statusLuz = (luzPercent <= 40) ? "OK" : (luzPercent <= 60 ? "ALERTA" : "CRITICO");

  // Lógica de alerta geral
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVermelho, LOW);
  noTone(buzzer);

  // Liga LED/buzzer conforme status
  if (statusTemp != "OK" || statusUmi != "OK" || statusLuz != "OK") {
    if (statusTemp == "QUENTE" || statusUmi == "UMIDA" || statusLuz == "CRITICO") {
      digitalWrite(ledVermelho, HIGH);
      tone(buzzer, 200);
    } else {
      digitalWrite(ledAmarelo, HIGH);
      tone(buzzer, 150);
    }
    delay(2000);
    noTone(buzzer);
  } else {
    digitalWrite(ledVerde, HIGH);
  }

  // Exibir dados no LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp   Lum  Umid");
  lcd.setCursor(0, 1);
  lcd.print(temp, 1);
  lcd.print("C");
  lcd.print("  ");
  lcd.setCursor(7, 1);
  lcd.print(luzPercent);
  lcd.print("%  ");
  lcd.setCursor(12, 1);
  lcd.print(umid, 0);
  lcd.print("%");

  // Se menu == 1, mostra também no Serial
  if (menu == 1) {
    Serial.print(now.day()); 
    Serial.print("/");
    Serial.print(now.month()); 
    Serial.print("/");
    Serial.print(now.year());
    Serial.print(" ");
    Serial.print(now.hour() < 10 ? "0" : ""); // Adiciona zero à esquerda se hora for menor que 10
    Serial.print(now.hour()); 
    Serial.print(":");
    Serial.print(now.minute() < 10 ? "0" : ""); // Adiciona zero à esquerda se minuto for menor que 10
    Serial.print(now.minute()); 
    Serial.print(":");
    Serial.print(now.second() < 10 ? "0" : ""); // Adiciona zero à esquerda se segundo for menor que 10
    Serial.print(now.second()); 
    Serial.print(" | ");

    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.print(" C | ");

    Serial.print("Umidade: ");
    Serial.print(umid);
    Serial.print("% | ");

    Serial.print("Luz: ");
    Serial.print(luzPercent);
    Serial.println("%");
  }

  delay(3000);

  // Leitura do Serial (controle do menu)
  if (Serial.available() > 0 && menu != 3) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "1") {
      menu = 1;
    } else if (input == "2") {
      menu = 2;
    } else if (input == "3") {
      menu = 3;
    } else if (input == "0") {
      menu = 0;
    } else {
      for (int i = 0; i < 20; i++) {
        Serial.println();
      }
      Serial.println("Número inválido.");
    }
  }

  // Exibe histórico
  else if (menu == 2) {
    for (int i = 0; i < 20; i++) {
      Serial.println();
    }

    historicoDados();
  }
  // Exibe menu
  else if (menu == 0) {
    for (int i = 0; i < 20; i++) {
      Serial.println();
    }

    Serial.println("============ Menu ============");
    Serial.print("[0] Menu - [1] Registros - [2] - Histórico - [3] - Configurar o tempo");
    Serial.print("\n");
    Serial.print("\n");
  }
  // Configura tempo entre registros 
  else if (menu == 3) {
      for (int i = 0; i < 20; i++) {
        Serial.println();
      }

      Serial.println("========== Configuração de Tempo ==========");
      Serial.println("Digite o tempo entre os registros dos dados na memória (em segundos): ");

      String input = Serial.readStringUntil('\n');
      input.trim();

      if (input != "") {
        tempoEntreRegistros = input.toInt() * 1000;
        Serial.print("Tempo entre registros alterado para: ");
        Serial.print(tempoEntreRegistros);
        Serial.println(" milisegundos.");
        menu = 0;
        EEPROM.put(1, tempoEntreRegistros);
      }
  }

  bool foraDosLimites = statusTemp != "OK" || statusUmi != "OK" || statusLuz != "OK";

  // Exibe no LCD caso os status estejam fora dos padroões
  if (statusTemp == "QUENTE") { exibeAlerta("Temperatura", "Muito Quente"); }
  if (statusTemp == "FRIO") { exibeAlerta("Temperatura", "Muito Frio"); }
  if (statusUmi == "UMIDA") { exibeAlerta("Umidade", "Muito Umido"); }
  if (statusUmi == "SECA") { exibeAlerta("Umidade", "Muito Seco"); }
  if (statusLuz == "ALERTA") { exibeAlerta("Luminosidade", "Alerta"); }
  if (statusLuz == "CRITICO") { exibeAlerta("Luminosidade", "Critico"); }

  // Verifica se passou 60 segundos desde a última gravação
  if (millis() - ultimoLog >= tempoEntreRegistros) {
    if (foraDosLimites) {
      // Verifica se cabe os 6 bytes antes de gravar
      if (endereco > EEPROM.length() - 7) {
        endereco = 0; // volta pro começo (circular)
      }

      int intTemp = (int)(temp);
      int intUmid = (int)(umid);
      int intLuz = (int)luzPercent;

      EEPROM.put(endereco, now.unixtime());
      EEPROM.write(endereco + 4, intTemp);  
      EEPROM.write(endereco + 5, intUmid);  
      EEPROM.write(endereco + 6, intLuz);

      EEPROM.put(0, endereco);

      if (menu == 1) {
        Serial.println("Último registro gravado na memória.");
      }

      endereco += 7;
    }

    // Atualiza o tempo da última gravação (independente de gravar ou não)
    ultimoLog = millis();
  }

  // Leitura do Serial (controle do menu)
  if (Serial.available() > 0 && menu != 3) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "1") {
      menu = 1;
    } else if (input == "2") {
      menu = 2;
    } else if (input == "3") {
      menu = 3;
    } else if (input == "0") {
      menu = 0;
    } else {
      for (int i = 0; i < 20; i++) {
        Serial.println();
      }
      Serial.println("Número inválido.");
    }
  }

  // Exibe histórico
  else if (menu == 2) {
    for (int i = 0; i < 20; i++) {
      Serial.println();
    }

    historicoDados();
  }
  // Exibe menu
  else if (menu == 0) {
    for (int i = 0; i < 20; i++) {
      Serial.println();
    }

    Serial.println("============ Menu ============");
    Serial.print("[0] Menu - [1] Registros - [2] - Histórico - [3] - Configurar o tempo");
    Serial.print("\n");
    Serial.print("\n");
  }
  // Configura tempo entre registros 
  else if (menu == 3) {
      for (int i = 0; i < 20; i++) {
        Serial.println();
      }

      Serial.println("========== Configuração de Tempo ==========");
      Serial.println("Digite o tempo entre os registros dos dados na memória (em segundos): ");

      String input = Serial.readStringUntil('\n');
      input.trim();

      if (input != "") {
        tempoEntreRegistros = input.toInt() * 1000;
        Serial.print("Tempo entre registros alterado para: ");
        Serial.print(tempoEntreRegistros);
        Serial.println(" milisegundos.");
        menu = 0;
        EEPROM.put(1, tempoEntreRegistros);
      }
  }
      
  delay(100);
}
