CC=g++

CFLAGS=-c -DMACOSX -I../../src

LIBS=-framework Cocoa -framework OpenGL

OBJDIR=sample00-lineart-obj

OBJSIMPLEWIN=$(OBJDIR)/fsmacosxwrapper.o $(OBJDIR)/fsmacosxwrappercpp.o

OBJAPP=$(OBJDIR)/sample00-lineart.o
OBJS=$(OBJAPP) $(OBJSIMPLEWIN)

EXE=sample00-lineart

$(EXE).app/Contents/MacOS/$(EXE) : $(EXE)
	mkdir -p $(EXE).app/Contents/MacOS
	cp $(EXE) $(EXE).app/Contents/MacOS/.

$(EXE) : $(OBJDIR) $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(LIBS)

run :
	$(EXE).app/Contents/MacOS/$(EXE)

link :
	$(CC) -o $(EXE) $(OBJS) $(LIBS)
	mkdir -p $(EXE).app/Contents/MacOS
	cp $(EXE) $(EXE).app/Contents/MacOS/.

$(OBJDIR) :
	mkdir -p $(OBJDIR)



$(OBJDIR)/sample00-lineart.o : sample00-lineart.cpp
	$(CC) $? $(CFLAGS) -o $@



$(OBJDIR)/fsmacosxwrapper.o : ../../src/macosx/fsmacosxwrapper.m
	$(CC) $? $(CFLAGS) -o $@

$(OBJDIR)/fsmacosxwrappercpp.o : ../../src/macosx/fsmacosxwrappercpp.cpp
	$(CC) $? $(CFLAGS) -o $@

