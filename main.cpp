/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include "SceneDrawer.h"
#include <XnPropNames.h>
#include <string>   // Fuer Stringmanipulationen (nur fuer Dateipfad)
#include <unistd.h> // zum Bestimmen des Absoluten Dateipfads
#include "tga.h"    //zum laden der Texturen
#include "Blase.h"  //definiert die Blasen
#include <vector>   // Fuer die Klasse std::vector
#include <math.h>   //Fuer die Methode sqrt

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
xn::Context g_Context;
xn::ScriptNode g_scriptNode;
xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;
xn::Player g_Player;

XnBool g_bNeedPose = FALSE;
XnChar g_strPose[20] = "";
XnBool g_bDrawBackground = TRUE;
XnBool g_bDrawPixels = TRUE;
XnBool g_bDrawSkeleton = TRUE;
XnBool g_bPrintID = TRUE;
XnBool g_bPrintState = TRUE;

XnBool g_bGame = FALSE; //Status des Spiels gestartet/nicht gestartet
XnBool g_bHelp = TRUE;  //Status der Hilfe angezeigt/nicht angezeigt
XnBool g_bBlase = TRUE; //Auswahl der Blasentextur Seife/Alternative

#ifndef USE_GLES
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif
#else
	#include "opengles.h"
#endif

#ifdef USE_GLES
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;
#endif

#define GL_WIN_SIZE_X 720	//Standardwert: 720
#define GL_WIN_SIZE_Y 480   //Standardwert: 480

XnBool g_bPause = false;
XnBool g_bRecord = false;

XnBool g_bQuit = false;

std::vector<Blase> bla;	// enthaelt die einzelnen Instanzen der Blasen

Texture dieBlase;		//speichert die Seifenblasentextur
Texture dieOrange;		//speichert eine alternative Textur
Texture dieHilfe;		// speichert die Textur der Hilfe

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

void CleanupExit()
{
	g_scriptNode.Release();
	g_DepthGenerator.Release();
	g_UserGenerator.Release();
	g_Player.Release();
	g_Context.Release();

	exit (1);
}

// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d New User %d\n", epochTime, nId);
	// New user found
	if (g_bNeedPose)
	{
		g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
	}
	else
	{
		g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}
}
// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d Lost user %d\n", epochTime, nId);	
}
// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d Pose %s detected for user %d\n", epochTime, strPose, nId);
	g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d Calibration started for user %d\n", epochTime, nId);
}
// Callback: Finished calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	if (bSuccess)
	{
		// Calibration succeeded
		printf("%d Calibration complete, start tracking user %d\n", epochTime, nId);
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		printf("%d Calibration failed for user %d\n", epochTime, nId);
		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	if (eStatus == XN_CALIBRATION_STATUS_OK)
	{
		// Calibration succeeded
		printf("%d Calibration complete, start tracking user %d\n", epochTime, nId);		
		printf("Calibration complete, start tracking user %d\n", nId);
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		printf("%d Calibration failed for user %d\n", epochTime, nId);
		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

#define XN_CALIBRATION_FILE_NAME "UserCalibration.bin"

// Save calibration to file
void SaveCalibration()
{
	XnUserID aUserIDs[20] = {0};
	XnUInt16 nUsers = 20;
	g_UserGenerator.GetUsers(aUserIDs, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// Find a user who is already calibrated
		if (g_UserGenerator.GetSkeletonCap().IsCalibrated(aUserIDs[i]))
		{
			// Save user's calibration to file
			g_UserGenerator.GetSkeletonCap().SaveCalibrationDataToFile(aUserIDs[i], XN_CALIBRATION_FILE_NAME);
			break;
		}
	}
}
// Load calibration from file
void LoadCalibration()
{
	XnUserID aUserIDs[20] = {0};
	XnUInt16 nUsers = 20;
	g_UserGenerator.GetUsers(aUserIDs, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// Find a user who isn't calibrated or currently in pose
		if (g_UserGenerator.GetSkeletonCap().IsCalibrated(aUserIDs[i])) continue;
		if (g_UserGenerator.GetSkeletonCap().IsCalibrating(aUserIDs[i])) continue;

		// Load user's calibration from file
		XnStatus rc = g_UserGenerator.GetSkeletonCap().LoadCalibrationDataFromFile(aUserIDs[i], XN_CALIBRATION_FILE_NAME);
		if (rc == XN_STATUS_OK)
		{
			// Make sure state is coherent
			g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(aUserIDs[i]);
			g_UserGenerator.GetSkeletonCap().StartTracking(aUserIDs[i]);
		}
		break;
	}
}

//Zeigt die Hilfe an
void showHelp()
{
	glEnable(GL_TEXTURE_2D);
	    	 glShadeModel(GL_SMOOTH);
	    	 glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	    	 glClearDepth(1.0f);
	   	 glEnable(GL_DEPTH_TEST);
	  	 glDepthFunc(GL_LEQUAL);
	  	 glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glEnable(GL_BLEND);
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glColor4f(1,1,1,1);

		if(g_bBlase)
		{
			// gehe sicher, das die Textur ausgewaehlt ist
			glBindTexture(GL_TEXTURE_2D, dieHilfe.texID);
		}

		//Koordinaten festlegen
		// Kinect hat aufloesung von 640 × 480
		int x = 640 / 2;
		int y = 480 / 2;
		int r = 640 / 4;

		// Zeichne die Quadrate und lege die Textur darueber
		glBegin(GL_QUADS);
			    glTexCoord2f(0.0f, 0.0f); glVertex3f( x - r, y + r,  1.0f); //links unten
			    glTexCoord2f(1.0f, 0.0f); glVertex3f( x + r, y + r,  1.0f); //rechts unten
			    glTexCoord2f(1.0f, 1.0f); glVertex3f( x + r, y - r,  1.0f); //rechts oben
			    glTexCoord2f(0.0f, 1.0f); glVertex3f( x - r, y - r,  1.0f); //links oben
		glEnd();
}

// Wird in jedem Frame aufgerufen und zeichnet die Blasen an ihrer jeweiligen Position
void drawBubbles()
{
	glEnable(GL_TEXTURE_2D);
    	 glShadeModel(GL_SMOOTH);
    	 glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    	 glClearDepth(1.0f);
   	 glEnable(GL_DEPTH_TEST);
  	 glDepthFunc(GL_LEQUAL);
  	 glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glColor4f(1,1,1,1);

	if(g_bBlase)
	{
		// gehe sicher, das die Seifenblasentextur ausgewaehlt ist
		glBindTexture(GL_TEXTURE_2D, dieBlase.texID);
	}
	else
	{
		// gehe sicher, das die alternative Textur ausgewaehlt ist
		glBindTexture(GL_TEXTURE_2D, dieOrange.texID);
	}
	
	// Zeichne die Quadrate und lege die Textur darueber
	glBegin(GL_QUADS);

		for(std::vector<Blase>::iterator i = bla.begin(); i != bla.end(); ++i)
		{
		    glTexCoord2f(0.0f, 0.0f); glVertex3f( (*i).x - (*i).r, (*i).y + (*i).r,  1.0f); //links unten
		    glTexCoord2f(1.0f, 0.0f); glVertex3f( (*i).x + (*i).r, (*i).y + (*i).r,  1.0f); //rechts unten
		    glTexCoord2f(1.0f, 1.0f); glVertex3f( (*i).x + (*i).r, (*i).y - (*i).r,  1.0f); //rechts oben
		    glTexCoord2f(0.0f, 1.0f); glVertex3f( (*i).x - (*i).r, (*i).y - (*i).r,  1.0f); //links oben
		}

	glEnd();
}

// eine Methode um einen beliebigen Punkt (x,y) mit einem Quadrat mit Seitenlaenge 2r zu markieren
void markierePosition( int x , int y, int r)
{
	glColor4f(1,0.8,0,1);
	glBegin(GL_QUADS);

		    glVertex3f( x - r, y + r,  1.0f); //links unten
		    glVertex3f( x + r, y + r,  1.0f); //rechts unten
		    glVertex3f( x + r, y - r,  1.0f); //rechts oben
		    glVertex3f( x - r, y - r,  1.0f); //links oben

	glEnd();
}

//prueft ob eine Blase das Koerperglied zwischen joint1 und joint2 des Users usr beruehrt
void checkCollisionLimb(XnUserID usr, XnSkeletonJoint joint1, XnSkeletonJoint joint2)
{
	XnSkeletonJointPosition joint_pos;

	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition( usr, joint1, joint_pos);
	XnPoint3D limb_start = joint_pos.position;
	g_DepthGenerator.ConvertRealWorldToProjective(1, &limb_start, &limb_start);

	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition( usr, joint2, joint_pos);
	XnPoint3D limb_end = joint_pos.position;
	g_DepthGenerator.ConvertRealWorldToProjective(1, &limb_end, &limb_end);

	//Fuer jede Blase pruefen
	for(std::vector<Blase>::iterator i = bla.begin(); i != bla.end(); ++i)
	{
		//Methode: Kollision zwischen einem Kreis und einer Geraden

		// Richtungsvektor des Koerperglieds
		double ax = limb_end.X - limb_start.X;
		double ay = limb_end.Y - limb_start.Y;

		// Verbindungsvektor von start joint zum Mittelpunkt des Kreises
		double bx = (*i).x - limb_start.X;
		double by = (*i).y - limb_start.Y;

		// Das Koerperglied hat also die Geradengleichung v = limb_start + a * t
		// der punkt auf der Geraden der dem Mittelpunkt des Kreises am naechsten liegt
	    //   wird nun durch t = <a,b> / <a,a> berechnet
		double t = (ax * bx + ay * by) / (ax * ax + ay * ay);

		// wenn der Punkt nicht zwischen den joints liegt, muss man die joints selbst ueberpruefen
		if (t < 0)
		{
			t = 0;

			//pruefe mit Pythagoras ob Blase User am limb_start beruehrt
			if( bx*bx + by*by < ((*i).r)*((*i).r))
			{
				// Normalisiere den Vektor
				bx = bx / (double)(*i).r ;
				by = by / (double)(*i).r ;

				// Setze anhand des Beruehrpunktes der Blase einen Richtungsvektor
				(*i).setMove( bx * (*i).getAcc() * 1.3 , by * (*i).getAcc() * 1.3);

				// Breche Collision Ueberpruefung ab.
				return;
			}
		}
		else if (t > 1)
		{
			t = 1;

			// Verbindungsvektor von end joint zum Mittelpunkt des Kreises
			double cx = (*i).x - limb_end.X;
			double cy = (*i).y - limb_end.Y;

			//pruefe mit Pythagoras ob Blase User am limb_end beruehrt
			if( cx*cx + cy*cy < ((*i).r)*((*i).r))
			{
				// Setze anhand des Beruehrpunktes der Blase einen Richtungsvektor
				(*i).setMove( cx * (*i).getAcc() * 1.3 , cy * (*i).getAcc() * 1.3);

				// Breche Collision ueberpruefung ab.
				return;
			}
		}

		// punkt auf der Strecke berechnen da wir jetzt t kennen
		double px = limb_start.X + ax * t;
		double py = limb_start.Y + ay * t;

		// Berechne Vektor von Berührpunkt zu Mittelpunkt der Blase
		double vx = ( (*i).x - px );
		double vy = ( (*i).y - py );

		//pruefe mit Pythagoras ob Blase, User am Punkt des Limb beruehrt der am naechsten liegt
		if( vx * vx + vy * vy < ((*i).r)*((*i).r))
		{

			markierePosition( px, py, 5);

			//Vereinfachte Berechnung damit das Spielen moeglich ist
			if((*i).vx > 0 ){ (*i).vx = (*i).vx * (-1)  - 2; }
			else{(*i).vx = (*i).vx * (-1)  + 2;}
			
			if((*i).vy > 0 ){ (*i).vy = (*i).vy * (-1)  - 2; }
			else{(*i).vy = (*i).vy  + 2;}
			 
/*
//Versuch die Limbs als Vektoren aufzufassen und den Winkel zu berechnen
// Funktioniert noch nicht

			// Laenge des Koerperglieds
			double la = sqrt( ax * ax + ay * ay );

			//Laenge des Bewegungsvektors der Kugel
			double lk = sqrt( (*i).vx * (*i).vx + (*i).vy * (*i).vy );

			//Winkel zwischen Limb und Bewegungsrichtung der Kugel
			double cosW = ax * (*i).vx + ay * (*i).vy / ( lk * la );
			printf(" ax = %f und ay = %f xxxxx vx = %f und vy = %f \n" , ax, ay, (*i).vx, (*i).vy );

			if( cosW < 0){ cosW *= -1; }
			printf( "la = %f __ lk = %f __ das ist der Cosinus: %f \n", la, lk , cosW);

			//sin x = Wurzel aus (1 - cos²x)
			double sinW = sqrt(1 - cosW * cosW );
			//Drehmatrix auf Vektor anwenden um neue Richtung nach Kollision zu bestimmen
			//   | cosW  -sinW |
			//   | sinW   cosW |

			// Da vx und vy nicht mehr gebraucht werden speichere ich kurz den (*i).x wert in vx
			vx = (*i).vx;

			// Drehe den alten Vektor um den Winkel w
			(*i).vx = cosW * (*i).vx - sinW * (*i).vy;
			(*i).vy =  sinW * vx + cosW * (*i).vy;
*/
		}
	}
}

// prueft ob die Blase den user usr am joint jt beruehrt
// wird nicht mehr benoetigt da checkCollisionLimb die joints
// und das komplette koerperglied zwischen den joints ueberprueft
void checkCollisionOnJoint( XnUserID usr, XnSkeletonJoint jt )
{
	XnSkeletonJointPosition joint_pos;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition( usr, jt, joint_pos);
	XnPoint3D point = joint_pos.position;
	g_DepthGenerator.ConvertRealWorldToProjective(1, &point, &point);

	//markierePosition( point.X, point.Y, 5);

	for(std::vector<Blase>::iterator i = bla.begin(); i != bla.end(); ++i)
	{
		//pruefe mit Pythagoras ob Blase User an der position point beruehrt
		if( (point.X - (*i).x)*(point.X - (*i).x) + ( point.Y - (*i).y)*( point.Y - (*i).y) < ((*i).r)*((*i).r))
		{
			(*i).setMove( 3, -8);
		}
	}
}

void checkCollisionAll()
{
	XnUInt16 nUsers = g_UserGenerator.GetNumberOfUsers();
	XnUserID aUsers[nUsers];

	g_UserGenerator.GetUsers(aUsers, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// man kann auch nur die Gelenke pruefen
		//	checkCollisionOnJoint( aUsers[i], XN_SKEL_LEFT_HAND );
		//	checkCollisionOnJoint( aUsers[i], XN_SKEL_RIGHT_HAND );
		//	checkCollisionOnJoint( aUsers[i], XN_SKEL_LEFT_ELBOW );
		//	checkCollisionOnJoint( aUsers[i], XN_SKEL_RIGHT_ELBOW);
		//	checkCollisionOnJoint( aUsers[i], XN_SKEL_LEFT_SHOULDER );
		//	checkCollisionOnJoint( aUsers[i], XN_SKEL_RIGHT_SHOULDER);

		// Kopf pruefen
		checkCollisionOnJoint( aUsers[i], XN_SKEL_HEAD);

		// Glieder mit Gelenken pruefen
		checkCollisionLimb( aUsers[i], XN_SKEL_LEFT_HAND, XN_SKEL_LEFT_ELBOW);
		checkCollisionLimb( aUsers[i], XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_SHOULDER);
		checkCollisionLimb( aUsers[i], XN_SKEL_RIGHT_HAND, XN_SKEL_RIGHT_ELBOW);
		checkCollisionLimb( aUsers[i], XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_SHOULDER);
	}
}

//Wird in jedem Frame aufgerufen, aktualisiert die Position der Blasen indem der Richtungsvektor angewandt wird
void updateBubbles()
{
	glRasterPos2i(20, 20);
	char label[50] = "";
	sprintf( label, "Anzahl Blasen: %d", bla.size());
	glPrintString(GLUT_BITMAP_HELVETICA_18, label);

	for(std::vector<Blase>::iterator i = bla.begin(); i != bla.end(); ++i)
	{
		// Pruefe ob Blase Rand beruehrt
		// Kinect hat aufloesung von 640 × 480
		if( (*i).x < (*i).r || (*i).x > ( 640 - (*i).r ) )
		{
			// x Richtung der Blase aendern
			(*i).vx *= -1;
		}

		if( (*i).y >  GL_WIN_SIZE_Y )
		{
			// Wenn Blase auf den Boden kommt, soll sie wieder oben Starten
			(*i).newStart();

			//wollte eigentlich die Blasen loeschen wenn sie auf den Boden fallen
			// aber wenn ich diese Methode aufrufe kommt es spaeter zu einem Speicherzugrifffehler
			//bla.erase( i );
		}
		else
		{
			(*i).updateBlase();
		}
	}
}

// this function is called each frame
void glutDisplay (void)
{

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup the OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	xn::SceneMetaData sceneMD;
	xn::DepthMetaData depthMD;
	g_DepthGenerator.GetMetaData(depthMD);
#ifndef USE_GLES
	glOrtho(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);
#else
	glOrthof(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);
#endif

	glDisable(GL_TEXTURE_2D);

	if (!g_bPause)
	{
		// Read next available data
		g_Context.WaitOneUpdateAll(g_UserGenerator);
	}
	else if( g_bGame )
	{
		drawBubbles();
	}

		// Process the data
		g_DepthGenerator.GetMetaData(depthMD);
		g_UserGenerator.GetUserPixels(0, sceneMD);
		DrawDepthMap(depthMD, sceneMD);

	if( g_bGame )
	{
		checkCollisionAll();
		updateBubbles();
		drawBubbles();
	}

	if( g_bHelp )
	{
		showHelp();
	}

#ifndef USE_GLES
	glutSwapBuffers();
#endif
}

#ifndef USE_GLES
void glutIdle (void)
{
	if (g_bQuit) {
		CleanupExit();
	}

	// Display the frame
	glutPostRedisplay();
}

void glutKeyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		CleanupExit();
	case 'b':
		// Draw background?
		g_bDrawBackground = !g_bDrawBackground;
		break;
	case 'x':
		// Draw pixels at all?
		g_bDrawPixels = !g_bDrawPixels;
		break;
	case 's':
		// Draw Skeleton?
		g_bDrawSkeleton = !g_bDrawSkeleton;
		break;
	case 'i':
		// Print label?
		g_bPrintID = !g_bPrintID;
		break;
	case 'l':
		// Print ID & state as label, or only ID?
		g_bPrintState = !g_bPrintState;
		break;
	case'p':
		g_bPause = !g_bPause;
		g_bGame = !g_bGame;     // spiel pausieren/starten
		break;

	case'o':
		g_bBlase = !g_bBlase;
		break;
                
        // Starte das Seifenblasen Spiel        
        case'g':
                g_bGame = !g_bGame;     // spiel gestartet
                g_bPrintID = FALSE;     // Tracking Status muss nicht mehr angezeigt werden
                g_bHelp = FALSE;		// Hilfe auf jeden Fall mal ausblenden
                g_bDrawSkeleton = !g_bDrawSkeleton;	//Zeige Skelett nicht
                //g_bDrawBackground = FALSE;
                bla.push_back( Blase() );
                break;

        // erstelle weitere Blase
        case 'n':
        		// Ich muss dem konstruktor eine Zahl mitgeben sonst erzeugt er
        		// wenn innerhalb der gleichen Sekunde mehrmals gedrueckt wird
        		// mehrere gleiche Blasen
        		bla.push_back( Blase( bla.size()) );
        		break;

        //Zeige Hilfe an
        case 'h':
        		g_bHelp = !g_bHelp;		//Zeige Hilfe an
        		break;

	case 'S':
		SaveCalibration();
		break;
	case 'L':
		LoadCalibration();
		break;
	}
}

