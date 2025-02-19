#include <stdlib.h>
#include <stdio.h>
#include <pico/multicore.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "inc/ssd1306_i2c.h"
#include "inc/ssd1306_font.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"  // Incluia a biblioteca do PWM

// Pinos e configurações de hardware
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C // Endereço do controlador SSD1306

#define LED_RED 12  // LED vermelho conectado neste pino
#define LED_GREEN 11  // LED verde
#define LED_BLUE 13  // LED azul

#define BUTTON_A 5  // Pino do botão A
#define BUTTON_JOYSTICK 22  // Pino do botão do joystick

#define DEBOUNCE_TIME_US 200000  // Tempo de debounce de 200ms

// Variáveis que controlam o estado dos LEDs e botões
static volatile uint32_t last_press_A = 0;  // Marca o momento do último acionamento do botão A
static volatile uint32_t last_press_joystick = 0;  // Marca o momento do último acionamento do botão do joystick

static volatile uint8_t green = 0;  // Estado atual do LED verde
static volatile uint8_t pwm_enabled = 1;  // Estado atual do PWM

// Pinos do joystick
const int vRx = 26;          // Pino de leitura do eixo X do joystick (conectado ao ADC)
const int vRy = 27;          // Pino de leitura do eixo Y do joystick (conectado ao ADC)
const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X do joystick
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y do joystick

// Configuração inicial dos pinos
void setup() {
    // Configuração do botão A
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);  // Define o botão como entrada
    gpio_pull_up(BUTTON_A);  // Ativa o resistor pull-up interno

    // Configuração do botão do joystick
    gpio_init(BUTTON_JOYSTICK);
    gpio_set_dir(BUTTON_JOYSTICK, GPIO_IN);  // Define o botão como entrada
    gpio_pull_up(BUTTON_JOYSTICK);  // Ativa o resistor pull-up interno
    
    // Configuração dos LEDs RGB
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);  // Define LED vermelho como saída
    gpio_put(LED_RED, 0);  // Inicialmente apagado

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);  // Define LED verde como saída
    gpio_put(LED_GREEN, 0);  // Inicialmente apagado

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);  // Define LED azul como saída
    gpio_put(LED_BLUE, 0);  // Inicialmente apagado

    // Configuração do PWM para os LEDs vermelho e azul
    gpio_set_function(LED_RED, GPIO_FUNC_PWM);  // Configura o pino do LED vermelho para PWM
    gpio_set_function(LED_BLUE, GPIO_FUNC_PWM); // Configura o pino do LED azul para PWM

    // Obtém as fatias de PWM para os pinos dos LEDs
    uint slice_num_red = pwm_gpio_to_slice_num(LED_RED);
    uint slice_num_blue = pwm_gpio_to_slice_num(LED_BLUE);

    // Configura o PWM para operar com um contador de 16 bits
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 255);  // Define o valor máximo do PWM como 255 (8 bits)
    pwm_init(slice_num_red, &config, true);  // Inicializa o PWM para o LED vermelho
    pwm_init(slice_num_blue, &config, true); // Inicializa o PWM para o LED azul

    // Configuração do joystick
    adc_init();         // Inicializa o módulo ADC
    adc_gpio_init(vRx); // Configura o pino VRX (eixo X) para entrada ADC
    adc_gpio_init(vRy); // Configura o pino VRY (eixo Y) para entrada ADC
}

// Configuração do barramento I2C
void I2C_init() {
    i2c_init(I2C_PORT, 400 * 1000);  // Configura o I2C para operar a 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);  // Define a função I2C para o pino SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);  // Define a função I2C para o pino SCL
    gpio_pull_up(I2C_SDA);  // Habilita resistor pull-up no pino SDA
    gpio_pull_up(I2C_SCL);  // Habilita resistor pull-up no pino SCL
}

// Inicialização do display SSD1306
void SSD1306_init(ssd1306_t* ssd) {
    oled_init(ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);  // Configura o display
    oled_config(ssd);  // Aplica as configurações ao display
    oled_send_data(ssd);  // Envia os dados para atualizar o estado do display
    oled_fill(ssd, false);  // Limpa a tela do display
    oled_send_data(ssd);  // Atualiza a exibição após limpar
}

