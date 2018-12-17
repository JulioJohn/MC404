#include "montador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int retornarToken(char *aux, int tamanho);
int verificarRotulos(char *aux, int tamanho);
int verificarDiretivas(char *palavra);
int verificarInstrucoes(char *palavra);
int verificarHexadecimal(char *palavra, int tamanho);
int verificarDecimal(char *palavra, int tamanho);
int verificarNome(char *palavra, int tamanho);
int verificarTamanhoSet(char *palavra);
int decimal1a1023(char *palavra);
int verificarGramatical();
int verificarROT(char *palavra, char** matrizRotulos, int tam);
Token *criar_token(char *palavra, TipoDoToken tipo, int linha, int tamanho);

int processarEntrada(char* entrada, unsigned tamanho)
{
	int linha = 0;
	int tamAux = 0;
	char *aux = malloc(50*sizeof(char));

    for (int i = 0; i < tamanho; i++)
    {
    	if(entrada[i] == ' ' || entrada[i] == '\0' || entrada[i] == '\n')
    	{
    		//encontrou espaço ou \0 ou \n em seguida
    		if(tamAux == 0)
    		{
    			if(entrada[i] == '\n')
    				linha++;
    			continue;
    		}
    		aux[tamAux] = '\0';

    		int avaliarToken = retornarToken(aux, tamAux);
    		//printf("%d\n", avaliarToken);
    		//diretiva
    		if(avaliarToken == 1)
    		{
    			adicionarToken(*criar_token(aux, Diretiva, linha+1, tamAux));
    		}
    		//rotulo
    		else if(avaliarToken == 2)
    		{
    			adicionarToken(*criar_token(aux, DefRotulo, linha+1, tamAux));
    		}
    		//instrucao
    		else if(avaliarToken == 3)
    		{
    			adicionarToken(*criar_token(aux, Instrucao, linha+1, tamAux));
    		}
    		//hexadecimal
    		else if(avaliarToken == 4)
    		{
    			adicionarToken(*criar_token(aux, Hexadecimal, linha+1, tamAux));
    		}
    		//decimal
    		else if(avaliarToken == 5)
    		{
    			adicionarToken(*criar_token(aux, Decimal, linha+1, tamAux));
    		}
    		//nome
    		else if(avaliarToken == 6)
    		{
    			adicionarToken(*criar_token(aux, Nome, linha+1, tamAux));
    		}
    		//erro lexico
    		else if(avaliarToken == 10)
    		{
    			fprintf(stderr, "ERRO LEXICO: palavra inválida na linha %d!\n", linha+1);
    			return 1;
    		}

    		if(entrada[i] == '\n' && tamAux != 0)
    			linha++;
    		//ao final reseta o auxiliar
    		tamAux = 0;
    		continue;
    	}
    	//se encontrar comentario
    	else if(entrada[i] == '#')
    	{
    		//percorre ate o \n afim de eliminar o comentario
    		for(int tamComentario = 0; tamComentario < tamanho - i; tamComentario++)
    		{
    			// usar printf("%c", entrada[i+tamComentario]); para imprimir o comentario
    			if(entrada[i + tamComentario] == '\n')
    			{
    				i = i + tamComentario;
    				//como pula para depois do \n, adicionamos a linha e zeramos o aux daquela linha
    				linha++;
    				tamAux = 0;
    				break;
    			}
    		}
    	}
    	//armazena os pseudo tokens em aux, se cair no primeiro if testa estes
    	else
    	{
    		if(entrada[i] != '\n')
    		{
    			aux[tamAux] = tolower(entrada[i]);
    			tamAux++;
    		}
    	}
    }

    int resultado = verificarGramatical();

    if(resultado != 0)
    {
    	fprintf(stderr, "ERRO GRAMATICAL: palavra na linha %d!\n", resultado);
    	return 1;
    }

    return 0;
}

