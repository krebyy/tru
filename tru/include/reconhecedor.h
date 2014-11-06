#ifndef RECONHECEDOR_H
#define RECONHECEDOR_H

#include <unistd.h>
#include "opencv2/core/core.hpp"
using namespace cv;

#define FITA 255        // Cor da fita: 0 - Preta | 255 - Branca
#define FUNDO 0         // Cor do fundo: 0 - Preta | 255 - Branca
#define FALHA_FRAME 5	// Numero maximo de linhas a serem descartadas na leitura do restante de cada frame
#define POS_CURVA_90 30	// PosiÁ„o para habilitar a leitura da curva de 90 graus
#define OFFSET_CURVA_90 30  // Numero de pixels desconsiderados a partir do centro valido anterior ‡ curva de 90
#define OK_CURVA_90 40 // Numero de colunas que devem ser lidas para considerar uma curva de 90
#define DELTA 30    // Numero maximo permitido entre o centro atual e o anterior
#define VARREDURA_COMECO 100 // Expansao dos limites durante a varredura no comeco do frame

//#define BIFURCACAO_ESQUERDA
#define BIFURCACAO_DIREITA

class Reconhecedor
{
public:
    Reconhecedor();
    Reconhecedor(int n_colunas, int n_linhas, int trilha_min, int trilha_max, int varredura, int inc_linhas);

    int algoritmo(Mat&, int *, int *);
    int lerLinha(uchar *, int, bool);
    bool curva_90_direita(Mat&, int *, int *);
    bool curva_90_esquerda(Mat&, int *, int *);
    
    int centro_frame_ant; 	// Primeiro centro v·lido do frame anterior

private:
    int n_colunas;      // Largura da imagem
    int n_linhas;       // Altura da imagem
    int trilha_min;     // Tamanho m√≠nimo da trilha (em pixels)
    int trilha_max;     // Tamanho m√°ximo da trilha (em pixels)
    int varredura;      // Faixa da varredura em rela√ß√£o √† posi√ß√£o do centro anterior (em pixels)
    int inc_linhas;     // 1 - considera todas as linhas; x - considera 1 linha a cada "x"
};

#endif // RECONHECEDOR_H
