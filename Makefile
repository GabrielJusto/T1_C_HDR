#
# Makefile para Linux e macOS
#

SOURCE  = main.c opengl.c
OBJECTS = $(SOURCE:.c=.o)

UNAME = `uname`

all: $(TARGET)
	-@make $(UNAME)

Darwin: $(OBJECTS)
	gcc $(OBJECTS) -Wno-deprecated -framework OpenGL -framework GLUT -lm -o hdrvis

Linux: $(OBJECTS)
	gcc $(OBJECTS) -lGL -lGLU -lglut -lm -o hdrvis

clean:
	-@ rm -f $(OBJECTS) hdrvis