int retornarToken(char *aux, int tamanho)
{
	//caso em que eh diretiva
	if(verificarDiretivas(aux) == 1)
	{
		return 1;
	}
	if(verificarDiretivas(aux) == 10)
	{
		return 10;
	}
	//caso em que eh um rotulo
	else if(verificarRotulos(aux, tamanho) == 1)
	{
		return 2;
	}
	else if(verificarRotulos(aux, tamanho) == 10)
	{
		return 10; //ERRO LEXICO
	}
	//caso em que eh instrucao
	else if(verificarInstrucoes(aux))
	{
		return 3;
	}
	else if(verificarHexadecimal(aux, tamanho) == 1)
	{
		return 4;
	}
	else if(verificarHexadecimal(aux, tamanho) == 10)
	{
		return 10; //ERRO LEXICO
	}
	else if(verificarDecimal(aux, tamanho))
	{
		return 5;
	}
	else if(verificarNome(aux, tamanho) == 1)
	{
		return 6;
	}
	else if(verificarNome(aux, tamanho) == 10)
	{
		return 10;
	}
	return 0;
}

int verificarRotulos(char *aux, int tamanho)
{
	for (int i = 0; i < tamanho; i++)
	{
		if(aux[i] == ':')
		{
			int teste = i + 1;
			if((teste <= tamanho))
			{
				//caso em que tem : no meio da frase, verifica se tem coisa apos o :
				if(aux[teste] != '\0')
				{
					return 10; //erro lexico
				}
				//caso em que verifica se comeca com numero
				else if(aux[0] >= '0' && aux[0] <= '9')
				{
					return 10; //erro lexico
				}
				//verificar o caso para alfanumericos
				for(int j = 0; j <= i; j++)
				{
					if(aux[j] == '_')
					{
						continue;
					}
					if((aux[j] >= '!' && aux[j] <= '/') || (aux[j] >= ';' && aux[j] <= '@') || (aux[j] >= '[' && aux[j] <= '`') || (aux[j] >= '{' && aux[j] <= '~')) //alguns eu n consegui achar, nao esta completa a tabela de intervalos **
					{
						return 10;
					}
				}
			}
			return 1;
		}
	}
	return 0;
}

int verificarDiretivas(char *palavra)
{
	if(!(strcmp(palavra, "."))){ //ERRO LEXICO
		return 10;
	}
	if(!(strcmp(palavra, ".org"))){
  		return 1;
	}
  	else if(!(strcmp(palavra, ".word"))){
  		return 1;
  	}
  	else if(!(strcmp(palavra, ".align"))){
  		return 1;
  	}
  	else if(!(strcmp(palavra, ".wfill"))){
  		return 1;
  	}
  	else if(!(strcmp(palavra, ".skip"))){
  		return 1;
  	}
  	else if(!(strcmp(palavra, ".set"))){
  		return 1;
  	}
  	return 0;
}

int verificarInstrucoes(char *palavra)
{
	if(!(strcmp(palavra, "ld"))){
		return 1;
	}
	else if(!(strcmp(palavra, "ldinv"))){
		return 1;
	}
	else if(!(strcmp(palavra, "ldabs"))){
		return 1;
	}
	else if(!(strcmp(palavra, "ldmq"))){
		return 1;
	}
	else if(!(strcmp(palavra, "ldmqmx"))){
		return 1;
	}
	else if(!(strcmp(palavra, "store"))){
		return 1;
	}
	else if(!(strcmp(palavra, "jump"))){
		return 1;
	}
	else if(!(strcmp(palavra, "jumpl"))){
		return 1;
	}
	else if(!(strcmp(palavra, "jumpr"))){
		return 1;
	}
	else if(!(strcmp(palavra, "add"))){
		return 1;
	}
	else if(!(strcmp(palavra, "addabs"))){
		return 1;
	}
	else if(!(strcmp(palavra, "sub"))){
		return 1;
	}
	else if(!(strcmp(palavra, "subabs"))){
		return 1;
	}
	else if(!(strcmp(palavra, "mult"))){
		return 1;
	}
	else if(!(strcmp(palavra, "div"))){
		return 1;
	}
	else if(!(strcmp(palavra, "lsh"))){
		return 1;
	}
	else if(!(strcmp(palavra, "rsh"))){
		return 1;
	}
	else if(!(strcmp(palavra, "storal"))){
		return 1;
	}
	else if(!(strcmp(palavra, "storar"))){
		return 1;
	}
	return 0;
}

