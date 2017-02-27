Lombardia-STIL
==============

A C++ -based STIL parser.

A parser is written for STIL test vectors to produce a FLAT tabular test pattern. In the tabular file, the scan sequences are to be represented as indicated in the original STIL file. See stil_parser.zip
The code can be compiled and executed using "make all" in a UNIX environment. The make-file is named "Makefile", and a read-me file is named "readme.txt".

 To run the program, try: ./parse.out stil.1 output.txt

The executable is named "parse.out" and the input file is "stil.1". The output file is named "output.txt".
No error checking has been developed for the absence or modification of STIL keywords. I realized this halfway through the development of the software, and stuck to my original design plans due to a lack of time. I should have parsed the "stil.1" file to search for keywords, store them in a data structure. When I am processing the definitions of the macro, pattern, and other blocks, I can traverse these data structures to search for these keywords in the "stil.1" file. Instead, I chose to skip this step and assume that the instance names for the blocks are predefined, and will not be changed. I am wrong in assuming this. Consequently, when the parser processes the input file, it assumes that certain keywords must exist in the file as names for the block definitions or patterns, macros, and procedures.

It is noted that the data on the PI/PO and scan pins (in output.txt) do not match. The length of each scan pins should be the same as that of the scan register. Ditto for PI/PO scan size plus load/unload vectors. My current lack of familiarity of the STIL interface has resulted in developing assumptions that affected the design of my algorithm. I recognize that the scan pins are fed from the primary output and input pins, but was unable to figure out how they were connected. Hence, I assumed that the conditions C indicated the initial values for the pins, and that the values V are the only input values passed into the pins. Since piPin had values passed to it twice in the pattern definition, it would have more data than the other input pins.

 In addition, the scan input pins will load in the pattern P that is defined in the scanproc procedure definition when the value P is encountered as a value definition. Thus, the clock signals would load more data values than other pins. Moreover, I assumed that when the value of a signal is defined to be "#", it will process the most recent pattern defined for this signal. If the signals has no defined patterns, it will assume the value of logic high "1". Since the patterns in "stil.1" are defined for scan input and output signals, and the value of P is assigned only to clock signals, the sizes of the signals do not match.

The development of the next version of this software to parse STIL test vectors would use lex/yacc or lex/bison to provide parsing with error correction. 

