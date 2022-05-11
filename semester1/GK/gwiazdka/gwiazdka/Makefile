
include ../options.mak

LIBS = -lglut $(PROCLIB) -ldl -lGLU -lGL -lm

OBJECTS = gl3w.o utilities.o timer.o lights.o mygltext.o \
  font12x6.o font18x10.o app1dproc.o

TARGET = gwiazdka

default: $(TARGET)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $*.o

gl23w.o: gl3w.c

utilities.o: utilities.c utilities.h

timer.o: timer.c utilities.h

lights.o: lights.c utilities.h lights.h

mygltext.o: mygltext.c utilities.h mygltext.h

font12x6.o: font12x6.c utilities.h mygltext.h

font18x10.o: font18x10.c utilities.h mygltext.h

app1dproc.o: app1dproc.c utilities.h lights.h app1d.h

gwiazdka: app1d.c utilities.h mygltext.h lights.h app1d.h $(OBJECTS)
	$(CC) $(CFLAGS) app1d.c $(OBJECTS) $(LIBS) -o gwiazdka

clean:
	rm -f *.o

mrproper:
	rm -f *.o $(TARGET)