int verificarHexadecimal(char *palavra, int tamanho)
{
	if(palavra[0] == '0' && palavra[1] == 'x')
	{
		if(tamanho <= 2)
		{
			return 10; //erro lexico, o hexadecimal nao pode ser apenas 0x
		}
		for(int i = 2; i < tamanho ; i++)
		{
			if((palavra[i] >='0' && palavra[i]<='9') || (palavra[i]>='a' && palavra[i]<='f')){
				//printf("%c", palavra[i]);
				continue;
			}
			else{
				return 10; //erro lexico
			}
		}
		return 1;
	}
	return 0;
}

int verificarDecimal(char *palavra, int tamanho)
{
	for(int i = 0; i < tamanho; i++)
	{
		//se todos os caracteres forem 'decimais' logo pode ser decimal, se nao, ja eh erro lexico, visto que eh o ultimo caso
		if(palavra[i] >= '0' && palavra[i] <= '9')
		{
			continue;
		}
		return 0;
	}
	return 1;
}

int verificarNome(char *palavra, int tamanho)
{
	if((palavra[0] >= 'a' && palavra[0] <= 'z') || palavra[0] == '_')
	{
		for(int i = 0; i < tamanho; i++)
		{
			if((palavra[i] >= 'a' && palavra[i] <= 'z') || palavra[i] == '_' || (palavra[i] >= '0' && palavra[i] <= '9'))
			{
				continue;
			}
			else
			{
				return 10; //erro lexico
			}
		}
		return 1;
	}
	else
	{
		return 10; //erro lexico, ou seja, pegou numeros ou simbolos como primeiro caracter, nao pode!
	}
}

Token *criar_token(char *palavra, TipoDoToken tipo, int linha, int tamanho)
{
  Token *tokenAuxiliar = malloc(sizeof(Token));
  tokenAuxiliar->palavra = malloc(tamanho*sizeof(char));
  tokenAuxiliar->linha = linha;
  tokenAuxiliar->tipo = tipo;
  strcpy(tokenAuxiliar->palavra, palavra);
  return tokenAuxiliar;
}

int verificarTamanhoSet(char *palavra)
{
	char *lixo;
	long int casoDecimal = strtol(palavra, &lixo, 10);
	if(casoDecimal >= 0 && casoDecimal <= 2147483647)
	{
		return 1;
	}
	return 0;
}

int decimal1a1023(char *palavra)
{
	char *lixo;
	long int casoDecimal = strtol(palavra, &lixo, 10);
	if(casoDecimal >= 1 && casoDecimal <= 1023)
	{
		return 1;
	}
	return 0;
}

int verificarTamanhoWfill(char *palavra)
{
	char *lixo;
	long int casoDecimal = strtol(palavra, &lixo, 10);
	if(casoDecimal >= -2147483648 && casoDecimal <= 2147483647)
	{
		return 1;
	}
	return 0;
}

int verificarROT(char *palavra, char** matrizRotulos, int tam)
{
	for(int i = 0; i < tam; i++)
	{
		if(!strcmp(matrizRotulos[i], palavra))
		{
			return 1;
		}
	}
	return 0;
}

