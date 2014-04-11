/*
 * blase.cpp
 *
 *  Created on: 03.04.2014
 *      Author: matthias
 */

#include "Blase.h"

//#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

Blase::Blase() {

	srand (time(NULL));  	//initialize random seed:
	r = rand() % 50 + 10; 	//generate  number between 10 and 50
	x = rand() % (600-r) + r; 	//generate number between r and 600 - r
	y = 0;

	vx = 0;
	vy = 3;
}

Blase::~Blase() {
	// TODO Auto-generated destructor stub
}

void Blase::updateBlase()
{
	x += vx;
	y += vy;
}

void Blase::setMove( int x, int y)
{
	vx = x;
	vy = y;
}
