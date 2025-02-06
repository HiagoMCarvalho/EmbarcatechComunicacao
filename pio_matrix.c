#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "hardware/i2c.h"
#include "font.h"
#include "ssd1306.h"
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define FRAMES 10
#define NUM_PIXELS 25
#define LEDG 11 // GPIO13 LED VERDE
#define LEDB 12 // GPIO13 LED AZUL
#define BOTAOA 5 // Botão A = 5
#define BOTAOB 6 // Botão A = 6
#define OUT_PIN 7 // Matriz de leds

PIO pio = pio0;
bool ok;
uint16_t i;
uint32_t valor_led;
uint sm;
uint offset;
double r = 0.0, b = 0.0, g = 0.0;

ssd1306_t ssd; // Inicializa a estrutura do display

#include "pio_matrix.pio.h"

static volatile uint32_t lastEventButton = 0;

double numeros[FRAMES][NUM_PIXELS] =
{ 
    {
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.01, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.01, 0.01, 0.01, 0.01
    },

    {
    0.0, 0.0, 0.01, 0.0, 0.0,
    0.0, 0.01, 0.01, 0.0, 0.0,
    0.0, 0.0, 0.01, 0.0, 0.0,
    0.0, 0.0, 0.01, 0.0, 0.0,
    0.0, 0.0, 0.01, 0.0, 0.0
    },

    {
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.0, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.01, 0.0, 0.0, 0.0, 0.0,
    0.01, 0.01, 0.01, 0.01, 0.01
    },


    {
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.0, 0.0, 0.0, 0.0, 0.01,
    0.0, 0.01, 0.01, 0.01, 0.01,
    0.0, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.01, 0.01, 0.01, 0.01
    },



    {
    0.01, 0.0, 0.0, 0.01, 0.0,
    0.01, 0.0, 0.0, 0.01, 0.0,
    0.01, 0.01, 0.01, 0.01, 0.0,
    0.0, 0.0, 0.0, 0.01, 0.0,
    0.0, 0.0, 0.0, 0.01, 0.0
    },


    {
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.01, 0.0, 0.0, 0.0, 0.0,
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.0, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.01, 0.01, 0.01, 0.01
    },


    {
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.01, 0.0, 0.0, 0.0, 0.0,
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.01, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.01, 0.01, 0.01, 0.01
    },


    {
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.0, 0.0, 0.0, 0.0, 0.01,
    0.0, 0.0, 0.0, 0.01, 0.0,
    0.0, 0.0, 0.01, 0.0, 0.0,
    0.0, 0.01, 0.0, 0.0, 0.0
    },


    {
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.01, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.01, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.01, 0.01, 0.01, 0.01
    },

    {
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.01, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.01, 0.01, 0.01, 0.01,
    0.0, 0.0, 0.0, 0.0, 0.01,
    0.01, 0.01, 0.01, 0.01, 0.01
    }

 };

 // rotina para definição da intensidade de cores do led e conversao para um binario de 32 bits
uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// rotina para "desenhar" o numero na matriz de leds
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    //funcao para espelhar os leds (Por padrão as imagens aparecem invertidas)
    int ordem_fisica[NUM_PIXELS] = 
    {
        24, 23, 22, 21, 20, 
        15, 16, 17, 18, 19, 
        14, 13, 12, 11, 10,
        5, 6, 7, 8, 9,     
        4, 3, 2, 1, 0       
    };

    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        int indice_fisico = ordem_fisica[i];
        valor_led = matrix_rgb(desenho[indice_fisico], r = 0.0, g = 0.0); // Define a cor do LED
        pio_sm_put_blocking(pio, sm, valor_led); //Manda a configuração do led para o pio
    }
}

// Função de interrupção com debouncing para incremento e decremento dos numeros
void gpio_irq_handler_BOTAO(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - lastEventButton > 200000) // 200 ms de debouncing
    {
            lastEventButton = current_time;
            if(gpio == BOTAOA)
            {   
              gpio_put(LEDG, !(gpio_get(LEDG)));

              if(gpio_get(LEDG))
              {
                ssd1306_fill(&ssd, false);
                ssd1306_draw_string(&ssd, "Led verde on", 20, 30); // Desenha uma string
                ssd1306_send_data(&ssd);
                printf("Led verde ligou\n");
              }
              else
              {
                ssd1306_fill(&ssd, false);
                ssd1306_draw_string(&ssd, "Led verde off", 20, 30); // Desenha uma string
                ssd1306_send_data(&ssd);
                printf("Led verde desligou\n");
              }
                 
            }

            if (gpio == BOTAOB)
            {
              gpio_put(LEDB, !(gpio_get(LEDB)));

              if(gpio_get(LEDB))
              {
                ssd1306_fill(&ssd, false);
                ssd1306_draw_string(&ssd, "Led azul on", 20, 30); // Desenha uma string
                ssd1306_send_data(&ssd);
                printf("Led azul ligou\n");
              }
              else
              {
                ssd1306_fill(&ssd, false);
                ssd1306_draw_string(&ssd, "Led azul off", 20, 30); // Desenha uma string
                ssd1306_send_data(&ssd);
                printf("Led azul desligou\n");
              }
            }                                      
    }
}

int main()
{
  // coloca a frequência de clock para 128 MHz, facilitando a divisão pelo clock
  ok = set_sys_clock_khz(128000, false);

  // Inicialização da saida padrao
  stdio_init_all();

  printf("iniciando a transmissão PIO");
  if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

  //configuracao do pio (offset e state machine)
  offset = pio_add_program(pio, &pio_matrix_program);
  sm = pio_claim_unused_sm(pio, true);
  pio_matrix_program_init(pio, sm, offset, OUT_PIN);

  // Inicializa o LED
  gpio_init(LEDG);              // Inicializa o pino do LED
  gpio_set_dir(LEDG, GPIO_OUT); // Configura o pino como saída
  gpio_put(LEDG, false); // inicializacao do LED desligado

  // Inicializa o LED
  gpio_init(LEDB);              // Inicializa o pino do LED
  gpio_set_dir(LEDB, GPIO_OUT); // Configura o pino como saída
  gpio_put(LEDB, false); // inicializacao do LED desligado

  gpio_init(BOTAOA);             // Inicializa o pino do Botao A
  gpio_set_dir(BOTAOA, GPIO_IN); // Configura o pino como entrada
  gpio_pull_up(BOTAOA);          // Habilita o pull-up interno

  gpio_init(BOTAOB);              // Inicializa o pino do Botao Bs
  gpio_set_dir(BOTAOB, GPIO_IN); // Configura o pino como entrada
  gpio_pull_up(BOTAOB); 
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Configuração da interrupção com callback para botao A
  gpio_set_irq_enabled_with_callback(BOTAOA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler_BOTAO);

  // Configuração da interrupção com callback para botao B
  gpio_set_irq_enabled_with_callback(BOTAOB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler_BOTAO);


 while (true) 
 {
    if (stdio_usb_connected()) 
    {   char c;
        if (scanf("%c", &c) == 1) 
        {
            printf("Recebido: %c\n", c);

            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, &c, 30, 30);
            ssd1306_send_data(&ssd);

            if (c >= '0' && c <= '9') 
            {
                int num = c - '0';
                desenho_pio(numeros[num], valor_led, pio, sm, r, g, b);
            }
        }
    }
}

}