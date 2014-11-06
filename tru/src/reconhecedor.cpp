#include "reconhecedor.h"
using namespace cv;

Reconhecedor::Reconhecedor()
{
    this->n_colunas = 640;
    this->n_linhas = 480;
    this->trilha_min = 40;
    this->trilha_max = 60;
    this->varredura = 100;
    this->inc_linhas = 10;
    this->centro_frame_ant = n_colunas / 2;
}


// Construtor
Reconhecedor::Reconhecedor(int n_colunas, int n_linhas, int trilha_min, int trilha_max, int varredura, int inc_linhas)
{
    this->n_colunas = n_colunas;
    this->n_linhas = n_linhas;
    this->trilha_min = trilha_min;
    this->trilha_max = trilha_max;
    this->varredura = varredura;
    this->inc_linhas = inc_linhas;
    this->centro_frame_ant = n_colunas / 2;
}


// Rotina principal do algoritmo de identificação da trilha
int Reconhecedor::algoritmo(Mat& imagem, int *X, int *Y)
{
    int centro_ant = (*X), soma_linhas = 0, centros_invalido = 0;
    (*Y) = n_linhas;
    (*X) = 0;

    // realiza a varredura de FALHA_COMECO linhas no inicio de cada frame
    for (int i = n_linhas - 1; i > n_linhas - FALHA_FRAME; i--)
    {
        // Realiza a varredura da linha atual
        int centro = lerLinha(imagem.ptr<uchar>(i), centro_frame_ant, true);

        // caso um centro valido tenha sido encontrado na linha
        if (centro >= 0)// && (abs(centro - centro_ant) <= ERRO_CENTRO))
        {
            //trilha.push_back(Point(centro, i)); // Guarda a ponto para depois desenhar

            (*X) += (centro * (n_linhas - 1 - i));
            soma_linhas += (n_linhas - 1 - i);
            (*Y) = i;   // Posição do centro válido
            centro_ant = centro;
            centro_frame_ant = centro;  // Guarda o valor do primeiro centro válido encontrado no frame

            break;  // Sai ao encontrar um centro válido
        }
    }
    // caso nenhum centro valido tenha sido encontrado nas falha_frame linhas a função retorna com -1
    if ((*Y) == n_linhas)
    {
        // o valor de x eh restaurado
        *X = centro_ant;
        return -1;
    }

    // Continua a varredura a partir da linha FALHA_COMECO até a linha mais distante
    for (int i = (*Y) - 1; i >= 0; i -= inc_linhas)
    {
        // Realiza a leitura da linha atual
        int centro = lerLinha(imagem.ptr<uchar>(i), centro_ant, false);

        // caso a linha possua um centro valido
        if (centro >= 0)// && (abs(centro - centro_ant) <= ERRO_CENTRO))  // Contabiliza os centros válidos
        {
            //trilha.push_back(Point(centro, i)); // Guarda a ponto para depois desenhar

            // a variavel de erro eh zerada
            centros_invalido = 0;

            (*X) += (centro * (n_linhas - 1 - i));
            soma_linhas += (n_linhas - 1 - i);
            (*Y) = i;   // Posição do centro válido mais distante
            centro_ant = centro;
        }
        // caso a linha nao represente uma trilha normal, nem cruzamento nem
        // curva de 90 o limite de linhas sem centro valido eh acrescentado
        else
        {
            // a verficação do frame é interrompida caso o numero máximo de linhas falhas seja atingido
            if (++centros_invalido > FALHA_FRAME)
            {
                break;
            }
        }
    }

    if (soma_linhas) (*X) /= soma_linhas;   // Média ponderada dos centros encontrados
    else (*X) = centro_ant;

    return 0;
}


#ifdef BIFURCACAO_ESQUERDA

// Lê a linha atual e retorna o centro válido encontrado
int Reconhecedor::lerLinha(uchar *linha, int centro_anterior, bool comeco)
{
    int inicio, fim, bordas = 0, temp = 0;
    int limite_inf, limite_sup;

    // Limites da varredura para diminuir processamento e verificar apenas os lugares de interesse
    if (comeco == true)
    {
        // Verifica o caso dos quadros anteriores serem uma curva de 90
        if ((centro_anterior == 0) || (centro_anterior == n_colunas - 1))
        {
            centro_anterior = n_colunas / 2;
        }
        limite_inf = centro_anterior - VARREDURA_COMECO;
        limite_sup = centro_anterior + VARREDURA_COMECO;
    }
    else
    {
        limite_inf = centro_anterior - varredura;
        limite_sup = centro_anterior + varredura;
    }
    // Ajusta os limites da imagem
    if (limite_inf < 0) limite_inf = 0;
    if (limite_sup > n_colunas) limite_sup = n_colunas;


    // Realiza a varredura da esquerda para a direita de acordo com os limites anteriores
    inicio = fim = limite_inf;
    for(int i = limite_inf; i < limite_sup; i++)
    {
        temp = bordas % 2;
        if ((linha[i] == FITA) && !temp)
        {
            bordas++;
            inicio = i;
        }
        else if ((linha[i] == FUNDO) && temp)
        {
            bordas++;
            fim = i;
        }
        // caso a trilha esteja encostada no limite
        else if ((linha[i] == FITA) && i == limite_sup - 1)
        {
            fim = i;
        }

        temp = fim - inicio;    // Se for uma trilha válida..
        if (temp > trilha_min && temp < trilha_max)
        {
            int centro = (inicio + fim) / 2;
            if ((abs(centro - centro_anterior) > DELTA) && (comeco == false))
            {
                return -1;
            }
            else
            {
                return centro;    // ..Retorna a posição do centro
            }
        }
        if (temp > trilha_max)
        {
            return -2;  // Indica que deve ser verificado se existe uma curva de 90
        }
    }

    return -1;  // Retorna (-1) caso não tenha encontrado um centro válido
}

