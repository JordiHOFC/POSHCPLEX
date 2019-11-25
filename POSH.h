#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

typedef struct sCliente
{
  float x; //coordenada X
  float y; //coordenada Y
  int Si;  //bonificaçao, quando a bonificação é zero significa que o ponto é um hotel

} Cliente;

class POSH
{
public:
  //ponteiro que fara a leitura do arquivo
  ifstream f;

  //string que recebera o nome do arquivo
  string nomearquivo;

  //construtor
  POSH(string &nomearquivo);
  //destrutor
  virtual ~POSH();

  //numero de vertices
  int n;

  //numero de hoteis extras(fora inicial e final)
  int h;

  //numero de viagens
  int d;

  //tempo total de viagem
  int Tmax;

  //duracao de viagem para cada viagem m
  vector<double> Td;

  //constante de penalizacao

  //matriz de distancia
  vector<vector<double>> matrizDist;
  vector<Cliente>listaC;
 
  //vetor de score por cliente
  vector<int>Si;

  //retornar a matriz de distancias
  //mostra solucao
  
  //funcao que retorna uma solucao gulosa apartir do ponto de melhor custo por beneficio
};