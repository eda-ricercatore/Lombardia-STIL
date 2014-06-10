# This is written by Zhiyang Ong to parse a STIL file into a FLAT tabular
# test pattern format that contains input test patterns for execution on
# automatic test equipment

# Definition of macros...
# IMPORTANT NOTE: Makefile is written for Mac OS X 10.4.8
# Hence, g++ is used instead of gcc
# In other UNIX-based operating systems, switch to gcc
GPLUSPLUS=	g++
COMPILE=	-c
LINK=		-o
RM=			rm -rf
EXECUTABLE=	./parse.out
OBJFILES=	*.o
SOURCE=		*.cpp
INPUT=		stil.1
OUTPUT=		output.txt

# Definition of Target Rules
all:
	@echo ===Compiling C++ source code...
	$(GPLUSPLUS)	$(COMPILE)	$(SOURCE)
	@echo ===Link the C++ object files...
	$(GPLUSPLUS)	$(LINK)		$(EXECUTABLE)	$(OBJFILES)
	@echo ===Executing the software...
	$(EXECUTABLE)	stil.1	output.txt
	@echo ===View the output file
	more $(OUTPUT)

# Remove all executables
clean:
	@echo ===Removing executables from the subdirectories...
	$(RM)	$(EXECUTABLE)
	$(RM)	$(OBJFILES)
	$(RM)	$(OUTPUT)
	
# Run/Execute the program
run:
#	@echo -->Executing the SoFTwaRe...
	$(EXECUTABLE)	$(INPUT)	$(OUTPUT)
	
numlines:
	cat *.h *.cpp | wc -l


help:
	@echo The options are:
	@echo all:_______Compile all source files and link their object code, and run the program
	@echo clean:_____Remove all executables, object files, and output files
	@echo run:_______Execute the program
	@echo numlines:__Count the number of lines in the source code and header files