//retorna a linha do erro caso tenha erro, caso nao tenha erro retorna 0
int verificarGramatical()
{
	//vamos armazenar nosso rotulos criados nessa matriz
	char **rotulosDeclarados = malloc(100*sizeof(char*));
	for(int k = 0; k < 100; k++){
		rotulosDeclarados[k] = malloc(100*sizeof(char));
	}
	int qtdRotulosDeclarados = 0;

	int posicaoLinha = 1;

	Token tokenAux = recuperaToken(0);
	int linhaAux = tokenAux.linha;

	//caso de rotulo
	for(int i = 1; i < getNumberOfTokens(); i++)
	{
		tokenAux = recuperaToken(i);

		//Sempre pega o proximo ao primeiro da linha
		if(linhaAux == tokenAux.linha)
		{
			posicaoLinha++;
			//se esse token for rotulo, erro, pois rotulo so pode estar no inicio da frase e nao pode haver mais de um
			if(tokenAux.tipo == DefRotulo)
			{
				//vai guardar todos os possiveis rotulos
				strcpy(rotulosDeclarados[qtdRotulosDeclarados], tokenAux.palavra);
				qtdRotulosDeclarados++;
				return linhaAux;
			}
		}
		else
		{
			posicaoLinha = 1;
		}
	}

	//caso de diretivas && intrucoes no final
	for(int i = 0; i < getNumberOfTokens(); i++)
	{
		tokenAux = recuperaToken(i);
		//printf("%s\n", tokenAux.palavra);

		//caso .set
		if(!(strcmp(tokenAux.palavra, ".set")))
		{
			//maximo dois argumentos, se nao eh erro lexico
			if(i+3 < getNumberOfTokens()){
				if(tokenAux.linha == recuperaToken(i+3).linha)
				{
					return tokenAux.linha;
				}
			}

			//quando tem-se dois argumentos obrigatoriamente
			//atual eh i, i + 1 eh o argumento 1, i + 2 eh o argumento 2
			if(i+1 < getNumberOfTokens() && i+2 < getNumberOfTokens())
			{
				Token argUm = recuperaToken(i+1);
				Token argDois = recuperaToken(i+2);
				//se todos esses argumentos estiverem na mesma linha
				if(tokenAux.linha == argUm.linha && tokenAux.linha == argDois.linha)
				{
					if(verificarNome(argUm.palavra, strlen(argUm.palavra)) == 1 && (verificarHexadecimal(argDois.palavra, strlen(argDois.palavra)) == 1 || (verificarDecimal(argDois.palavra, strlen(argDois.palavra)) == 1 && verificarTamanhoSet(argDois.palavra) == 1)))
					{
						continue;
					}
					//se nao cumprir alguma dessas condicoes eh erro gramatical
					else
					{
						return tokenAux.linha;
					}
				}
				else
				{
					return tokenAux.linha;
				}
			}
			//se for um .set e nao tiver mais dois argumentos obrigatorios, logo eh um erro gramatical
			else
			{
				return tokenAux.linha; 
			}
		}
		//caso do .org
		else if(!(strcmp(tokenAux.palavra, ".org")))
		{
			//necessita de 1 argumento
			if(i+1 < getNumberOfTokens())
			{
				//maximo um argumento, se nao eh erro lexico
				if(tokenAux.linha == recuperaToken(i+2).linha)
				{
					return tokenAux.linha;
				}

				Token argUm = recuperaToken(i+1);
				if(tokenAux.linha == argUm.linha)
				{
					if(verificarHexadecimal(argUm.palavra, strlen(argUm.palavra)) == 1 || (verificarDecimal(argUm.palavra, strlen(argUm.palavra)) && verificarTamanhoSet(argUm.palavra)))
					{
						continue;
					}
					else
					{
						return tokenAux.linha;
					}
				}
				else
				{
					return tokenAux.linha;
				}
			}
			else
			{
				return tokenAux.linha;
			}
		}
		//caso .align
		else if (!(strcmp(tokenAux.palavra, ".align")))
		{
			//necessita de 1 argumento
			if(i+1 < getNumberOfTokens())
			{
				//maximo um argumento, se nao eh erro lexico
				if(i+2 < getNumberOfTokens()){
					if(tokenAux.linha == recuperaToken(i+2).linha)
					{
						return tokenAux.linha;
					}
				}

				Token argUm = recuperaToken(i+1);
				if(tokenAux.linha == argUm.linha)
				{
					if(decimal1a1023(argUm.palavra))
					{
						continue;
					}
					else
					{
						return tokenAux.linha;
					}
				}
				else
				{
					return tokenAux.linha;
				}
			}
			else
			{
				return tokenAux.linha;
			}
		}
		//caso .wfill
		else if (!(strcmp(tokenAux.palavra, ".wfill")))
		{
			//quando tem-se dois argumentos obrigatoriamente
			//atual eh i, i + 1 eh o argumento 1, i + 2 eh o argumento 2
			if(i+1 < getNumberOfTokens() && i+2 < getNumberOfTokens())
			{
				//maximo dois argumentos, se nao eh erro lexico
				if(i+3 < getNumberOfTokens()){
					if(tokenAux.linha == recuperaToken(i+3).linha)
					{
						return tokenAux.linha;
					}
				}

				Token argUm = recuperaToken(i+1);
				Token argDois = recuperaToken(i+2);
				//se todos esses argumentos estiverem na mesma linha
				if(tokenAux.linha == argUm.linha && tokenAux.linha == argDois.linha)
				{
					if(decimal1a1023(argUm.palavra) == 1 && (verificarHexadecimal(argDois.palavra, strlen(argDois.palavra)) == 1 || (verificarDecimal(argDois.palavra, strlen(argDois.palavra)) == 1 && verificarTamanhoWfill(argDois.palavra) == 1) || verificarROT(argDois.palavra, rotulosDeclarados, qtdRotulosDeclarados) == 1 || verificarNome(argDois.palavra, strlen(argDois.palavra)) == 1))
					{
						continue;
					}
					//se nao cumprir alguma dessas condicoes eh erro gramatical
					else
					{
						return tokenAux.linha;
					}
				}
				else
				{
					return tokenAux.linha;
				}
			}
			//se for um .set e nao tiver mais dois argumentos obrigatorios, logo eh um erro gramatical
			else
			{
				return tokenAux.linha; 
			}
		}
		//caso .word
		else if (!(strcmp(tokenAux.palavra, ".word")))
		{
			//necessita de 1 argumento
			if(i + 1 < getNumberOfTokens())
			{
				////maximo um argumento, se nao eh erro lexico
				if(tokenAux.linha == recuperaToken(i+2).linha)
				{
					return tokenAux.linha;
				}


				Token argUm = recuperaToken(i+1);
				if(tokenAux.linha == argUm.linha)
				{
					if(verificarHexadecimal(argUm.palavra, strlen(argUm.palavra)) == 1 || (verificarDecimal(argUm.palavra, strlen(argUm.palavra)) == 1 && verificarTamanhoWfill(argUm.palavra) == 1) || verificarROT(argUm.palavra, rotulosDeclarados, qtdRotulosDeclarados) == 1 || verificarNome(argUm.palavra, strlen(argUm.palavra)) == 1)
					{
						continue;
					}
					else
					{
						return tokenAux.linha;
					}
				}
				else
				{
					return tokenAux.linha;
				}
			}
			else
			{
				return tokenAux.linha;
			}
		}
		else if(!(strcmp(tokenAux.palavra, "lsh")) || !(strcmp(tokenAux.palavra, "rsh")))
		{
			if(i+1 < getNumberOfTokens()){
				//nao tem argumentos para essas duas instrucoes
				if(tokenAux.linha == recuperaToken(i+1).linha)
				{
					return tokenAux.linha;
				}
			}
		}
		else if(tokenAux.tipo == Nome)
		{
			if(i - 1 >= 0 && i + 1 <= getNumberOfTokens())
			{
				Token tokenAnterior = recuperaToken(i-1);
				Token tokenProximo = recuperaToken(i+1);

				//quer dizer que o proximo eh o primeiro de sua linha, se ele for um nome ele nao pode ser o primeiro da linha, assim:
				if(tokenAnterior.linha != tokenAux.linha && tokenAux.linha == tokenProximo.linha)
				{
					return tokenAux.linha;
				}
			}
		}
	}

	return 0;
}
