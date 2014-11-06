# project name (generate the executable file with this name)
TARGET = executable

# currently directory of each file
SRCDIR 		= src
INCLUDEDIR 	= include
OBJDIR 		= obj
LIBDIR 		= lib
BINDIR 		= bin

# compiler version in use
CC = g++

# linker's arguments
EXELINKER = $(CC) -o
LIBLINKER = ar -crs

# compiling flags here
CFLAGS 	=	-Ofast \

OPENCVFLAGS = `pkg-config --cflags --libs opencv libv4l2` 			

INCLUDESPATH = -I./$(INCLUDEDIR) 

# list of libraries used
LIBS = -lm -pthread

# general definition of the files
STATICLIB 	= $(LIBDIR)/lib$(TARGET).a
SOURCES 	:= $(wildcard $(SRCDIR)/*.cpp)
INCLUDES 	:= $(wildcard $(INCLUDEDIR)/*.h)
OBJECTS 	:= $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# clean process
rm = rm -f

# directories creation process
mkdir = mkdir -p

# find process
find = find -maxdepth 1 -name

# move process
mv = mv -i

# correct time files
time = find ./ -maxdepth 2 -exec touch --date="01 JAN 1998" {} \;

# links the objects creating the static library and then the executable
$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(time)
	@$(LIBLINKER) $(STATICLIB) $(OBJECTS)
	@$(EXELINKER) $@ $(STATICLIB) $(OPENCVFLAGS) $(LIBS)
	@echo "Linking complete!"

# compilates the sources creating the objects
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(INCLUDES) Makefile
	@$(CC) $(CFLAGS) $(INCLUDESPATH) -c $< -o $@
	@echo "Compiled "$<" successfully!"

# executes the target
exe: $(BINDIR)/$(TARGET)
	@$(BINDIR)/$(TARGET)

# cleanup
PURGE: clean_bin clean_lib clean_obj

# cleans the executable only
clean_bin:
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

# cleans the static library only
clean_lib:
	@$(rm) $(STATICLIB)
	@echo "Static library removed!"

# cleans the objects only
clean_obj:
	@$(rm) $(OBJECTS)
	@echo "Objects removed!"
	
force: PURGE $(BINDIR)/$(TARGET)

force-exe: PURGE exe

# creates the folders if needed
# and moves the archives to its proper folder
organize:
	@$(mkdir) $(SRCDIR) $(INCLUDEDIR) $(OBJDIR) $(LIBDIR) $(BINDIR)
	@$(find) "*.a" -exec $(mv) "{}" "./$(LIBDIR)/{}" \;
	@$(find) "*.cpp" -exec $(mv) "{}" "./$(SRCDIR)/{}" \;
	@$(find) "*.h" -exec $(mv) "{}" "./$(INCLUDEDIR)/{}" \;
	@$(find) "*.o" -exec $(mv) "{}" "./$(OBJDIR)/{}" \;
	@$(find) "$(TARGET)*" -exec $(mv) "{}" "./$(BINDIR)/{}" \;
	@sleep 1.5
	@echo "The files have been organized!"
