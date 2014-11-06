#include "controlador.h"

Controlador::Controlador()
{
    this->n_colunas = 640;
    this->n_linhas = 480;
    this->sp_x = n_colunas / 2;
    this->kp_min = 0;
    this->kp_max = 102400;
    this->kd = 10;
    this->esc_min = 1500000;
    this->esc_max = 1700000;

    this->a = (float)(((kp_max - kp_min) * 4.0) / (n_colunas * n_colunas));
    this->b = (float)(-a * n_colunas);
    this->c = kp_max;
}


// Construtor
Controlador::Controlador(int n_colunas, int n_linhas, int kp_min, int kp_max, int kd, int esc_min, int esc_max)
{
    this->n_colunas = n_colunas;
    this->n_linhas = n_linhas;
    this->sp_x = n_colunas / 2;
    this->kp_min = kp_min;
    this->kp_max = kp_max;
    this->kd = kd;
    this->esc_min = esc_min;
    this->esc_max = esc_max;

    this->a = (float)(((kp_max - kp_min) * 4.0) / (n_colunas * n_colunas));
    this->b = (float)(-a * n_colunas);
    this->c = kp_max;
}


// Controle de direção a partir do vetor de erros (anterior e atual)
int Controlador::direcao(int erro[])
{
    // Controlador PD com kp dinâmico em função do erro (desvio em relação ao centro da foto)
    float controlador = ((erro[1] * kp((erro[1] + sp_x))) + ((erro[1] - erro[0]) * kd));
    return map(controlador, (kp_max * -sp_x), (kp_max * sp_x), SERVO_MIN, SERVO_MAX);
}


// Controle de velocidade a partir do número de linhas válidas
int Controlador::velocidade(int n_linhas_ok)
{
    // Relação quadrática entre a velocidade e o número de linhas válidas
    return map((n_linhas_ok * n_linhas_ok), 0, (n_linhas * n_linhas), esc_min, esc_max);
}


// Altera a escala de um número e também verifica os limites
int Controlador::map(int in, int in_min, int in_max, int out_min, int out_max)
{
    float out = (float)(out_max - out_min) / (float)(in_max - in_min);
    out *= (float)(in - in_min);
    out += out_min;

    if(out < out_min) return out_min;
    else if(out > out_max) return out_max;
    else return  (int)out;
}


// Retorna o valor do Kp dinâmico
float Controlador::kp(int x)
{
    return (float)((a*x*x) + (b*x) + c);
}


// Atualiza a equação do Kp dinâmico
void Controlador::setKp(int kp_min)
{
	this->kp_min = kp_min;

	this->a = (float)(((kp_max - kp_min) * 4.0) / (n_colunas * n_colunas));
	this->b = (float)(-a * n_colunas);
	this->c = kp_max;
}


// Atualiza o valor do Kd
void Controlador::setKd(int kd)
{
	this->kd = kd;
}


// Atualiza a velocidade mínima
void Controlador::setVelocMin(int esc_min)
{
	this->esc_min = esc_min;
}


// Atualiza a velocidade máxima
void Controlador::setVelocMax(int esc_max)
{
	this->esc_max = esc_max;
}
