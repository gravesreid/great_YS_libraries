.SUFFIXES: .c .cpp .obj


TARGETS=sample01.exe sample02.exe sample03.exe sample04.exe sample05.exe sample06.exe sample07.exe simldg\ldg.exe


LIBS=ysclass.lib kernel32.lib user32.lib gdi32.lib advapi32.lib impls32d.lib

# This combination is for Blue Impulse on OpenGL
#LIBS=ysclass.lib kernel32.lib user32.lib gdi32.lib advapi32.lib impls32o.lib opengl32.lib glu32.lib


CC=cl
CFLAGS=/W3 /Z7 /DWIN32 /O2 /G5 /ML
MAKE=nmake



all : $(TARGETS)


.c.exe:
	$(CC) $(CFLAGS) $*.c $(LIBS)

.cpp.exe:
	$(CC) $(CFLAGS) $*.cpp $(LIBS)

simldg\ldg.exe : simldg\ldg.cpp
	$(CC) $(CFLAGS) simldg\ldg.cpp /Fesimldg\ldg.exe $(LIBS)

