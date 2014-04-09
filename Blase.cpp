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
	r = rand() % 50 + 10; 	//generate secret number between 10 and 50
	x = rand() % 10 + 600 - r; 	//generate secret number between r and 600 - r
	y = 0;
}

Blase::~Blase() {
	// TODO Auto-generated destructor stub
}

