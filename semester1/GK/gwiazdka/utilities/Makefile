
include ../options.mak

OBJECTS = gl3w.o utilities.o timer.o sproduct.o teapot.o \
  xwidgets.o linkage.o initglxctx.o quaternions.o

default: $(OBJECTS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $*.o

gl3w.o: gl3w.c

utilities.o: utilities.c openglheader.h utilities.h

timer.o: timer.c openglheader.h utilities.h

sproduct.o: sproduct.c openglheader.h utilities.h bezpatches.h sproduct.h

teapot.o: teapot.c openglheader.h utilities.h bezpatches.h teapot.h

xwidgets.o: xwidgets.c openglheader.h xwidgets.h xvariables.h

linkage.o: linkage.c openglheader.h utilities.h linkage.h

initglxctx.o: initglxctx.c openglheader.h xvariables.h initglxctx.h utilities.h

quaternions.o: quaternions.c openglheader.h utilities.h quaternions.h

clean:
	rm -f *.o

mrproper:
	rm -f *.o

