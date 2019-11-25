#include "POSH.h"
#include <algorithm>
#include <time.h>

POSH::POSH(string &nomeArq)
{
    // printf("aq");
    //abrimos o arquivo
    f = ifstream(nomeArq);
    //verificamos se foi aberto
    if (!f.is_open())
        cout << "Erro ao abrir o arquivo" << endl;
    //ler o numero de vertices
    f >> n;
    n++; //
    //ler o numero de hoteis extras
    f >> h;
    //ler o numero total de viagens
    f >> d;
    //ler o tempo total da viagem
    f >> Tmax;
    //ler o tempo de viagem para cada viagem
    Td = vector<double>(h + 1);

    Si=vector<int>(n);
    double a;
    
    for (int i = 0; i < d; i++)
    {
        f >> a;
        Td[i] = a;
    }

    listaC = vector<Cliente>(n);
   
    Cliente ponto,pontoI,pontoJ;
    
    
    //prenchemos nossos pontos de cliente e hoteis.
    for(int i = 0; i < n; i++)
    {
        f >> ponto.x;
        f >> ponto.y;
        f >> Si[i];

        listaC[i] = ponto;
        
        
    }
    matrizDist = vector<vector<double>>(n, vector<double>(n));
    double distanciatotal;
    //vamos agora calcular a matriz de distancias

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            pontoI = listaC[i];

            pontoJ = listaC[j];

            matrizDist[i][j] = sqrt((pow((pontoJ.x - pontoI.x), 2)) + (pow((pontoJ.y - pontoI.y), 2)));
            
        }
    }
    for (int i = 0; i < n; i++)
    {
        cout<<endl;
        for (int j = 0; j < n; j++)
        {
            cout<<matrizDist[i][j]<<" ";
            distanciatotal+=matrizDist[i][j];
    
        }
    }
    cout<<"distancia total: "<<distanciatotal<<endl;
    
    f.close();
}

POSH::~POSH()
{
}

