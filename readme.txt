-----------------------------------------------------------------------------------------------------------
| Anforderungen fuer bubbles
-----------------------------------------------------------------------------------------------------------
| // Wenn zuerst die Anleitung des Repositorys kinect befolgt wurde
| // genuegt es einfach dieses Repository zu laden, dann sollte nach einem make
| // in bin/Release die ausfuehrbahre Datei seifenblasen liegen
-----------------------------------------------------------------------------------------------------------
___________________________________________________________________________________________________________
1. OpenNi, SensorKinect, NiTe
	- Schauen sie hierzu in das dafuer erstellte Git Repository

2. den ordner openni  (in den bubbles ordner legen)
(genauer gesagt den ordner openni/Platform/Linux-x86/Redist)
in diesem muss man den Redist ordner erstellen indem man in das Verzeichnis 
openni/Platform/Linux-x86/CreateRedist
geht und dort ./RedistMaker ausfuehrt (evtl chmod +x RedistMaker noetig)

dann muss man den pfad der samplesConfig in den Quelldateien aendern z.B. in
#define SAMPLE_XML_PATH "../../openniRedist/Samples/Config/SamplesConfig.xml"
wenn man den redist ordner in den gleichen ordner wie das make file und die quelldateien legt.
Dann muss noch das Makefile geaendert werden 
z.B. so:

OSTYPE := $(shell uname -s)

BIN_DIR = bin

INC_DIRS = openniRedist/Include /usr/include/ni

SRC_FILES = \
	main.cpp \
	SceneDrawer.cpp	

EXE_NAME = seifenblasen

ifneq "$(GLES)" "1"
ifeq ("$(OSTYPE)","Darwin")
	LDFLAGS += -framework OpenGL -framework GLUT
else
	USED_LIBS += glut
endif
else
	DEFINES += USE_GLES
	USED_LIBS += GLES_CM IMGegl srv_um
	SRC_FILES += opengles.cpp
endif

USED_LIBS += OpenNI

LIB_DIRS += ../../Lib
include openniRedist/Samples/Build/Common/CommonCppMakefile


-----------------------------------------------------------------------------------------------------------



OpenNI (Version 1.3.3.6 Stable version - Sep 18th 2011)
-------------------------------------------------------

  
Build Notes:
------------

Linux:
	Requirements:
		1) GCC 4.x
		   From: http://gcc.gnu.org/releases.html
		   Or via apt:
		   sudo apt-get install g++
		2) Python 2.6+/3.x
		   From: http://www.python.org/download/
		   Or via apt:
		   sudo apt-get install python
		3) LibUSB 1.0.8 
		   From: http://sourceforge.net/projects/libusb/
		   Or via apt:
		   sudo apt-get install libusb-1.0-0-dev
		4) FreeGLUT3
		   From: http://freeglut.sourceforge.net/index.php#download
		   Or via apt:
		   sudo apt-get install freeglut3-dev
		5) JDK 6.0
		   From: http://www.oracle.com/technetwork/java/javase/downloads/jdk-6u26-download-400750.html
		   Or via apt:
		   sudo add-apt-repository "deb http://archive.canonical.com/ lucid partner"
		   sudo apt-get update
		   sudo apt-get install sun-java6-jdk
		   
	Optional Requirements (To build the documentation):
		1) Doxygen
		   From: http://www.stack.nl/~dimitri/doxygen/download.html#latestsrc
		   Or via apt:
		   sudo apt-get install doxygen
		2) GraphViz
		   From: http://www.graphviz.org/Download_linux_ubuntu.php
		   Or via apt:
		   sudo apt-get install graphviz

	Optional Requirements (To build the Mono wrapper):
		1) Mono
		   From: http://www.go-mono.com/mono-downloads/download.html
		   Or via apt:
		   sudo apt-get install mono-complete
		   
	Building OpenNI:
		1) Go into the directory: "Platform/Linux-x86/CreateRedist".
		   Run the script: "./RedistMaker".
		   This will compile everything and create a redist package in the "Platform/Linux-x86/Redist" directory.
		   It will also create a distribution in the "Platform/Linux-x86/CreateRedist/Final" directory.
		2) Go into the directory: "Platform/Linux-x86/Redist".
		   Run the script: "sudo ./install.sh" (needs to run as root)

  		   The install script copies key files to the following location:
		       Libs into: /usr/lib
		       Bins into: /usr/bin
		       Includes into: /usr/include/ni
		       Config files into: /var/lib/ni
			
		To build the package manually, you can run "make" in the "Platform\Linux-x86\Build" directory.
		If you wish to build the Mono wrappers, also run "make mono_wrapper" and "make mono_samples".
		
		Important: Please note that even though the directory is called Linux-x86, you can also use it to compile it for 64-bit targets and pretty much any other linux based environment.
