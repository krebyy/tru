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

//#define DEBUG				// Retire o comentário para visualizar as mensagens de Debug
#define TRATAMENTO_RAMPA	// Retire o comentário para habilitar o tratamento da rampa (turbo e freio)

#define N_COLUNAS   320     // Largura da imagem
#define N_LINHAS    176     // Altura da imagem
#define SP_X (N_COLUNAS / 2)    // Set-point da direção em relação à imagem (centro da foto)

#ifdef TRATAMENTO_RAMPA
	// Estruturas auxiliares para o tratamento da rampa
	#define SUBIDA 1
	#define DESCIDA 2
	#define PATAMAR 3
	#define	PASSOU 4
	#define GY_SUBIDA -10000	// limiar do valor lido pelo giroscópio para identificar uma subida
	#define TURBO 90000			// Incremento de velocidade para execução da rampa

	#define T_LARGADA 6	// valor * 100 ms: tempo de turbo na largada
	#define T_HABILITA_RAMPA 100	// valor * 100 ms: tempo para habilitar rampa após a partida
	#define T_TURBO 6	// valor * 100 ms: tempo com velocidade turbo
	#define T_NORMAL 2	// valor * 100 ms: tempo com velocidade normal durante o patamar
	#define T_FREIO 6	// valor * 100 ms: tempo de atuação do freio
	#define T_FIM 230	// valor * 100 ms: tempo para parar o robô no fim da volta
	#define T_PARADA 205

	void* tickTimerThread(void *ticks_ptr);

	int rampa = 0;
	unsigned int ticks_rampa = 0;
#endif


#endif // MAIN_H
