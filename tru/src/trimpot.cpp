#include "trimpot.h"

using namespace std;


// Construtor - número do canal do AD e limites da escala
Trimpot::Trimpot(unsigned int channel, int out_min, int out_max)
{
	this->channel = channel;
	this->out_min = out_min;
	this->out_max = out_max;
}


// Realiza a leitura do trimpot
int Trimpot::getValue(void)
{
    int fd;
    char buf[MAX_BUF];
    char ch[5];

    snprintf(buf, sizeof(buf), SYSFS_ADC_DIR "/in_voltage%d_raw", channel);
    fd = open(buf, O_RDONLY);
    read(fd, ch, 4);
    close(fd);

    return map(atoi(ch));    // Converte para inteiro e altera a escala
}


// Altera a escala de um número
int Trimpot::map(int in)
{
    float out = (float)(out_max - out_min) / 4095.0;
    out *= (float)in;
    out += out_min;

    return  (int)out;
}
