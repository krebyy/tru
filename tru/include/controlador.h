#ifndef CONTROLADOR_H
#define CONTROLADOR_H


#define SERVO_MIN 900000   // Valor mínimo para controle do servo (direção)
#define SERVO_MAX 1900000   // Valor máximo para controle do servo (direção)


class Controlador
{
public:
    Controlador();
    Controlador(int n_colunas, int n_linhas, int kp_min, int kp_max, int kd, int esc_min, int esc_max);

    int direcao(int erro[]);
    int velocidade(int n_linhas_ok);
    int map(int in, int in_min, int in_max, int out_min, int out_max);
    void setVelocMin(int esc_min);
    void setVelocMax(int esc_max);
    void setKp(int kp_min);
    void setKd(int kd);


private:
    int n_colunas;  // Largura da imagem
    int n_linhas;   // Altura da imagem
    int sp_x;       // Setpoint da direção em relação à imagem (centro da foto)
    int kp_min;     // Valor mínimo do Kp dinâmico
    int kp_max;     // Valor máximo do Kp dinâmico
    int kd;         // Constante derivativa do controlador PD de direção
    int esc_min;    // Valor mínimo para controle do ESC (velocidade)
    int esc_max;    // Valor máximo para controle do ESC (velocidade)

    float a;
    float b;
    int c;
    float kp(int x);    // Kp dinâmico do controlador de direção
};

#endif // CONTROLADOR_H
