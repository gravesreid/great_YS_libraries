.SUFFIXES: .c .cpp .obj


TARGETS=sample01.exe sample02.exe sample03.exe sample04.exe sample05.exe sample06.exe sample07.exe sample08.exe sample09.exe


LIBS=ysclass.lib


CC=cl
CFLAGS=/W3 /Z7 /DWIN32 /O2 /G5 /ML
MAKE=nmake



all : $(TARGETS)


.c.exe:
	$(CC) $(CFLAGS) $*.c $(LIBS)

.cpp.exe:
	$(CC) $(CFLAGS) $*.cpp $(LIBS)
