#include "pwm.h"

Pwm::Pwm()
{
}


// Inicialização do PWM do SERVO
bool Pwm::initServo(void)
{
    return configPWM(SERVO, SERVO_PERIODO, SERVO_NEUTRO, 0);
}


// Inicialização do PWM do ESC
bool Pwm::initESC(void)
{
    return configPWM(ESC, ESC_PERIODO, ESC_NEUTRO, 0);
}


// Atualiza o duty-cycle do SERVO
bool Pwm::updateServo(int duty)
{
    return setDuty(SERVO, duty);
}


// Atualiza o duty-cycle do ESC
bool Pwm::updateESC(int duty)
{
    return setDuty(ESC, duty);
}


// Rotina de calibração do ESC
bool Pwm::calibraESC(void)
{
    printf("Iniciando com o valor maximo...\n");
    setDuty(ESC, 2000000);
    usleep(5000000);
    printf("Valor minimo\n");
    setDuty(ESC, 1000000);
    usleep(4000000);
    printf("Valor neutro\n");
    setDuty(ESC, 1500000);
    usleep(4000000);
    printf("Fim da calibracao do ESC\n");

    return true;
}


// Função para configurar o PWM
bool Pwm::configPWM(const char* pino, unsigned long periodo, unsigned long duty, int polaridade)
{
    int fd, len;
    char buf[MAX_BUF];

    // Período do PWM (em microsegundos)
    snprintf(buf, sizeof(buf), SYSFS_PWM_DIR "%s/period", pino);
    fd = open(buf, O_WRONLY);
    len = snprintf(buf, sizeof(buf), "%lu", periodo);
    write(fd, buf, len);
    close(fd);

    // Duty Cycle de 0 a "periodo" (0 a 100%)
    snprintf(buf, sizeof(buf), SYSFS_PWM_DIR "%s/duty", pino);
    fd = open(buf, O_WRONLY);
    len = snprintf(buf, sizeof(buf), "%lu", duty);
    write(fd, buf, len);
    close(fd);

    // Polaridade: 0 (nível lógico alto como parte ativa), 1 (nível lógico baixo como parte ativa)
    snprintf(buf, sizeof(buf), SYSFS_PWM_DIR "%s/polarity", pino);
    fd = open(buf, O_WRONLY);
    len = snprintf(buf, sizeof(buf), "%d", polaridade);
    write(fd, buf, len);
    close(fd);

    // Habilita o PWM
    snprintf(buf, sizeof(buf), SYSFS_PWM_DIR "%s/run", pino);
    fd = open(buf, O_WRONLY);
    len = snprintf(buf, sizeof(buf), "1");
    write(fd, buf, len);
    close(fd);

    return true;
}


// Função para atualizar o Duty Cycle
bool Pwm::setDuty(const char* pino, unsigned long duty)
{
    int fd, len;
    char buf[MAX_BUF];

    // Duty Cycle de 0 a "periodo" (0 a 100%)
    snprintf(buf, sizeof(buf), SYSFS_PWM_DIR "%s/duty", pino);
    fd = open(buf, O_WRONLY);
    len = snprintf(buf, sizeof(buf), "%lu", duty);
    write(fd, buf, len);
    close(fd);

    return true;
}
