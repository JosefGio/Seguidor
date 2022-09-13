/*////////////////////////////////////
 * File:   main.c                    * 
 * Autor: Josef                      *
 *                                   *
 * Criado dia 7 de Julho de 2022     *
 * Último update 29 de Julho de 2022 *
 *///////////////////////////////////*


#include <avr/interrupt.h>
#include "ADC.h"
#include <stdio.h>
#include <util/delay.h>
#include "PWM.h"
#include "lcd.h"
#include "uart.h"
#include <stdbool.h>  //biblioteca para variável bool

#define In1 PB5 // 13
#define In2 PB4 // 12
#define In3 PD7 // 7
#define In4 PD6 // 6

int PWM_geral = 200; //0 a 1023

// PID
float Kp = 2.5, Ki = 0., Kd = 2.5; // constante de Proporcionalidade, Integral e Diferencial
float P = 0, I = 0, D = 0, PID = 0;
int erro = 0, erro_anterior = 0;

int media_p;
unsigned char valor_max = 12;
unsigned char valor_min = 0;

unsigned char sensores[6];
char buffer[10];
int pesos[6] = {-300, -200, -100, 100 , 200, 300};
int PWM_esquerdo;
int PWM_direito; 
unsigned int millissegundos = 0;
bool flag_start = 0;
// Declaração de funções

void timers1(void);
void f_timer1(void);
void calculo_erro(void);
void calculoPID(int erro);
void controle_motor(void);
void MP(void);
void leitura_sensores(void);
void boot_func(void);
void menu_ihm (void);
void loop(void);
void disp_config(void);
void timer_config(void);
void motor_config(void);
void setup(void);
void motor_frete(void);
void motor_giro_direita(void);
void motor_giro_esquerda(void);
void motor_off(void);
void motor_stop(void);

ISR(ADC_vect)
{
  tratar_leitura_do_ADC();
}

ISR(TIMER0_OVF_vect)
{
  TCNT0 = 6;
  timers1();
}

int main()
{
  setup();
  while (1) loop();

}

void setup(void)
{
  DDRC = 0x00;
  disp_config();
  timer_config();
  motor_config();
  uart_setup(103);
  adc_setup();
  disp_init();                                 //inicializa o display
  disp_cmd(0x0C);                              //desliga o cursor e o blink
  boot_func();                                 //chama fun��o de boot do sistema  
  tratar_leitura_do_ADC();
  sei(); // habilita a chave geral das interrupções
  flag_start = 1;
}

void loop(void)
{
  menu_ihm();
}

void disp_config(void)
{
  set_bit(DDRD,RS);		                         //configura RS como sa�da
  set_bit(DDRB,EN);						                 //configura EN como sa�da
  set_bit(DDRD,D4);                            //configura D4 como sa�da	
  set_bit(DDRD,D5);                            //configura D5 como sa�da
  set_bit(DDRD,D6);                            //configura D6 como sa�da
  set_bit(DDRD,D7);                            //configura D7 como sa�da 
}

void timer_config(void)
{
  TCCR0B = 0x03;   // defino o prescaler em 64
  TCNT0 = 6;       // começa em 6 e vai até 255 gerando um tempo de 1ms
  TIMSK0 = 0x01;   // habilito a interrupção do timer0
}

void motor_config(void)
{
  set_bit(DDRB, In1);
  set_bit(DDRB, In2);
  set_bit(DDRD, In4);
  set_bit(DDRD, In3);
  PWM_init();
  setup_pwm_setFreq(10);  //freq de 2 kHz
}

void motor_frete()
{
  set_bit(PORTB, In1);  //frente
  clear_bit(PORTB, In2);
  clear_bit(PORTD, In4);
  set_bit(PORTD, In3); //frente
}


void motor_giro_direita()
{
  clear_bit(PORTB, In1); //frente
  set_bit(PORTB, In2);
  clear_bit(PORTD, In4);
  set_bit(PORTD, In3); //frente
}

void motor_giro_esquerda()
{
  set_bit(PORTB, In1); //frente
  clear_bit(PORTB, In2);
  set_bit(PORTD, In4); //tras
  clear_bit(PORTD, In3);
}

void motor_off()
{
  clear_bit(PORTB, In1);
  clear_bit(PORTB, In2);
  clear_bit(PORTD, In4);
  clear_bit(PORTD, In3);
}

void motor_stop()
{
  pwm_set_duty_service(0, PWM_CHANNEL_1);
  pwm_set_duty_service(0, PWM_CHANNEL_2);
  motor_off();
}

void MP(void)
{
  leitura_sensores();

  int denominador = 0;
  int numerador = 0;

  for (int i = 0; i < 6; i++)
  {
    sensores[i] = valor_max - sensores[i]; //tratamento para ler linha branca
    denominador += sensores[i];
    numerador += (sensores[i] * pesos [i]);
  }

  if (!denominador)
    media_p = 0;
  else
  {
    media_p = (numerador) / (denominador);
  }

  for (int i = 0; i < 6; i++)
  {
    sprintf(buffer, "%d\t", sensores[i]);
    uart_string_sending_service(buffer);
  }
  sprintf(buffer, "%d\n", media_p);
  uart_string_sending_service(buffer);
  calculo_erro();
  controle_motor();
}

void timers1(void)
{

  static unsigned int c_timer1 = 1;

  if(!flag_start)
    return;
  millissegundos++;

  if (c_timer1 < 10)
  {
    c_timer1++;
  }

  else
  {
    f_timer1(); //10ms
    c_timer1 = 1;
  }
}

void f_timer1(void)
{
  MP();
}

void calculo_erro(void)
{

  static unsigned char Setpoint = 0;

  erro = Setpoint - media_p;

  calculoPID(erro);
}
void calculoPID(int erro)
{
  P = erro;  // Proporcional
  I += erro; // Integral

  D = erro - erro_anterior;
  erro_anterior = erro;
  PID = (Kp * P) + (Ki * I) + (Kd * D);
}

void controle_motor(void)
{

  PWM_esquerdo = PWM_geral - PID;
  PWM_direito = PWM_geral + PID;

  motor_frete();

  if (PWM_esquerdo < 0)
  {
    motor_giro_esquerda();
  }

  else if (PWM_direito < 0)
  {
    motor_giro_direita();
  }

  pwm_set_duty_service(PWM_direito, PWM_CHANNEL_1);
  pwm_set_duty_service(PWM_esquerdo, PWM_CHANNEL_2);
}

void leitura_sensores(void)
{
  for (int i = 0; i < 6; i++)
  {
    if (AD_pins[i] > valor_max)
    {
      sensores[i] = valor_max;
    }

    else if (AD_pins[i] < valor_min)
    {
      sensores[i] = valor_min;
    }
    else
    {
      sensores[i] = AD_pins[i];
    }
  }
}


void boot_func()
{
    
  disp_wr_po('I',0,3);
  disp_write('H');
  disp_write('M');
  disp_write(' ');
  disp_write('A');
  disp_write('V');
  disp_write('R');
  disp_wr_po('.',1,0);
  
  for(int i=0; i<16;i++)
  {
    disp_write('.');
	_delay_ms(125);
  } //end for
  
  disp_clear();
} //end boot_func

void menu_ihm(void)
{
  static unsigned char segundos = 0;
  segundos = millissegundos / 1000;

  disp_text("Line Follower", 0, 2);
  disp_number(segundos, 1,3);
}