#endif


#ifdef BIFURCACAO_DIREITA

// Lê a linha atual e retorna o centro válido encontrado
int Reconhecedor::lerLinha(uchar *linha, int centro_anterior, bool comeco)
{
    int inicio, fim, bordas = 0, temp = 0;
    int limite_inf, limite_sup;

    // Limites da varredura para diminuir processamento e verificar apenas os lugares de interesse
    if (comeco == true)
    {
        // Verifica o caso dos quadros anteriores serem uma curva de 90
        if ((centro_anterior == 0) || (centro_anterior == n_colunas - 1))
        {
            centro_anterior = n_colunas / 2;
        }
        limite_inf = centro_anterior - VARREDURA_COMECO;
        limite_sup = centro_anterior + VARREDURA_COMECO;
    }
    else
    {
        limite_inf = centro_anterior - varredura;
        limite_sup = centro_anterior + varredura;
    }
    // Ajusta os limites da imagem
    if (limite_inf < 0) limite_inf = 0;
    if (limite_sup > n_colunas) limite_sup = n_colunas;


    inicio = fim = limite_sup;
    // Realiza a varredura da esquerda para a direita de acordo com os limites anteriores
    for(int i = limite_sup; i > limite_inf; i--)
    {
        temp = bordas % 2;
        if ((linha[i] == FITA) && !temp)
        {
            bordas++;
            inicio = i;
        }
        else if ((linha[i] == FUNDO) && temp)
        {
            bordas++;
            fim = i;
        }
        // caso a trilha esteja encostada no limite
        else if ((linha[i] == FITA) && i == limite_inf + 1)
        {
            fim = i;
        }

        temp = inicio - fim;    // Se for uma trilha válida..
        if (temp > trilha_min && temp < trilha_max)
        {
            int centro = (inicio + fim) / 2;
            if ((abs(centro - centro_anterior) > DELTA) && (comeco == false))
            {
                return -1;
            }
            else
            {
                return centro;    // ..Retorna a posição do centro
            }
        }
        if (temp > trilha_max)
        {
            return -2;
        }
    }

    return -1;  // Retorna (-1) caso não tenha encontrado um centro válido
}
#endif


bool Reconhecedor::curva_90_direita(Mat &imagem, int *X, int *Y)
{
    int centro_ant = *X, centros_invalidos = 0, centros_validos = 0;
    int limite_inf, limite_sup;

    // Limites da varredura para diminuir processamento e verificar apenas os lugares de interesse
    limite_inf = centro_ant - (varredura - 20);
    if (limite_inf < 0) limite_inf = 0;

    limite_sup = centro_ant + (varredura - 20);
    if (limite_sup > n_linhas) limite_sup = n_linhas;

    // a varredura se inicia a partir do ultimo centro válido
    // mais distante do frame antes do mesmo ser transposto
    for (int k = (*Y + OFFSET_CURVA_90); k < n_colunas; k++)
    {
        int inicio, fim, bordas = 0, temp = 0;
        
        // salva a linha atual do frame

        uchar* linha = imagem.ptr<uchar>(k);
        int centro = -1;
        inicio = fim = limite_sup;

        // Realiza a varredura da direta para a esquerda (imagem transposta) equivalente
        // a de baixo para cima (imagem original), de acordo com os limites anteriores
        for(int i = limite_sup; i > limite_inf; i--)
        {
            temp = bordas % 2;
            if ((linha[i] == FITA) && !temp)
            {
                bordas++;
                inicio = i;
            }
            else if ((linha[i] == FUNDO) && temp)
            {
                bordas++;
                fim = i;
            }
		    // caso a trilha esteja encostada no limite
		    else if ((linha[i] == FITA) && i == limite_inf + 1)
		    {
		        fim = i;
		    }

            temp = inicio - fim;    // Se for uma trilha válida..
            if (temp > (trilha_min - 7) && temp < trilha_max)
            {
                if(++centros_validos == OK_CURVA_90) return true;
                centros_invalidos = 0;
                centro = (inicio + fim) / 2;    // ..Retorna a posição do centro
                centro_ant = centro;
                break;
            }
        }
        // caso nenhum centro valido tenha sido encontrado na linha
        if (centro < 0)
        {
            // a verficacao do frame é interrompida caso o numero máximo de linhas falhas seja atingido
            if (++centros_invalidos > FALHA_FRAME)
            {
                return false;
            }
        }
    }

    return false;
}

