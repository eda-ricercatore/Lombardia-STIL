This is written by Zhiyang Ong to parse a STIL file into a FLAT tabular test pattern format that contains input test patterns for execution on automatic test equipment.

The program is developed and tested on Mac OS X 10.4.9 and Sun Solaris 9.




PROGRAM DESCRIPTION:
First, it parses the file, and processes the descriptive information about the test, such as its day, month, date, and year.

Secondly, it determines which signals are in the system. I have only assumed that signals are unidirectional. 

Next, it processes the pattern block to determine the test pattern for the integrated circuit or system. As it processes the pattern block, it defines the waveform patterns of the signals as appropriate. It also loads the initial conditions of the signals, and then processes its subsequent values. In addition, it will shift in signal vectors that have been defined by the test input file.

Lastly, when the pattern block definition has been processed, the program terminates.











IMPORTANT INFORMATION AND ASSUMPTIONS:
# Definition of logic values in the output file:
A logic zero is indicated by 0;
A logic one is indicated by 1;
A logic of high impedance is indicated by 2;
A logic that is undefined is indicated by 3;

# The initial Condition statement defines the background states for all signals during application of the scan data in the Shift block, including activating the MASTER and SLAVE clocks, and defining the possible padding states for scan inputs (0) and scan outputs (X). This condition ensures that the state of all device signals are the same during the unload and load shift blocks, and the same pre-shift and post-shift vectors are applied. Therefore, STIL translators may merge the scan data and apply the inputs while simultaneously observing the previous tests’ results. That is, a condition is used to indicate the initial values of signals before further values are assigned to them during testing.

# Since wave properties for a group input (including clock signals) and output signals can be redefined within the waveform block definition, the waveform properties of existing clock signals shall be erased when accessing each group. That is, the list of waveform properties is cleared whenever a waveform block is processed; this occurs during the execution of the function to process the waveform block so that the programmer (method/function caller) does not have to clear the waveform block just before the method call to process the waveform block

# For each interval of the clock period, the value of the signal is indicated. This is because I have chosen to represent the logic of the signals, including the clocks, in terms of time intervals within a clock period. This yields more accurate representation of the signal vectors. However, the author notes that the length of the time interval used in the quantization of the signal vector was manually determined. Its value was a posteriori chosen based on the observation of the clock periods by the author... The author notes that the value should be chosen based on the automated parsing of the input text file. In the interest of completing this software promptly, the author did not provide another function to automatically parse the file once to determine the clock periods for the signals. The author also notes that this missing function would parse the input text file, or enumerate its parsed string tokens, an additional time, and would not add to the complexity of the software. The number of timing intervals is determined by the quotient of the clock period (dividend) and the length of each timing interval (the divisor). That is, number of intervals = clock period / length of timing interval

# Assume that the signals rise/fall at the rising edge of the selected clock

# Assume that when a clock signal has a condition of "P", it will use the logic value of "1" as its condition if it has no prior condition or value to consider. I have chosen that over ground, since I can load a clock period as opposed to a grounded logic value for a clock period.

# It is acceptable to have signal vectors, or input patterns to each signal, that are different in length. This is because they vary in input frequency, and the signal inputs differ in length.


# Replaced the string "sampleSetUp" with "sampleSetup" so that the cases for the marco definition (sampleSetUp) and its defined usage (sampleSetup) correspond to each other.

# An assumption that there are only two groups of signals is made. They represent input and output signals. That is, no other groups of signals in the system are of concern to the tester, and a black box abstraction of the system is made to justify the input/output signal representation.






PROBLEMS FACED:
# I had problems implementing static functions in the SignalZ class that can be called from another class... C++ does not allow static functions of a class to be called from another class, if that other class resides in a separate file. This may be resolved by including the SignalZ class in a file that that call its static functions... But this makes the software architecture cumbersome, and less modular. Another method is to use extern functions. I have problems implementing "extern" functions in C++. Also, I realize that using static functions in the class SignalZ will not reduce coupling, since I will still have to access functions and variables in that class. Consequently, I have chosen to use instance methods instead. IMPORTANT NOTE: Unlike C++, Java allows static functions to be declared in a file, and called from another class residing in a separate file.
To be more specific: instead of passing the information from the file_analyzer class into a static variable in the SignalZ class using a simple static method, I have to update all instance variables in appropriate signal classes using instance methods.
This implies that the code can be refactored into a generic accessor/mutator method for the signal class that accepts keys (using the enum data structure) for deciding which data item (instance variable) to update/access.




REFERENCE:
Test Technology Standards Committee of the IEEE Computer Society/IEEE-SA Standards Board, "IEEE Standard Test Interface Language (STIL) for Digital Test Vector Data", IEEE Std 1450-1999, New York, NY, 1999