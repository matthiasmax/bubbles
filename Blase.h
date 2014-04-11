/*
 * blase.h
 *
 *  Created on: 03.04.2014
 *      Author: matthias
 */

#ifndef BLASE_H_
#define BLASE_H_

class Blase {
public:
	Blase();
	virtual ~Blase();

	void updateBlase();
	void setMove( int x, int y);

	int r;	//radius
	int x;	//X Koordinate
	int y;	//Y Koordinate

	int vx;	// Bewegungsrichtung der x Koordinate mit Geschwindigkeit
	int vy;  // Bewegungsrichtung der y Koordinate mit Geschwindigkeit


};

#endif /* BLASE_H_ */
