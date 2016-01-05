#
# Base Makefile for RainbowAPs project.
#
TESTS 	=
 
TARGETS = rainbow.exe rainbow_zn.exe 


OBJECTS =				
			


CC 	= gcc
CXX = g++

OPT =  -g 
# -O7
DEBUG =  
# -g 
# -static 
WARNINGS = -Wall


CFLAGS 		= $(OPT) $(DEBUG) $(WARNINGS)
CXXFLAGS 	= $(OPT) $(DEBUG) $(WARNINGS)
LFLAGS 		= $(OPT) $(DEBUG) $(WARNINGS) -g


			
			
.SUFFIXES: .c .cpp .o .obj .exe 

all: $(OBJECTS) $(TESTS) $(TARGETS)


# The default object compiler
.c.o: $<
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -c $< -o $@
        
.cpp.o: $<
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $< -o $@
        
.cpp.exe: $< $(OBJECTS)  
	$(CXX) $(LFLAGS)			\
        	$(INCLUDES)	$(DEBUG)			\
        	$(LIBOBJS) $(LIBS)				\
            -g $< -o $@
        
.c.exe: $< $(COBJECTS)
	$(CC) 	$(LFLAGS)			    \
        	$(INCLUDES)				\
        	$(NAUTYOBJS)  $(COBJECTS) $(LIBS)		\
            $< -o $@
	cp $@ ../bin/
        
clean:
	rm $(OBJECTS) $(TARGETS) $(TESTS)
	rm -rf *.exe.*
	
cleanexe:
	rm $(TARGETS)

cleantest:
	rm $(TESTS)

clexe:
	rm $(TARGETS)
