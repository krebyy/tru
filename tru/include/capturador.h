#ifndef CAPTURADOR_H
#define CAPTURADOR_H

#include <iostream>
#include "OCVCapture.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

class Capturador
{
public:
    Capturador();
    Capturador(int n_colunas, int n_linhas);

    bool getFrame(Mat& frame);
    void open();
    void close();

private:
    int n_colunas;
    int n_linhas;
    OCVCapture cap;
};

#endif // CAPTURADOR_H
