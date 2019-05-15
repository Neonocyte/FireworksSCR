CC = g++

#add your path to the SDL2 and SDL2-image include directories here; start each with -I
INCLUDE = -I

#add your path to the SDL2 and SDL2-image libraries here; start each with -L
LIB = -L

COMPILER_FLAGS = -w

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

WINDOWS_FLAGS = -mwindows

OBJS = main.cpp entity.cpp particle.cpp emitter.cpp

OUTPUT = Fireworks

#debug will run a few visual tests and allow the console to display and output program information while running
#windows should be used when building for a Windows machine; after compiling, the output file will need to be renamed with a .scr file ending to be used as a screensaver
all: $(OBJS)
		$(CC) $(OBJS) $(INCLUDE) $(LIB) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OUTPUT)

debug: $(OBJS)
		$(CC) -DDEBUG -g $(OBJS) $(INCLUDE) $(LIB) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OUTPUT)
		
windows: $(OBJS)
		$(CC) $(OBJS) $(INCLUDE) $(LIB) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(WINDOWS_FLAGS) -o $(OUTPUT)