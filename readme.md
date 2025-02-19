# Controle de LEDs e Display com Joystick na BitDogLab

## 📌 Descrição do Projeto
Este projeto visa a utilização do **Conversor Analógico-Digital (ADC)** do **RP2040** para captar os valores do **joystick** e controlá-los de forma visual e interativa. Ele é desenvolvido na placa **BitDogLab** e inclui as seguintes funcionalidades:

- Controle da intensidade de **LEDs RGB** utilizando **PWM**.
- Exibição da posição do **joystick** em um **display OLED SSD1306**.
- Integração via **I2C**.
- Implementação de **interrupções (IRQ)** e **debouncing** para os botões.
- Alternação de estados do LED Verde e alteração do estilo da borda do display ao pressionar os botões.

---
## 🎯 Objetivos
- Compreender o funcionamento do **ADC** no **RP2040**.
- Aplicar **PWM** para controle de **LEDs RGB** com base nos valores analógicos do **joystick**.
- Utilizar **I2C** para comunicação com o **display SSD1306**.
- Implementar **interrupções** e **debouncing** para botões.

---
## 🛠 Componentes Utilizados
- **Placa BitDogLab** (RP2040)
- **LED RGB** (GPIOs 11, 12, 13)
- **Joystick** (Eixo X: GPIO 26, Eixo Y: GPIO 27)
- **Botão do Joystick** (GPIO 22)
- **Botão A** (GPIO 5)
- **Display OLED SSD1306** (I2C - GPIOs 14 e 15)

---
## 📌 Funcionalidades
### 🎮 Controle dos LEDs RGB com Joystick
- **LED Azul**: Controlado pelo eixo **Y**.
  - Joystick solto (**2048**): LED **desligado**.
  - Movendo para cima (**0 - 2047**): Aumenta o brilho.
  - Movendo para baixo (**2049 - 4095**): Aumenta o brilho.

- **LED Vermelho**: Controlado pelo eixo **X**.
  - Joystick solto (**2048**): LED **desligado**.
  - Movendo para a esquerda (**0 - 2047**): Aumenta o brilho.
  - Movendo para a direita (**2049 - 4095**): Aumenta o brilho.

- LEDs são controlados via **PWM** para suavização.

### 🖥 Exibição no Display OLED
- Um **quadrado de 8x8 pixels** representa a posição do **joystick**.
- O movimento do quadrado é proporcional aos valores capturados pelo **joystick**.
- O display **muda sua borda** ao pressionar o botão do **joystick**.

### 🔘 Funções dos Botões
- **Botão do Joystick (GPIO 22)**:
  - Alterna o estado do **LED Verde**.
  - Modifica a **borda do display**.

- **Botão A (GPIO 5)**:
  - Liga/desliga os **LEDs RGB** controlados por PWM.

---
## 🔧 Requisitos do Projeto
1. **Uso de Interrupções (IRQ)** para botões.
2. **Debouncing via software** para evitar leituras falsas.
3. **Utilização do display OLED (128x64)** para representação do joystick.
4. **Código bem estruturado e comentado** para facilitar o entendimento.

---
## 🚀 Como Rodar o Projeto
1. Clone este repositório:
   ```bash
   git clone https://github.com/LeonardoGermano/tarefa_JOYSTICK.git
   cd tarefa_JOYSTICK
   ```
2. Envie os arquivos para a **BitDogLab**.
3. Conecte os componentes conforme o esquema indicado.
4. Execute o programa e teste as funcionalidades!

---
## 🎥 Entrega
- **demonstrando:**
- Video: [Tarefa Joystick](https://youtu.be/_NrgHh0Fcgc)

---
## 👨‍💻 Autor
**Leonardo Jonatan**


📌 *Projeto desenvolvido como parte das atividades práticas da BitDogLab!* 🚀

