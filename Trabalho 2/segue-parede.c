#include "api_robot.h"
#include stdio.h
#include stdlib.h

#define VELOCIDADE 20; 
#define DESLIGAR_MOTOR 0;
#define LIMIAR_PAREDE 50;

motor_cfg_t mleft;
motor_cfg_t mright;

//void busca_parede();
//void segue_parede();

int _start()
{
	mleft.id = 0;
	mright.id = 1;

	mleft.speed = 0;
	mright.speed = 0;

	SET_MOTOR_SPEED(&mleft);
	SET_MOTOR_SPEED(&mright);

	//busca_parede()

	while(1)
	{
		
	}

	return 0;
}

/*

void busca_parede()
{
	while(0)
	{
		//andar em linha reta
		mleft.speed = VELOCIDADE;
		mright.speed = VELOCIDADE;
		SET_MOTOR_SPEED(&mleft);
		SET_MOTOR_SPEED(&mright);
		int limiar_frente = READ_SONAR(4);

		//chegou no limiar
		if(READ_SONAR(4) <= LIMIAR_PAREDE)
		{
			//rotacionar para ficar a direita
			mleft.speed = VELOCIDADE;
			mright.speed = DESLIGAR_MOTOR;
			SET_MOTOR_SPEED(&mleft);
			SET_MOTOR_SPEED(&mright);

			if(READ_SONAR(7) == limiar_frente) //direita ter a mesma distancia que a frente, ou seja, ficar com a direita do carrinho para a parede
			{
				segue_parede(mleft, mright);
			}
		}
	}
}

void segue_parede()
{
	while(0)
	{
		//andar para frente
		mleft.speed = VELOCIDADE;
		mright.speed = VELOCIDADE;
		SET_MOTOR_SPEED(&mleft);
		SET_MOTOR_SPEED(&mright);

		//Caso que em que a distancia é menor que o limiar:
		//para corrigir isso, giramos para a esquerda de tal forma que o carrinho fique de costas para a parede
		//andamos para frente até o sensor das costas ser igual ao LIMIAR
		//reajustamos o carrinho com o lado direito para a parede
		if(READ_SONAR(7) <= LIMIAR_PAREDE)
		{
			int distanciaDireita = READ_SONAR(7);
			//girar para a esquerda, até a distancia do sonar de traz ser igual a distancia do sonar da direita do inicio
			while(READ_SONAR(11) != distanciaDireita)
			{
				mleft.speed = VELOCIDADE;
				mright.speed = DESLIGAR_MOTOR;
				SET_MOTOR_SPEED(&mleft);
				SET_MOTOR_SPEED(&mright);
			}
			//andar para frente ate encontrar o limiar
			while(READ_SONAR(11) != LIMIAR_PAREDE)
			{
				mleft.speed = VELOCIDADE;
				mright.speed = VELOCIDADE;
				SET_MOTOR_SPEED(&mleft);
				SET_MOTOR_SPEED(&mright);
			}
			//quando encontrar, girar o carrinho para a direita, fazendo o ajuste de tal forma que a distancia do sonar da direita seja igual ao limiar, voltando a acompanhar a parede
			while(READ_SONAR(7) != LIMIAR_PAREDE)
			{
				mleft.speed = DESLIGAR_MOTOR;
				mright.speed = VELOCIDADE;
				SET_MOTOR_SPEED(&mleft);
				SET_MOTOR_SPEED(&mright);
			}
		}
		//Caso que em que aumenta a distancia até a parede:
		//para corrigir isso, giramos para a direita, de tal forma que o carrinho fique de frente para a parede
		//andamos para frente até o LIMIAR
		//reajustamos o carrinho de lado para a parede
		else
		{
			int distanciaDireita = READ_SONAR(7);
			//girar para a direita até a frente ficar virada para a parede
			while(READ_SONAR(4) != distanciaDireita)
			{
				mleft.speed = DESLIGAR_MOTOR;
				mright.speed = VELOCIDADE;
				SET_MOTOR_SPEED(&mleft);
				SET_MOTOR_SPEED(&mright);
			}
			//andar para frente ate encontrar o limiar
			while(READ_SONAR(4) != LIMIAR_PAREDE)
			{
				mleft.speed = VELOCIDADE;
				mright.speed = VELOCIDADE;
				SET_MOTOR_SPEED(&mleft);
				SET_MOTOR_SPEED(&mright);
			}
			//girar para a esquerda até ficar de lado para a parede
			if(READ_SONAR(4) <= LIMIAR_PAREDE)
			{
				//rotacionar para ficar a direita
				mleft.speed = VELOCIDADE;
				mright.speed = DESLIGAR_MOTOR;
				SET_MOTOR_SPEED(&mleft);
				SET_MOTOR_SPEED(&mright);
			}
		}
	}
}
*/