#include "POSH.h"
#include <algorithm>
#include <time.h>
#include <ilcplex/ilocplex.h>


typedef IloArray<IloNumVarArray> IloNumVarMatrix;
typedef IloArray<IloNumVarMatrix> IloNumVar3Matrix;
typedef IloArray<IloNumVar3Matrix> IloNumVar4Matrix;

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
    //ler o numero de Hoteis extras
    f >> H;
    //ler o numero total de viagens
    f >> D;
    //ler o tempo total da viagem
    f >> Tmax;
    //ler o tempo de viagem para cada viagem
    Td = vector<double>(H + 1);
    totalH = H+2;
    Si=vector<int>(n);
    double a;
    
    for (int i = 0; i < D; i++)
    {
        f >> a;
        Td[i] = a;
    }

    listaC = vector<Cliente>(n);
   
    Cliente ponto,pontoI,pontoJ;
    
    
    //prencHemos nossos pontos de cliente e Hoteis.
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
    /*for (int i = 0; i < n; i++)
    {
        cout<<endl;
        for (int j = 0; j < n; j++)
        {
            cout<<matrizDist[i][j]<<" ";
            distanciatotal+=matrizDist[i][j];
    
        }
    }
    */
    IloEnv env;
	IloModel modelo;
    // iniciando o modelo
    modelo = IloModel(env);
    // DECLARAÇÃO DE VARIÁVEIS DE DUAS DIMENSÕES-matriz de arestas
    IloNumVar3Matrix x(env,n);
    
    for(int i=0;i<n;i++){
        x[i]=IloNumVarMatrix(env,n);
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            x[i][j]=IloNumVarArray(env,D);
        }
    }
    //DECLARANDO VARIAVEL DE DECISAO UI
    IloNumVarArray U(env,n,1,n,ILOINT);
    for(int i=0;i<n;i++){
        cout<<U[i]<<"TAMANHO DE N:    "<<n<<endl;
       modelo.add(U[i]); 
    }
    
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int d=0;d<D;d++)
                if(i!=j)
                {
                    x[i][j][d]=IloNumVar(env,0,1,ILOBOOL);
                }                   
                else
                {
                    x[i][j][d]=IloNumVar(env,0,0,ILOBOOL);
                }
                
        }
    }
    
    //adicionando a matriz binaria no modelo 
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int d=0;d<D;d++)
                modelo.add(x[i][j][d]);
        }
    }
    //modelando a funcao obj
    IloExpr fo(env);
    for(int d=0;d<D;d++){
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                fo+=Si[i]*x[i][j][d];
                }
            }
    }
    modelo.add(IloMaximize(env, fo));
    //fo.end();

   //restrição 1 (2.2) ////////////////////////////////////////////////////////////////////////////////////////////////
    IloExpr soma(env);
    for(int l=1;l<n;l++){
       if(l>H+2){
        soma+=x[0][l][0];
       }
    }
    IloRange viagemH0(env,1,soma,1);
    modelo.add(viagemH0);
    
    
    //restrição 2 (2.3) ////////////////////////////////////////////////////////////////////////////////////////////////
    IloExpr soma1(env);
    for(int k=0;k<n;k++){
        if(k>H+2){
            soma1+=x[k][1][D-1];      
       }
    }
    IloRange viagemH1(env,1,soma1,1);
    modelo.add(viagemH1);
    
   
    //restricao 3    (2.4) ////////////////////////////////////////////////////////////////////////////////////////////////
   for(int d=0;d<D;d++){
        IloExpr somaH(env);
        for(int h=0;h<H+2;h++){
            for(int l=0;l<n;l++){
                if(l>H+2)
                    somaH+=x[h][l][d];
 
            }
        }
        IloRange HoteisDisponiveis(env,1,somaH,1);    
        modelo.add(HoteisDisponiveis);
    }

    // restrição 4 (2.5) ////////////////////////////////////////////////////////////////////////////////////////////////
    for(int d=0;d<D;d++){
        IloExpr somaH2(env);
        for(int h=0;h<H+2;h++){
            for(int k=0;k<n;k++){
            
                if(k>H+2)
                    somaH2+=x[k][h][d];
 
            }
        }
        IloRange HoteisDisponiveis2(env,1,somaH2,1);    
        modelo.add(HoteisDisponiveis2);
    }
    

 //restricao 5 (2.6) ////////////////////////////////////////////////////////////////////////////////////////////////
   
    for(int h=0;h<H+2;h++){
        for(int d=0;d<(D-1);d++){
             IloExpr somaMesmoHotel1(env);
             IloExpr somaMesmoHotel2(env);
             for(int k=0;k<n;k++){
                if(k>H+2){
                    somaMesmoHotel1+=x[k][h][d];
                }
             }
                for(int l=0;l<n;l++){
                if(l>H+2){
                    somaMesmoHotel2+=x[h][l][d+1];
                }
             }
             IloRange mesmoHotel(env,0,somaMesmoHotel1-somaMesmoHotel2,0);
             modelo.add(mesmoHotel);
        }
    }
    
    //restrição 6 - (2.7) ////////////////////////////////////////////////////////////////////////////////////////////////
   
    for(int d=0;d<D;d++){
        for(int k=H+3;k<n;k++){
             IloExpr somaConectividade1(env);
             IloExpr somaConectividade2(env);
             for(int i=0;i<n;i++){
                if(k>H+2 && i>H+2){
                    somaConectividade1+=x[i][k][d];                    
                }
             }
             for(int j=0;j<n;j++){ 
                if(k>H+2 && j>H+2){                
                    somaConectividade2+=x[k][j][d];
                }
             }
             IloRange conectividadeUnica(env,0,somaConectividade1-somaConectividade2,0);
             modelo.add(conectividadeUnica);
        }
    } 
    //restrição 7 (2.8) ////////////////////////////////////////////////////////////////////////////////////////////////
    
    for(int i=0;i<n;i++){
        IloExpr visitaUnica(env);
        for(int d=0;d<D;d++){
            for(int j=0;j<n;j++){
            
                 if(i>H+2 && j>H+2){
                    visitaUnica+=x[i][j][d];
                    
                 }
            }
        }
        IloRange visitaUnicaa(env,0,visitaUnica,1);
        modelo.add(visitaUnicaa);
        
    }

    //restrição 8 (2.9) ////////////////////////////////////////////////////////////////////////////////////////////////
    for(int d=0;d<D;d++){
        IloExpr rest9(env);
        for(int i=1;i<n;i++){
            for(int j=0;j<n;j++){
                //if(i>H+2 && j> H+2){
                    rest9+=matrizDist[i][j]*x[i][j][d];
              // }
                
            }
        }
        IloRange a(env,0,rest9,Td[d]);
        modelo.add(a);
       
    }
      //restrição 8 (2.9) ////////////////////////////////////////////////////////////////////////////////////////////////
    
   
    for(int i=H+3;i<n;i++){
        for(int j=H+3;j<n;j++){
            IloExpr somaUi(env);
            IloExpr somaUj(env);
            IloExpr somaXijd(env);
            /*for(int u=H+3;u<n;u++){
                somaUi+=U[u];
            }
            for(int u=H+3;u<n;u++){
                somaUj+=U[u];
            }*/
            for(int d=0;d<D;d++){
                somaXijd+=x[i][j][d];
            }
             
            //IloRange posicaoNaSolucao(env,esquerda,direita);
            modelo.add((U[i]-U[j]+1)<=((n-1)*(1-somaXijd)));
        }
    }

   printf("\nCPLEX");
    //modelo.add(rest9<=Tmax);
    // Carregando o módulo do Cplex
    IloCplex cplex(modelo);
    
    
    // exportando o lp
    cplex.exportModel("tcc.lp");
    // Executando o modelo    
    cplex.solve();
    double fit=cplex.getObjValue();
    /*for(int d=0;d<D;d++){
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                int a=cplex.getValue(x[i][j][d]);
                printf("\ni: %d j: %d d: %d -> %d\n",i,j,d,a);
            }
        }
    }*/
    
    
    printf("\n\n fo: %lf\n",fit);
    for(int j=0;j<n;j++){
       int num=cplex.getValue(U[j]);         
    printf("%d  ",num);
    }
    printf("\n");
    f.close();
}

POSH::~POSH()
{
}

