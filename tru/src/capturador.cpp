#include "capturador.h"
using namespace cv;
using namespace std;

Capturador::Capturador()
{
    this->n_colunas = 640;
    this->n_linhas = 480;

    cap.setDesiredSize(n_colunas, n_linhas);
    cap.setVerbose(true);
}


// Construtor
Capturador::Capturador(int n_colunas, int n_linhas)
{
    this->n_colunas = n_colunas;
    this->n_linhas = n_linhas;

    cap.setDesiredSize(n_colunas, n_linhas);
    cap.setVerbose(true);
}


// Captura um frame (em escala de cinza) e retorna a imagem binária
bool Capturador::getFrame(Mat& frame)
{
    if(cap.grab()) cap.gray(frame);

    //GaussianBlur(frame, frame, Size(5,5), 0);
    threshold(frame, frame, 0, 255, THRESH_BINARY + THRESH_OTSU);

    return !frame.empty();
}


// Inizializa o modo de captura da câmera
void Capturador::open()
{
    cap.open();

    if (!cap.isOpen()) cerr << "Capturador: Falha ao abrir a camera" << endl;
}


// Pausa a captura da câmera
void Capturador::close()
{
    cap.close();
}
