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
#include <math.h>		/* sqrt */

Blase::Blase() {

	srand (time(NULL));  	//initialize random seed:
	r = rand() % 50 + 10; 	//generate  number between 10 and 50
	x = rand() % (600-r) + r; 	//generate number between r and 600 - r
	y = 0;

	vx = 0;
	vy = 1;
}

Blase::Blase( int helpRandom ) {

	srand (time(NULL) * helpRandom );  	//initialize random seed:
	r = rand() % 50 + 10; 	//generate  number between 10 and 50
	x = rand() % (600-r) + r; 	//generate number between r and 600 - r
	y = 0;

	vx = 0;
	vy = 1;
}

Blase::~Blase() {
	// TODO Auto-generated destructor stub
}

void Blase::newStart()
{
	srand (time(NULL) / 2);  	//initialize random seed:
	r = rand() % 50 + 10; 	//generate  number between 10 and 50
	x = rand() % (600-r) + r; 	//generate number between r and 600 - r
	y = 0;

	vx = 0;
	vy = 1;
}

void Blase::updateBlase()
{
	//Berechne Schwerkraft und Traegheit in die Bewegung der Blase mit ein
	vy += 0.3;
	vx *= 0.99;


	x += vx;
	y += vy;
}

void Blase::setMove( int x, int y)
{
	vx = x;
	vy = y;
}

// Gib die momentane Geschwindigkeit also die Laenge des Bewegungsvektors zurueck
double Blase::getAcc()
{
	return sqrt( vx*vx + vy*vy);
}
