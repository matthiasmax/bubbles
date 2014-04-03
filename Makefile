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

USED_LIBS += SOIL

USED_LIBS += OpenNI

LIB_DIRS += ../../Lib
include openniRedist/Samples/Build/Common/CommonCppMakefile

