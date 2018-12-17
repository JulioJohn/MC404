#include "montador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//***********Aluno: Júlio John Tavares Ramos
//***********Ra: 200481


//***********************HEAD***********************
char *verificarCodigo(int posicao);
int passarParaInteiro(char *palavra);

int emitirMapaDeMemoria()
{
	//******************************************************
	//********************Inicializacoes********************
	//******************************************************
	//posicao atual e lado da instrucao
	int enderecoAtualInt = 0;
	char *enderecoAtual = malloc(6*sizeof(char));
	int lado = 0; //0 eh left e 1 eh right

	//auxiliar
	int b = 0;

	//para .set, adotei que primeiro vem o simbolo e depois vem o valor que ele representa, ex: .set JULIO 10, entao vetorSet[1] == "JULIO" e vetorSet[2] == "10"
	char **vetorSet = malloc(20 * sizeof(char *));
	for(b = 0; b < 20; b++)
	{
		vetorSet[b] = malloc(64 * sizeof(char));
	}
	int tamVetorSet = 0;

	//coluna rotulo
	char **colunaRotulo = malloc(20 * sizeof(char *));
	for(b = 0; b < 20; b++)
	{
		colunaRotulo[b] = malloc(64 * sizeof(char));
	}
	int tamColunaRot = 0;

	//coluna endereço
	char **colunaEndereco = malloc(20 * sizeof(char *));
	for(b = 0; b < 20; b++)
	{
		colunaEndereco[b] = malloc(64 * sizeof(char));
	}

	//coluna posicao do endereco
	int **colunaPosEnd = malloc(20 * sizeof(int *));
	for(b = 0; b < 20; b++)
	{
		colunaPosEnd[b] = malloc(1 * sizeof(int));
	}

	//mapa de memoria
	char **mapaDeMemoria = malloc(1025 * sizeof(char *));
	for(b = 0; b < 1025; b++)
	{
		mapaDeMemoria[b] = malloc(13 * sizeof(char));
	}
	int tamMapaDeMemoria = 0;

	//mapa de memoria final
	char **mapaDeMemoriaFinal = malloc(1025 * sizeof(char *));
	for(b = 0; b < 1025; b++)
	{
		mapaDeMemoriaFinal[b] = malloc(13 * sizeof(char));
	}
	int tammapaDeMemoriaFinal = 0;

	//auxiliar
	char *aux = malloc(11*sizeof(char));

	//endereco das intrucoes do rotulo, exemplo: o endereco em mapaEndereco[0] == "001" e mapaDeMemoria[0] = "0110105102", unido eles das "001 01 101 05 102"
	char **mapaEndereco = malloc(1025 * sizeof(char *));
	for(b = 0; b < 1025; b++)
	{
		mapaEndereco[b] = malloc(4 * sizeof(char));
	}

	char **mapaEnderecoFinal = malloc(1025 * sizeof(char *));
	for(b = 0; b < 1025; b++)
	{
		mapaEnderecoFinal[b] = malloc(4 * sizeof(char));
	}


	//******************************************************
	//******************Passo 1: Montagem*******************
	//******************************************************
    for(int i = 0; i < getNumberOfTokens(); i++)
    {
    	Token tokenAux = recuperaToken(i);
    	Token argUm;
    	Token argDois;

    	if(tokenAux.tipo == Diretiva)
    	{
    		if(!(strcmp(tokenAux.palavra, ".org")))
    		{
    			argUm = recuperaToken(i+1);
    			if(argUm.tipo == Decimal)
    			{
    				enderecoAtualInt = passarParaInteiro(argUm.palavra);
    			}
    			else if(argUm.tipo == Hexadecimal)
    			{
    				enderecoAtualInt = passarParaInteiro(argUm.palavra);
    			}
    		}
    		else if(!(strcmp(tokenAux.palavra, ".align")))
    		{
    			argUm = recuperaToken(i+1);
				if(enderecoAtualInt % passarParaInteiro(argUm.palavra) == 0 && lado == 0)
				{
					//continua na mesma linha, visto que eh par e esta na posicaoEsquerda
				}
				else
				{
					//pula para a proxima linha tal que o resto da divisao pelo arg 1 seja 0
					enderecoAtualInt = enderecoAtualInt + ((passarParaInteiro(argUm.palavra)) - (enderecoAtualInt % passarParaInteiro(argUm.palavra)));
					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
					lado = 0;
				}
    		}
    		else if(!(strcmp(tokenAux.palavra, ".set")))
    		{
    			argUm = recuperaToken(i+1);
    			argDois = recuperaToken(i+2);
    			//simbolo pos i
    			strcpy(vetorSet[tamVetorSet], argUm.palavra);
    			tamVetorSet++;
    			//valor que representa pos i + 1
    			strcpy(vetorSet[tamVetorSet], argDois.palavra);
    			tamVetorSet++;
    		}
    		else if(!(strcmp(tokenAux.palavra, ".word")))
    		{
    			argUm = recuperaToken(i+1);
    			if(lado == 0)
    			{
    				if(argUm.tipo == Hexadecimal || argUm.tipo == Decimal)
    				{
    					//guarda o valor no mapa de memoria
    					sprintf(aux, "%010x", passarParaInteiro(argUm.palavra));
						strcpy(mapaDeMemoria[tamMapaDeMemoria], aux);

						//guarda o endereco no mapa de memoria de endereco
    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
    					strcpy(mapaEndereco[tamMapaDeMemoria], enderecoAtual);

    					//aumenta o endereco atual, visto que .word ocupa a palavra inteira
    					enderecoAtualInt++;
    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
    					tamMapaDeMemoria++;
    					lado = 0;

    				}
    				else
    				{
    					//faco na proxima etapa, nesta nao eh necessario, pois ainda nao temos o vetorSet completo
    					//nessa etapa eh apenas necessario aumentar a linha, visto que pode dar conflito na hora de montar o vetor de defRotulos
    					//guarda o endereco no mapa de memoria de endereco
    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
    					strcpy(mapaEndereco[tamMapaDeMemoria], enderecoAtual);

    					//aumenta o endereco atual, visto que .word ocupa a palavra inteira
    					enderecoAtualInt++;
    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
    					tamMapaDeMemoria++;
    					lado = 0;
    				}
    			}
    			else if(lado == 1) //se estiver na direita
    			{
    				fprintf(stderr, "IMPOSSIVEL MONTAR CODIGO!");
    				return 1;
    			}
    		}
    		else if(!(strcmp(tokenAux.palavra, ".wfill")))
    		{
    			argUm = recuperaToken(i+1);
    			argDois = recuperaToken(i+2);

				//alocar n posicoes de memoria com o seguinte valor
				if(argDois.tipo == Hexadecimal || argDois.tipo == Decimal)
				{
					for(int a = 0; a < passarParaInteiro(argUm.palavra); a++)
					{						
						sprintf(enderecoAtual, "%03x", enderecoAtualInt);
						strcpy(mapaEndereco[tamMapaDeMemoria], enderecoAtual);
						sprintf(mapaDeMemoria[tamMapaDeMemoria], "%010x", passarParaInteiro(argDois.palavra));
						enderecoAtualInt += 1; //incrementa n vezes o valor, sendo n argUm
						tamMapaDeMemoria++;
						lado = 0;
					}
				}
				else if(argDois.tipo == Nome)
				{
					for(int a = 0; a < passarParaInteiro(argUm.palavra); a++)
					{
						for(int k = 0; k < tamVetorSet; k++)
    					{
    						if(!strcmp(vetorSet[k], argUm.palavra))
    						{
    							//nessa primeira etapa, nao precisamos saber o valor que esta em .set, assim vamos deixar "00MONTADO" para ficar mais facil de visualizar, caso imprima essa matriz
    							strcpy(mapaDeMemoria[tamMapaDeMemoria], "00MONTANDO");
								//guarda o endereco no mapa de memoria de endereco
		    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
		    					strcpy(mapaEndereco[tamMapaDeMemoria], enderecoAtual);
								//aumenta o endereco atual, visto que .word ocupa a palavra inteira
		    					enderecoAtualInt++;
		    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
		    					tamMapaDeMemoria++;
		    					lado = 0;
    							break;
    						}
    					}	
					}
				}
    		}
    	}

    	else if(tokenAux.tipo == DefRotulo)
    	{
    		int tamanhoDoToken = strlen(tokenAux.palavra) - 1;
    		//retirar o ":" do rotulo
    		tokenAux.palavra[tamanhoDoToken] = '\0';

    		strcpy(colunaRotulo[tamColunaRot], tokenAux.palavra);

    		sprintf(enderecoAtual, "%03x", enderecoAtualInt);
    		strcpy(colunaEndereco[tamColunaRot], enderecoAtual);

    		colunaPosEnd[tamColunaRot][0] = lado;

    		tamColunaRot++;
    	}

    	else if(tokenAux.tipo == Instrucao)
    	{
    		char* valorInstrucao = verificarCodigo(i);
       		if(!(strcmp(valorInstrucao, "14")) || !(strcmp(valorInstrucao, "15")))
    		{
				sprintf(aux, "%2x", passarParaInteiro(valorInstrucao));
				strcat(mapaDeMemoria[tamMapaDeMemoria], aux);
				strcat(mapaDeMemoria[tamMapaDeMemoria], "000");
				
				if(lado == 0)
				{
					lado = 1;
				}
				else if(lado == 1)
				{
					enderecoAtualInt++;
					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
					lado = 0;
					tamMapaDeMemoria++;
				}
    		}
    		//todas as outras intrucoes
    		else
    		{
    			//Nesse primeiro passo, podemos ignorar esse token apenas no segundo, devemos criar mapa com as devidas posicoes argUm = recuperaToken(i+1);
				sprintf(aux, "%2x", passarParaInteiro(valorInstrucao));
				strcat(mapaDeMemoria[tamMapaDeMemoria], aux);
				strcat(mapaDeMemoria[tamMapaDeMemoria], "000");
				
				if(lado == 0)
				{
					lado = 1;
				}
				else if(lado == 1)
				{
					enderecoAtualInt++;
					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
					lado = 0;
					tamMapaDeMemoria++;
				}
    		}
    	}
    	else
    	{
    		continue;
    	}
    }

    //Resetar os valores para realizar o novo escaneamento dos tokens
    tamMapaDeMemoria = 0;
    enderecoAtualInt = 0;
    lado = 0;
    sprintf(enderecoAtual, "%03x", enderecoAtualInt);

    //******************************************************
	//**************Passo 2: Mapa de Memoria****************
	//******************************************************
    //REPETE-SE A ETAPA UM, POREM COM A MATRIZ ROT/END MONTADA!

    for(int i = 0; i < getNumberOfTokens(); i++)
    {
    	Token tokenAux = recuperaToken(i);
    	Token argUm;
    	Token argDois;
    	if(tokenAux.tipo == Diretiva)
    	{
    		if(!(strcmp(tokenAux.palavra, ".org")))
    		{
    			argUm = recuperaToken(i+1);
    			if(argUm.tipo == Decimal)
    			{
    				enderecoAtualInt = passarParaInteiro(argUm.palavra);
    				sprintf(enderecoAtual, "%03x", enderecoAtualInt);

    			}
    			else if(argUm.tipo == Hexadecimal)
    			{
    				enderecoAtualInt = passarParaInteiro(argUm.palavra);
    				sprintf(enderecoAtual, "%03x", enderecoAtualInt);
    			}
    		}
    		else if(!(strcmp(tokenAux.palavra, ".align")))
    		{
    			argUm = recuperaToken(i+1);

				if(enderecoAtualInt % passarParaInteiro(argUm.palavra) == 0 && lado == 0)
				{
					//continua na mesma linha, visto que eh par e esta na posicaoEsquerda
				}
				else
				{
					//pula para a proxima linha tal que o resto da divisao pelo arg 1 seja 0
					enderecoAtualInt = enderecoAtualInt + ((passarParaInteiro(argUm.palavra)) - (enderecoAtualInt % passarParaInteiro(argUm.palavra)));
					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
					lado = 0;
				}
    		}
    		//Nao necessita do .set, pois na primeira etapa, ja consegue analisar quais sao as variaveis setadas, e guardar elas no vetorSet
    		else if(!(strcmp(tokenAux.palavra, ".word")))
    		{
    			argUm = recuperaToken(i+1);
    			if(lado == 0)
    			{
    				if(argUm.tipo == Hexadecimal || argUm.tipo == Decimal)
    				{
    					//guarda o valor no mapa de memoria
    					sprintf(aux, "%010x", passarParaInteiro(argUm.palavra));
						strcpy(mapaDeMemoriaFinal[tammapaDeMemoriaFinal], aux);

						//guarda o endereco no mapa de memoria de endereco
    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
    					strcpy(mapaEndereco[tammapaDeMemoriaFinal], enderecoAtual);

    					//aumenta o endereco atual, visto que .word ocupa a palavra inteira
    					enderecoAtualInt++;
    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
    					tammapaDeMemoriaFinal++;
    					lado = 0;

    				}
    				else if(argUm.tipo == Nome)
    				{
    					for(int k = 0; k < tamVetorSet; k++)
    					{
    						if(!strcmp(vetorSet[k], argUm.palavra))
    						{
    							sprintf(aux, "%010x", passarParaInteiro(vetorSet[k+1]));
								strcpy(mapaDeMemoriaFinal[tammapaDeMemoriaFinal], aux);
								//guarda o endereco no mapa de memoria de endereco
		    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
		    					strcpy(mapaEndereco[tammapaDeMemoriaFinal], enderecoAtual);
								//aumenta o endereco atual, visto que .word ocupa a palavra inteira
		    					enderecoAtualInt++;
		    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
		    					tammapaDeMemoriaFinal++;
		    					lado = 0;
    							break;
    						}
    					}
    				}
    			}
    			else if(lado == 1) // lado == 1
    			{
    				fprintf(stderr, "IMPOSSIVEL MONTAR CODIGO!");
    				return 1;
    			}
    		}
    		else if(!(strcmp(tokenAux.palavra, ".wfill")))
    		{
    			argUm = recuperaToken(i+1);
    			argDois = recuperaToken(i+2);

				//alocar n posicoes de memoria com o seguinte valor
				if(argDois.tipo == Hexadecimal || argDois.tipo == Decimal)
				{
					for(int a = 0; a < passarParaInteiro(argUm.palavra); a++)
					{
						enderecoAtualInt += 1; //incrementa n vezes o valor, sendo n argUm
						
						sprintf(enderecoAtual, "%03x", enderecoAtualInt);
						sprintf(mapaDeMemoriaFinal[tammapaDeMemoriaFinal], "%010x", passarParaInteiro(argDois.palavra));
						tammapaDeMemoriaFinal++;
						lado = 0;
					}
				}
				else if(argDois.tipo == Nome)
				{
					for(int a = 0; a < passarParaInteiro(argUm.palavra); a++)
					{
						for(int k = 0; k < tamVetorSet; k++)
    					{
    						if(!strcmp(vetorSet[k], argDois.palavra))
    						{
    							//passar para inteiro, depois para hexadecimal com 10 casas, preechendo as nao declaradas com 0 na frente
    							sprintf(mapaDeMemoriaFinal[tammapaDeMemoriaFinal], "%010x", passarParaInteiro(vetorSet[k+1]));
								//guarda o endereco no mapa de memoria de endereco
		    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
		    					strcpy(mapaEndereco[tammapaDeMemoriaFinal], enderecoAtual);
								//aumenta o endereco atual, visto que .word ocupa a palavra inteira
		    					enderecoAtualInt++;
		    					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
		    					tammapaDeMemoriaFinal++;
		    					lado = 0;
    							break;
    						}
    					}	
					}
				}
    		}
    	}
    	//Nesta etapa, a matriz rotulo/endereco/posicao do endereco ja esta montada, basta percorrer e verificar se esse rotulo existe ^^
    	else if(tokenAux.tipo == Instrucao)
    	{
    		char* valorInstrucao = verificarCodigo(i);
       		if(strcmp(valorInstrucao, "14") == 0 || strcmp(valorInstrucao, "15") == 0)
    		{
				//sprintf(aux, "%2x", passarParaInteiro(valorInstrucao));
				strcat(mapaDeMemoriaFinal[tammapaDeMemoriaFinal], valorInstrucao);
				strcat(mapaDeMemoriaFinal[tammapaDeMemoriaFinal], "000");
				
				if(lado == 0)
				{
					lado = 1;
				}
				else if(lado == 1)
				{
					enderecoAtualInt++;
					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
					lado = 0;
					tammapaDeMemoriaFinal++;
				}
    		}
    		//todas as outras intrucoes
    		else
    		{
    			int auxiliar = 0;
    			//int guardarLado = 2;
    			argUm = recuperaToken(i+1);
    			//Nesse primeiro passo, podemos ignorar esse token apenas no segundo, devemos criar mapa com as devidas posicoes argUm = recuperaToken(i+1);
				//sprintf(aux, "%2x", passarParaInteiro(valorInstrucao));
				strcat(mapaDeMemoriaFinal[tammapaDeMemoriaFinal], valorInstrucao);
				if(argUm.tipo == Nome)
				{
					for(int p = 0; p < tamColunaRot; p++)
					{
						if(!(strcmp(colunaRotulo[p], argUm.palavra)))
						{
							strcat(mapaDeMemoriaFinal[tammapaDeMemoriaFinal], colunaEndereco[p]);
							//guardarLado = colunaPosEnd[p][0];
							auxiliar = 1;
							break;
						}
						auxiliar = 0;
					}
					if(auxiliar == 0)
					{
						//emitir erro, pois nao existe nenhum rotulo definido com esse nome
						fprintf(stderr, "ERRO: Usado mas não definido: %s\n", argUm.palavra);
						return 0;
					}
				}
				else if(argUm.tipo == Decimal || argUm.tipo == Hexadecimal)
				{
					sprintf(aux, "%03x", passarParaInteiro(argUm.palavra));
					strcat(mapaDeMemoriaFinal[tammapaDeMemoriaFinal], aux);
				}
				
				if(lado == 0)
				{
					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
					strcpy(mapaEndereco[tammapaDeMemoriaFinal], enderecoAtual);
					lado = 1;
				}
				else if(lado == 1)
				{
					enderecoAtualInt++;
					sprintf(enderecoAtual, "%03x", enderecoAtualInt);
					lado = 0;
					tammapaDeMemoriaFinal++;
				}
    		}
    	}
    	else
    	{
    		continue;
    	}
    }

    //se terminar soh com a instrucao a esquerda, incrementar o tamanho da matriz e preecher com 0;
    if(lado == 1)
    {
    	strcat(mapaDeMemoriaFinal[tammapaDeMemoriaFinal], "00000");
    	tammapaDeMemoriaFinal++;
    }

   /*Caso queira verificar a primeira matriz, rotulo/endereco/posicao
    for(int h = 0; h < tamColunaRot; h++)
    {
    	printf("%s ", colunaRotulo[h]);
    	printf("%s ", colunaEndereco[h]);
    	if(colunaPosEnd[h][0] == 0)
    	{
    		printf("esquerda\n");
    	}
    	else if(colunaPosEnd[h][0] == 1)
    	{
    		printf("direita\n");
    	}
    }
    */

    for(int k = 0; k < tammapaDeMemoriaFinal; k++){
		printf("%s ", mapaEndereco[k]);
		for (int emDois = 0; emDois < 2; emDois++)
		{
			printf("%c", mapaDeMemoriaFinal[k][emDois]);
		}
		printf(" ");
		for(int emTres = 2; emTres < 5; emTres++){
			printf("%c", mapaDeMemoriaFinal[k][emTres]);
		}
		printf(" ");
		for (int emDois = 5; emDois < 7; emDois++)
		{
			printf("%c", mapaDeMemoriaFinal[k][emDois]);
		}
		printf(" ");
		for(int emTres = 7; emTres < 10; emTres++){
			printf("%c", mapaDeMemoriaFinal[k][emTres]);
		}
		printf("\n");
    	}

    return 0;
}

