#include "api_robot.h"

motor_cfg_t mleft;
motor_cfg_t mright;

int _start()
{
	mleft.id = 0;
	mright.id = 1;

	mleft.speed = 10;
	mright.speed = 10;

	set_motor_speed(&mleft);
	set_motor_speed(&mright);

	while(1)
	{
		
	}

	return 0;
}