include Common/CommonDefs.mak

.PHONY: all

NETPROJ = 
ifneq "$(realpath /usr/bin/gmcs)" ""
	NETPROJ += UserTracker.net
	NETPROJ += SimpleViewer.net
	NETPROJ += SimpleRead.net
endif

all: $(NETPROJ) SimpleRead.java UserTracker.java NiSimpleViewer NiViewer NiSampleModule NiRecordSynthetic SimpleViewer.java NiCRead NiUserTracker NiAudioSample NiBackRecorder NiConvertXToONI NiSimpleRead NiSimpleCreate 


.PHONY: SimpleRead.java
SimpleRead.java:
	$(MAKE) -C ../SimpleRead.java

.PHONY: UserTracker.java
UserTracker.java:
	$(MAKE) -C ../UserTracker.java

.PHONY: NiSimpleViewer
NiSimpleViewer:
	$(MAKE) -C ../NiSimpleViewer

.PHONY: NiViewer
NiViewer:
	$(MAKE) -C ../NiViewer

.PHONY: NiSampleModule
NiSampleModule:
	$(MAKE) -C ../NiSampleModule

.PHONY: NiRecordSynthetic
NiRecordSynthetic:
	$(MAKE) -C ../NiRecordSynthetic

.PHONY: SimpleViewer.java
SimpleViewer.java:
	$(MAKE) -C ../SimpleViewer.java

.PHONY: NiCRead
NiCRead:
	$(MAKE) -C ../NiCRead

.PHONY: NiUserTracker
NiUserTracker:
	$(MAKE) -C ../NiUserTracker

.PHONY: NiAudioSample
NiAudioSample:
	$(MAKE) -C ../NiAudioSample

.PHONY: NiBackRecorder
NiBackRecorder:
	$(MAKE) -C ../NiBackRecorder

.PHONY: NiConvertXToONI
NiConvertXToONI:
	$(MAKE) -C ../NiConvertXToONI

.PHONY: NiSimpleRead
NiSimpleRead:
	$(MAKE) -C ../NiSimpleRead

.PHONY: UserTracker.net
UserTracker.net:
	$(MAKE) -C ../UserTracker.net

.PHONY: SimpleViewer.net
SimpleViewer.net:
	$(MAKE) -C ../SimpleViewer.net

.PHONY: NiSimpleCreate
NiSimpleCreate:
	$(MAKE) -C ../NiSimpleCreate

.PHONY: SimpleRead.net
SimpleRead.net:
	$(MAKE) -C ../SimpleRead.net