//retorna o codigo da instrucao, se nao achar retorna 0
char *verificarCodigo(int posicao)
{
	char *palavra = recuperaToken(posicao).palavra;
	if(!(strcmp(palavra, "ld"))){
		return "01";
	}
	else if(!(strcmp(palavra, "ldinv"))){
		return "02";
	}
	else if(!(strcmp(palavra, "ldabs"))){
		return "03";
	}
	else if(!(strcmp(palavra, "ldmq"))){
		return "0A";
	}
	else if(!(strcmp(palavra, "ldmqmx"))){
		return "09";
	}
	else if(!(strcmp(palavra, "store"))){
		return "21";
	}
	else if(!(strcmp(palavra, "jump"))){
		return "0D";
	}
	else if(!(strcmp(palavra, "jumpl"))){
		return "0F";
	}
	else if(!(strcmp(palavra, "jumpr"))){
		return "10";
	}
	else if(!(strcmp(palavra, "add"))){
		return "05";
	}
	else if(!(strcmp(palavra, "addabs"))){
		return "07";
	}
	else if(!(strcmp(palavra, "sub"))){
		return "06";
	}
	else if(!(strcmp(palavra, "subabs"))){
		return "08";
	}
	else if(!(strcmp(palavra, "mult"))){
		return "0B";
	}
	else if(!(strcmp(palavra, "div"))){
		return "0C";
	}
	else if(!(strcmp(palavra, "lsh"))){
		return "14";
	}
	else if(!(strcmp(palavra, "rsh"))){
		return "15";
	}
	else if(!(strcmp(palavra, "storal"))){
		return "12";
	}
	else if(!(strcmp(palavra, "storar"))){
		return "13";
	}
	return 0;
}

int passarParaInteiro(char *palavra)
{
	char *lixo;
	long int casoDecimal = strtol(palavra, &lixo, 0);
	return casoDecimal;
}