// Gerenciador de interrupções para os botões
static void button_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());  // Tempo atual em microssegundos

    // Interrupção do botão A
    if (gpio == BUTTON_A && (events & GPIO_IRQ_EDGE_FALL)) {
        if (current_time - last_press_A > DEBOUNCE_TIME_US) {  // Valida o tempo de debounce
            last_press_A = current_time;  // Atualiza o último momento em que o botão foi pressionado
            pwm_enabled = !pwm_enabled;  // Alterna o estado do PWM
        }
    } 
    // Interrupção do botão do joystick
    else if (gpio == BUTTON_JOYSTICK && (events & GPIO_IRQ_EDGE_FALL)) {
        if (current_time - last_press_joystick > DEBOUNCE_TIME_US) {  // Valida o tempo de debounce
            last_press_joystick = current_time;  // Atualiza o último momento em que o botão foi pressionado
            green = !green;  // Alterna o estado do LED verde
            gpio_put(LED_GREEN, green);  // Atualiza o LED verde
        }
    }
}

// Função para ler os valores dos eixos do joystick (X e Y)
void joystick_read_axis(uint16_t *eixo_x, uint16_t *eixo_y) {
    // Leitura do valor do eixo X do joystick
    adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo X
    sleep_us(2);                     // Pequeno delay para estabilidade
    *eixo_x = adc_read();         // Lê o valor do eixo X (0-4095)

    // Leitura do valor do eixo Y do joystick
    adc_select_input(ADC_CHANNEL_1); // Seleciona o canal ADC para o eixo Y
    sleep_us(2);                     // Pequeno delay para estabilidade
    *eixo_y = adc_read();         // Lê o valor do eixo Y (0-4095)
}

// Função principal
int main() {
    ssd1306_t ssd;
    uint16_t valor_x, valor_y;
    int square_x = 64, square_y = 32;  // Posição inicial do quadrado no display

    stdio_init_all();  // Inicializa a comunicação via USB
    setup();  // Executa a configuração inicial dos periféricos
    I2C_init();  // Configura a interface I2C
    SSD1306_init(&ssd);  // Configura o display OLED SSD1306

    // Habilita interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &button_handler);

    while (true) {
        joystick_read_axis(&valor_x, &valor_y);  // Lê os valores dos eixos do joystick

        // Controla o brilho dos LEDs RGB com base nos valores do joystick
        if (pwm_enabled) {
            // Define uma zona morta para evitar que os LEDs fiquem acesos quando o joystick está parado
            int dead_zone = 100;  // Margem de erro ao redor do valor central (2048)

            // Ajusta o brilho do LED vermelho com base no eixo X
            if (abs(valor_x - 2048) > dead_zone) {
                uint16_t red_brightness = abs(valor_x - 2048) * 255 / 2048;
                pwm_set_gpio_level(LED_RED, red_brightness);
            } else {
                pwm_set_gpio_level(LED_RED, 0);  // Desliga o LED vermelho na zona morta
            }

            // Ajusta o brilho do LED azul com base no eixo Y
            if (abs(valor_y - 2048) > dead_zone) {
                uint16_t blue_brightness = abs(valor_y - 2048) * 255 / 2048;
                pwm_set_gpio_level(LED_BLUE, blue_brightness);
            } else {
                pwm_set_gpio_level(LED_BLUE, 0);  // Desliga o LED azul na zona morta
            }
        } else {
            pwm_set_gpio_level(LED_RED, 0);  // Desliga o LED vermelho
            pwm_set_gpio_level(LED_BLUE, 0); // Desliga o LED azul
        }

        // Atualiza a posição do quadrado no display
        square_x = 64 + (valor_y - 2048) / 64;  // Eixo Y controla o movimento horizontal
        square_y = 32 + (valor_x - 2048) / 64;  // Eixo X controla o movimento vertical

        // Limpa o display e desenha o quadrado
        oled_fill(&ssd, false);
        oled_draw_rectangle(&ssd, square_x, square_y, 8, 8, true);
        oled_send_data(&ssd);

        sleep_ms(100);  // Aguarda antes de atualizar novamente
    }
}