MAKEFILE = Makefile

CFLAGS = /G7 /O2 /W3 /c /nologo /DWIN32
#CFLAGS = /W3 /c /DEBUG /nologo
LFLAGS = /nologo /subsystem:windows /opt:nowin98
#LFLAGS = /incremental:no /fixed /nologo

CC = cl
LINK = link

all: worldtime.exe

WorldTimeDLL.dll: WorldTimeDLL.obj
	$(LINK) /dll /out:$@ /nologo WorldTimeDLL.obj

WorldTime.obj: Makefile WorldTime.cpp
	$(CC) $(CFLAGS) WorldTime.cpp

WorldTimeDLL.obj: Makefile WorldTimeDLL.cpp WorldTimeDLL.h
	$(CC) $(CFLAGS) /D "_EXPORTING" WorldTimeDLL.cpp

DrawWorldTime.obj: Makefile DrawWorldTime.cpp DrawWorldTime.h
	$(CC) $(CFLAGS) DrawWorldTime.cpp

WorldTimeConfig.obj: Makefile WorldTimeConfig.cpp WorldTimeConfig.h
	$(CC) $(CFLAGS) WorldTimeConfig.cpp

worldtime.exe: Makefile WorldTimeDLL.h WorldTimeDLL.dll WorldTimeDLL.lib \
			DrawWorldTime.obj WorldTimeConfig.obj \
			WorldTime.obj WorldTime.res
	$(LINK) $(LFLAGS) WorldTime.obj DrawWorldTime.obj WorldTimeConfig.obj \
			WorldTimeDLL.lib kernel32.lib user32.lib gdi32.lib comctl32.lib \
			WorldTime.res /out:$@

WorldTime.res: worldmap.bmp WorldTime.ico WorldTime.rc
	rc WorldTime.rc

clean:
	del /s /q *.obj *.exe *.dll *.lib *.exp