// Das Spiel initialisieren, hauptsaechlich die Texturen laden
void gameInit()
{
	//Das aktuelle Arbeitsverzeichnis ermitteln
	// dafuer wurden <string> und <unistd.h> importiert
	char currentPath[FILENAME_MAX];

	if (!getcwd(currentPath, sizeof(currentPath)))
    {
		printf("Fehler beim ermitteln des Dateispeicherorts der Texturen \n Wurde das Programm aus dem Ordner bin/Release/ ausgefuehrt?");
    }

	currentPath[sizeof(currentPath) - 1] = '\0'; /* not really required */
	std::string s = currentPath;
	s.erase( s.size() - 11, 11 );	//Entferne bin/Release/
	s.append( "images/" );			//Haenge speicherort der Texturen an


	// OpenGL Einstellungen zur Darstellung der Texturen der Blasen
	glEnable(GL_TEXTURE_2D);
    	 glShadeModel(GL_SMOOTH);
    	 glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    	 glClearDepth(1.0f);
   	 glEnable(GL_DEPTH_TEST);
  	 glDepthFunc(GL_LEQUAL);
  	 glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Die Seifenblasen Textur laden mithilfe von tga.h
	// wird in der globalen Textur Variable dieBlase referenziert
	LoadTGA( &dieBlase, (s + "bubble.tga").c_str());
	//LoadTGA( &dieBlase, "/home/matthias/bubbles/bubble.tga");
    //LoadTGA( &dieBlase, "/home_nfs/2013ws_bubble_a/bubbles/bubble.tga");

    // Die Textur generieren mit den Daten des TGA Files
    glGenTextures(1, &dieBlase.texID);				// Textur erstellen
    glBindTexture(GL_TEXTURE_2D, dieBlase.texID);
    glTexImage2D(GL_TEXTURE_2D, 0, dieBlase.bpp / 8, dieBlase.width, dieBlase.height, 0, dieBlase.type, GL_UNSIGNED_BYTE, dieBlase.imageData);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    if (dieBlase.imageData)						// Falls die Textur existiert
    {
    	free(dieBlase.imageData);					// kann der Bildspeicher der Textur freigegeben werden
    }

	// Die Alternative Textur laden mithilfe von tga.h
	// wird in der globalen Textur Variable dieOrange referenziert
    LoadTGA( &dieOrange, (s + "bOrange.tga").c_str());
    //LoadTGA( &dieOrange, "/home/matthias/bubbles/bOrange.tga");
    //LoadTGA( &dieOrange, "/home_nfs/2013ws_bubble_a/bubbles/bOrange.tga");

    // Die Textur generieren mit den Daten des TGA Files
    glGenTextures(1, &dieOrange.texID);				//Textur erstellen
    glBindTexture(GL_TEXTURE_2D, dieOrange.texID);
    glTexImage2D(GL_TEXTURE_2D, 0, dieOrange.bpp / 8, dieOrange.width, dieOrange.height, 0, dieOrange.type, GL_UNSIGNED_BYTE, dieOrange.imageData);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    if (dieOrange.imageData)						// Falls die Textur existiert
    {
    	free(dieOrange.imageData);					// kann der Bildspeicher der Textur freigegeben werden
    }

	// Die  Textur der Hilfe laden mithilfe von tga.h
	// wird in der globalen Textur Variable dieHilfe referenziert
    LoadTGA( &dieHilfe, (s + "help.tga").c_str());

    // Die Textur generieren mit den Daten des TGA Files
    glGenTextures(1, &dieHilfe.texID);				//Textur erstellen
    glBindTexture(GL_TEXTURE_2D, dieHilfe.texID);
    glTexImage2D(GL_TEXTURE_2D, 0, dieHilfe.bpp / 8, dieHilfe.width, dieHilfe.height, 0, dieHilfe.type, GL_UNSIGNED_BYTE, dieHilfe.imageData);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    if (dieHilfe.imageData)						// Falls die Textur existiert
    {
    	free(dieHilfe.imageData);					// kann der Bildspeicher der Textur freigegeben werden
    }
}

