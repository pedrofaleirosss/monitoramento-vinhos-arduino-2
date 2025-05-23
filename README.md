
# 🍷 Projeto CP2 – Vinheria Agnello (Edge Computing)

Este projeto foi desenvolvido como continuação do desafio de monitoramento ambiental para a Vinheria Agnello, agora incluindo o controle de temperatura, umidade e funcionalidades avançadas de histórico.

## 🎯 Objetivo

Monitorar e registrar em tempo real as condições do ambiente de armazenamento de vinhos (luminosidade, temperatura e umidade), com alertas visuais e sonoros, display de status em LCD, e registro de eventos críticos em memória EEPROM com controle de tempo e menu interativo via Serial Monitor.

---

## 🛠️ Funcionalidades

- 📊 **Leitura periódica** de:
  - **Temperatura** (DHT11)
  - **Umidade relativa** (DHT11)
  - **Luminosidade** (LDR com `map()`)

- 🚨 **Alertas sonoros e visuais**:
  - LED verde = tudo adequado
  - LED amarelo = alerta leve
  - LED vermelho + buzzer = situação crítica

- 💾 **Gravação na EEPROM**:
  - Apenas quando algum valor estiver fora do ideal
  - Sistema circular: sobrescreve os dados antigos quando o espaço acaba

- 📅 **Registro com hora e data** usando módulo RTC (DS1307)

- 🖥️ **Display LCD 16x2** com:
  - Logo animada DevZilla
  - Valores atuais de temperatura, umidade e luminosidade
  - Alertas quando os valores estão fora dos padrões

- 📂 **Menu interativo via Serial Monitor**:
  - Ver registros atuais
  - Ver histórico completo da EEPROM
  - Alterar o intervalo entre gravações (em segundos)

---

## 💡 Diferenciais do Projeto

- Menu interativo no Serial Monitor que simula uma interface de sistema real
- Controle dinâmico de tempo entre gravações configurável em tempo real

---

## 📚 Bibliotecas Utilizadas

Conforme o `libraries.txt`:

- [`LiquidCrystal_I2C`](https://github.com/johnrickman/LiquidCrystal_I2C)
- [`DHT sensor library`](https://github.com/adafruit/DHT-sensor-library)
- [`RTClib`](https://github.com/adafruit/RTClib)

---

## 🔌 Componentes Utilizados

| Quantidade | Componente               |
|------------|--------------------------|
| 1          | Arduino Uno R3           |
| 1          | LCD 16x2 com I2C         |
| 1          | Sensor DHT11 (temperatura e umidade) |
| 1          | Sensor LDR (luminosidade)|
| 1          | Módulo RTC DS1307        |
| 1          | Buzzer piezoelétrico     |
| 3          | LEDs (verde, amarelo, vermelho) |
| Vários     | Resistores (para Buzzer, LEDs) |
| Vários     | Cabos jumpers            |
| 1          | Protoboard               |

---

## 🔧 Diagrama do Circuito

![image](https://github.com/user-attachments/assets/91283ff0-f310-4dfe-8a0e-0215d288ec29)

---

## ✨ Imagens do Projeto

![image](https://github.com/user-attachments/assets/4a2972f0-9fa4-4784-8c2b-e2a9aa466100)

![image](https://github.com/user-attachments/assets/9f09e840-55c4-47aa-920c-e57ae0058b52)

![image](https://github.com/user-attachments/assets/c5c52d9e-1f3c-4098-a8c7-fc30d15ad8af)

![image](https://github.com/user-attachments/assets/c24de72a-fc1c-49d2-b874-28054295c52d)

![image](https://github.com/user-attachments/assets/2d1dedc3-8425-404f-aae9-260ac852be89)

![image](https://github.com/user-attachments/assets/70b6dbcf-2694-4e32-b437-23df84d337ac)


## 🧪 Como usar

1. Faça o upload do código para o Arduino Uno.
2. Conecte os sensores conforme o diagrama (LDR no A0, DHT11 no A1, RTC via I2C, LCD 16x2 via I2C).
3. Abra o **Serial Monitor** e selecione **baud rate 9600**.
4. No menu que aparece, digite:
   - `0` para exibir o menu.
   - `1` para ver os registros atuais.
   - `2` para exibir o histórico completo da EEPROM.
   - `3` para alterar o tempo entre gravações.

---

## 👨‍💻 Equipe

- Pedro Faleiros
- Luan Felix
- João Lopes
- Felipe Campos
- Leandro Farias

---

## 🏁 Conclusão

Este projeto representa um sistema completo de monitoramento ambiental voltado à preservação da qualidade dos vinhos, utilizando técnicas de edge computing, gravação inteligente e interação com o usuário. Foi desenvolvido com foco em confiabilidade, flexibilidade e clareza visual.
