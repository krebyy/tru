#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "opencv2/core/core.hpp"
#include "OCVCapture.h"
#include "capturador.h"
#include "reconhecedor.h"
#include "controlador.h"
#include "pwm.h"
#include "SimpleGPIO.h"
#include "leds.h"
#include "buttons.h"
#include "trimpot.h"
#include "I2Cdev.h"
#include "MPU6050.h"

using namespace std;
using namespace cv;

#define N_COLUNAS   320     // Largura da imagem
#define N_LINHAS    176     // Altura da imagem
#define SP_X (N_COLUNAS / 2)    // Setpoint da direção em relação à imagem (centro da foto)

// Valores auxiliares para o tratamento da rampa
#define SUBIDA 1
#define DESCIDA 2
#define TEMPO_RAMPA 13		// valor * 100 ms: tempo do turbo ou freio
#define INTERVALO_RAMPA 10	// valor * 100 ms: intervalo entre leituras dos eventos de rampa
#define GY_SUBIDA -7000		// limiar do valor lido pelo giroscópio para identificar uma subida
#define GY_DESCIDA 7000		// limiar do valor lido pelo giroscópio para identificar uma descida


//#define DEBUG



void* tickTimerThread(void *ticks_ptr);


int rampa = 0;
unsigned int ticks_rampa = 0;


#endif // MAIN_H