void glInit (int * pargc, char ** argv)
{
	glutInit(pargc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow ("Seifenblasen balancieren");
      glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
#endif // USE_GLES

// Der Pfad ist fuer das Compilieren wichtig
#define SAMPLE_XML_PATH "../../openniRedist/Samples/Config/SamplesConfig.xml"

#define CHECK_RC(nRetVal, what)										\
	if (nRetVal != XN_STATUS_OK)									\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));\
		return nRetVal;												\
	}

int main(int argc, char **argv)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (argc > 1)
	{
		nRetVal = g_Context.Init();
		CHECK_RC(nRetVal, "Init");
		nRetVal = g_Context.OpenFileRecording(argv[1], g_Player);
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Can't open recording %s: %s\n", argv[1], xnGetStatusString(nRetVal));
			return 1;
		}
	}
	else
	{
		xn::EnumerationErrors errors;
		nRetVal = g_Context.InitFromXmlFile(SAMPLE_XML_PATH, g_scriptNode, &errors);
		if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
		{
			XnChar strError[1024];
			errors.ToString(strError, 1024);
			printf("%s\n", strError);
			return (nRetVal);
		}
		else if (nRetVal != XN_STATUS_OK)
		{
			printf("Open failed: %s\n", xnGetStatusString(nRetVal));
			return (nRetVal);
		}
	}

	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	if (nRetVal != XN_STATUS_OK)
	{
		printf("No depth generator found. Using a default one...");
		xn::MockDepthGenerator mockDepth;
		nRetVal = mockDepth.Create(g_Context);
		CHECK_RC(nRetVal, "Create mock depth");

		// set some defaults
		XnMapOutputMode defaultMode;
		defaultMode.nXRes = 320;
		defaultMode.nYRes = 240;
		defaultMode.nFPS = 30;
		nRetVal = mockDepth.SetMapOutputMode(defaultMode);
		CHECK_RC(nRetVal, "set default mode");

		// set FOV
		XnFieldOfView fov;
		fov.fHFOV = 1.0225999419141749;
		fov.fVFOV = 0.79661567681716894;
		nRetVal = mockDepth.SetGeneralProperty(XN_PROP_FIELD_OF_VIEW, sizeof(fov), &fov);
		CHECK_RC(nRetVal, "set FOV");

		XnUInt32 nDataSize = defaultMode.nXRes * defaultMode.nYRes * sizeof(XnDepthPixel);
		XnDepthPixel* pData = (XnDepthPixel*)xnOSCallocAligned(nDataSize, 1, XN_DEFAULT_MEM_ALIGN);

		nRetVal = mockDepth.SetData(1, 0, nDataSize, pData);
		CHECK_RC(nRetVal, "set empty depth map");

		g_DepthGenerator = mockDepth;
	}

	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
	if (nRetVal != XN_STATUS_OK)
	{
		nRetVal = g_UserGenerator.Create(g_Context);
		CHECK_RC(nRetVal, "Find user generator");
	}

	XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected, hCalibrationInProgress, hPoseInProgress;
	if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
	{
		printf("Supplied user generator doesn't support skeleton\n");
		return 1;
	}
	nRetVal = g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
	CHECK_RC(nRetVal, "Register to user callbacks");
	nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
	CHECK_RC(nRetVal, "Register to calibration start");
	nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);
	CHECK_RC(nRetVal, "Register to calibration complete");

	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
	{
		g_bNeedPose = TRUE;
		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
		{
			printf("Pose required, but not supported\n");
			return 1;
		}
		nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
		CHECK_RC(nRetVal, "Register to Pose Detected");
		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	}

	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationInProgress(MyCalibrationInProgress, NULL, hCalibrationInProgress);
	CHECK_RC(nRetVal, "Register to calibration in progress");

	nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseInProgress(MyPoseInProgress, NULL, hPoseInProgress);
	CHECK_RC(nRetVal, "Register to pose in progress");

	nRetVal = g_Context.StartGeneratingAll();
	CHECK_RC(nRetVal, "StartGenerating");

#ifndef USE_GLES
	glInit(&argc, argv);
	gameInit();
	glutMainLoop();
#else
	if (!opengles_init(GL_WIN_SIZE_X, GL_WIN_SIZE_Y, &display, &surface, &context))
	{
		printf("Error initializing opengles\n");
		CleanupExit();
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	// Das Spiel initialisieren
	gameInit();

	while (!g_bQuit)
	{
		glutDisplay();
		eglSwapBuffers(display, surface);
	}
	opengles_shutdown(display, surface, context);

	CleanupExit();
#endif
}
