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
    //ler o numero de hoteis extras
    f >> h;
    //ler o numero total de viagens
    f >> D;
    //ler o tempo total da viagem
    f >> Tmax;
    //ler o tempo de viagem para cada viagem
    Td = vector<double>(h + 1);
    totalH = h+2;
    Si=vector<int>(n);
    double a;
    
    for (int i = 0; i < D; i++)
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
    
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int d=0;d<D;d++)
                x[i][j][d]=IloNumVar(env,0,1,ILOBOOL);
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
    
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int d=0;d<D;d++){
                fo+=Si[i]*x[i][j][d];
            }
        }
    }
    modelo.add(IloMaximize(env, fo));
    //fo.end();

   //restrição 1
    IloExpr soma(env);
    for(int i=0;i<n;i++){
       if(i>2){
        soma+=x[0][i][0];
       }
    }
    IloRange viagemH0(env,1,soma,1);
    modelo.add(viagemH0);
    
    
    //restrição 2
    IloExpr soma1(env);
    for(int i=0;i<n;i++){
        soma1+=x[i][1][D-1];
       
      
    }
    IloRange viagemH1(env,1,soma1,1);
    modelo.add(viagemH1);
    //restricao 3
    IloExpr somaH(env);
     
    for(int d=0;d<D;d++){
        for(int i=0;i<h+2;i++){
            for(int j=0;j<n;j++){
                somaH+=x[i][j][d];
 
            }
        }
        IloRange hoteisDisponiveis(env,1,somaH,1);    
        modelo.add(hoteisDisponiveis);
    }


    // restrição 4
    IloExpr somaH2(env);

    for(int d=0;d<D;d++){
        for(int i=0;i<n;i++){
            for(int j=0;j<h+2;j++){
                somaH2+=x[i][j][d];
 
            }
        }
    IloRange hoteisDisponiveis2(env,1,somaH2,1);    
    modelo.add(hoteisDisponiveis2);
    }
    //IloRange hoteisDisponiveis2(env,1,somaH2,1);    
    //modelo.add(hoteisDisponiveis2);
    printf("\n\nAqui kkkk");
    

   /* //restricao 4 
    //int jj = 3;
   for(int i=0;i<n;i++){
        for(int j = 0 ;j<n;j++){           
            for(int d=0;d<D;d++){                
               
               if(i>=0 && i<totalH && j>=totalH)
                     modelo.add(x[i][j][d]==1);
                                 
             }          
            
        }
    }
  // restricao 5
  
    for(int j=0;j<n;j++){
        for(int i=0;i<2;i++){
            
                for(int d=0;d<D;d++){
                    if(j>2)
                        modelo.add(x[j][i][d]==1);                  
                
                }

            
            
        }
    }





     //restrição 6
    //IloExpr rest6(env);
    int ii = 2, j = 2;
    for(ii;ii<n;ii++){//k ou l
        for(j;j<n;j++){
            for(int d=0;d<D;d++){
                    if(d>1)
                    modelo.add(x[ii][j][d]==x[j][ii][d-1]);
            }
        }
    }

     //restrição 7
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){if
            for(int d=0;d<D;d++){
                if(i>=h+2 || j>=h+2)
                    modelo.add(x[i][j][d]==x[j][i][d]);
            }
        }
    }
   
    //restrição 8
    IloExpr rest8(env);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int d=0;d<D;d++){
                if(i>=h+2 || j>=h+2 )
                {
                    modelo.add(x[i][j][d]=1);
                    
                   
                }
            }
        }   
    }
*/
    //restrição 9
    IloExpr rest9(env);
    for(int d=0;d<D;d++){
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                
                    if(i>=1){
                        rest9+=matrizDist[i][j]*x[i][j][d];
                    //    modelo.add(rest9<=Td[d]);
                         
                    }
                }
            }
        IloRange a(env,0,rest9,Td[d]);
        modelo.add(a);
    }
   printf("\nCPLEX");
    //modelo.add(rest9<=Tmax);
    // Carregando o módulo do Cplex
    IloCplex cplex(modelo);
    // exportando o lp
    //cplex.exportModel("tcc.lp");
    // Executando o modelo    
    cplex.solve();
    double fit=cplex.getObjValue();
    printf("\n\n fo: %lf",fit);

    f.close();
}

POSH::~POSH()
{
}

/*void POSH::modeloMatematico(){
      // DECLARANDO O AMBIENTE E O MODELO MATEMATICO
   
}*/