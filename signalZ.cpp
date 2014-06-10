/**
 * This is written by Zhiyang Ong to parse a STIL file into a FLAT tabular
 * test pattern format that contains input test patterns for execution on
 * automatic test equipment
 *
 * Function to parse files for static analysis, and generation of output
 * file containing the test patterns for the automatic test equipment
 *
 * IMPORTANT ASSUMPTIONS:
 * Assume that the signals rise/fall at the rising edge of the selected clock
 * Assume that the condition refers to the trigger for setting up the clock
 * signals without placing values in the output.
 *
 * Reference:
 * #IEEE Standard Test Interface Language (STIL) for Digital Test Vector Data,
 *	IEEE Std 1450-1999, 1999
 */

// Import Header files from the C++ STL and the directory
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cstring>
#include <iomanip>
#include <climits>
#include <ctype.h>

#include "signalZ.h"
#include "file_analyzer.h"

#include "ViolatedAssertion.h"
#include "ViolatedPostcondition.h"
#include "ViolatedPrecondition.h"

// List of waveform/timing properties of the signal...
typedef list<signalZ::wave_prop> wp_list;
// Pointer to list of waveform/timing properties
typedef list<signalZ::wave_prop>::iterator wp_l_p;

using namespace std;

// =======================================================================

//class file_analyzer;

// Default constructor
signalZ::signalZ()
{	
	sig_name=UNDEFINED;
	sig=UNDEFINED;
	trigger=UNDEFINED;
	sig_is_odd=true;
}

// Standard constructor
signalZ::signalZ(string name) {
	sig_name=name;
	sig="";
	trigger="";
	sig_is_odd=true;
}

// =======================================================================

// Initializing constant...
const string signalZ::UNDEFINED = string("UNDEFINED");
// Low logic value for the signal
const string signalZ::L = string("L");
// High logic value for the signal 
const string signalZ::H = string("H");
/**
 * Unknown logic value for a signal
 * OR a logic high value in waveform definitions
 */
const string signalZ::U = string("U");
// High logic value for the signal
const string signalZ::ONE = string("1");
// Low logic value for the signal
const string signalZ::ZERO = string("0");
// Don't-care logic value for the signal
const string signalZ::X = string("X");
/**
 * Logic value is unknown or in high impedance
 * This is used to clear up the ambiguity between unknown logic values
 * and logic high values with the symbol "U"
 * This is only used to represent unknown logic values or logic values of
 * high impedance in the signal vector that will be printed in the output
 * file
 */
const string signalZ::Z = string("Z");
/**
 * Indicator for logic signal values that help define the waveform properties
 * of a system
 */
const string signalZ::P = string("P");
const string signalZ::HASH = string("#");
const string signalZ::D_U = string("D/U");
const string signalZ::X_H_L = string("X/H/L");
const string signalZ::H_L = string("H/L");
const string signalZ::LH = string("LH");
const string signalZ::XLH = string("XLH");
const string signalZ::ZERO_ONE = string("01");


// ------------------------------------------------------------------------

// Implement function definitions...

/**
 * IMPORTANT NOTE:
 * Since the character value '#' is supposed to represent incremental parameter
 * data replacememt in macros/procedures, I have assumed that it will append
 * the character/value '1' to existing values of the signal
 */

/**
 * Function to add the value "s" as the most recent values of the signal
 * @param s is the most recent values of the signal
 * @return nothing
 */
void signalZ::add_values(string s) {
	// Is this signal value a HASH?
	if(s=="#") {
		// Yes, check if this signal's condition is P?
		if(is_condition_p()) {
			// Its condition is indeed P, set its added value to its last pattern
			s=get_last_pattern();
		}else{
			// Else, set its added value to ONE
			s="1";
		}
	}

	if(s.substr(s.size()-1,1) == ";") {
		s.erase(s.size()-1,1);
	}

	sig=sig.insert(sig.size(),s);
}


/**
 * Function to add the value "c" as the most recent value of the signal
 * @param c is the most recent value of the signal
 * @return nothing
 * @deprecated
 */
void signalZ::add_char(char c) {
	/**
	 * The character "#" represents incremental parameter data replacement
	 * in macros/procedures
	 * Thus, if it is found, it is to be replaced with a incremental high
	 * signal
	 * I cannot process it with the last pattern since it is a character type
	 * and cannot store a string pattern
	 */
	if(c=='#') {
		c='1';
	}else if(c != ';') {
		string temp_c(1,c);
		sig=sig.insert(sig.size(),temp_c);
	}
}


/**
 * Function to append a string to the output file
 * @param current_ln is the current string to be appended as a line to
 *	the output file
 * @throws ViolatedAssertion exception when the signal's value or name is
 *	not properly initialized
 * @return nothing
 * O(1); COMPLETED
 */
void signalZ::print_output_ln(ofstream &outputfile) {
	if(sig_name==UNDEFINED) {
		throw ViolatedAssertion("Signal's name is improperly initialized");
	}
	
	if(sig==UNDEFINED) {
		throw ViolatedAssertion("Signal's value is improperly initialized");
	}

	// Left align the data in the output file
	outputfile.setf(ios::left);
	// Print its name...
	if((sig_name!=file_analyzer::MASTERCLK) && (sig_name!=file_analyzer::SCANOUT1)
		&& (sig_name!=file_analyzer::SCANOUT2)) {

		outputfile << sig_name << "\t\t";
	}else{
		outputfile << sig_name << "\t";
	}
	// Print its value...
	outputfile << sig << endl;
}



/**
 * Function to add a waveform/timing property to each signal in the list of
 * signals
 * @param w is the waveform/timing property that'll be added to this signal
 */
void signalZ::add_wave_prop(wave_prop w) {
	// Add the waveform/timing property to this signal
	wpl.push_back(w);
}



/**
 * Function to return the triggering condition of this signal
 */
string signalZ::get_trigger() {
	return trigger;
}



/**
 * Function to set the triggering condition of the signal...
 * @param t is the string indicating the trigger conditons
 */
void signalZ::set_trigger(string t) {
// Toggle the oddness/evenness of this signal
//toggle();

	trigger = t;
}


// Function to toggle between the odd/even signals
void signalZ::toggle() {
	if(sig_is_odd) {
		sig_is_odd=false;
	}else{
		sig_is_odd=true;
	}
}



/**
 * Function to determine if this signal is odd
 * @return true if signal is odd; else, return false
 */
bool signalZ::is_odd() {
	return sig_is_odd;
}


/**
 * Function to obtain the waveform/timing properties of the signal,
 * from a list of signals
 */
wp_list signalZ::get_wave_prop() {
	return wpl;
}


/**
 * Function to obtain the name of this signal
 * @return the name of this signal
 */
string signalZ::get_name() {
	return sig_name;
}


/**
 * Function to assign the condition for the signal to set its value from the
 * signal vector
 * @param c is the condition to set the signal's value
 * @return nothing
 */
void signalZ::set_condition(string c) {
	// Precondition
	if(c.size() != 1) {
		throw ViolatedPrecondition("Precondition is violated!");
	}

	// Precondition
	if((c!=ZERO) && (c!=ONE) && (c!=P) && (c!=X)) {
		throw ViolatedAssertion("The signal condition is invalid");
	}
	
	// If the last character of the pattern is not alphanumeric
	if(!isalnum(c[c.size()-1])) {
cout<<"c is:"<<c<<">:::"<<endl;
		// Drop the last character, which is not alphanumeric
		c.erase(c.size()-1,1);
cout<<"c IS NOw::"<<c<<"<:::"<<endl;
	}


	// If the condition is "X", ignore it... @deprecated
	sig_condition=c;
}


/**
 * Function to obtain the condition for the signal to set its value from the
 * signal vector
 * @return the condition to set the signal's value
 */
string signalZ::get_condition() {
	return sig_condition;
}


/**
 * Function to determine if the condition is met
 * @return true if the condition is met in its current value
 *	Else, return false
 */
bool signalZ::condition_met() {
cout<<"COndition pre-if statement"<<endl;
	// If the current value meets the condition...
	if((sig_condition==current_condition()) || (sig_condition==X)) {
cout<<"conditions are met"<<endl;
		// Condition is met; resume operation...
		return true;
	}else{
cout<<"conditions are NOT met"<<endl;
		// Else, condition is not met
		return false;
	}
}


/**
 * Function to get the most recent value
 * @return the most recent value, which is the last character in the signal's
 *	value
 */
string signalZ::current_condition() {
	// Are the signal values present?
	if(sig.size()>0) {
		// Yes
		return sig.substr(sig.size()-1,1);
	}else{
		// No
		return "";
	}
}



/**
 * Function to set the signal patterns that may be used with pattern insertion
 * @param s is the list of signal patterns that is to be assigned to this signal
 * @return nothing
 */
void signalZ::add_sig_patterns(string s) {
cout<<"s is THIS!!!"<<s<<"::::"<<endl;
	// If the last character of the pattern is alphanumeric
	if(!isalnum(s[s.size()-1])) {
cout<<"s is:"<<s<<">:::"<<endl;
		// Drop the last character, which is not alphanumeric
		s.erase(s.size()-1,1);
cout<<"s IS NOw::"<<s<<"<:::"<<endl;
	}

	s_p.push_back(s);
}



/**
 * Function to determine if the condition is equal to "P"
 * @return true if the condition is "P"; else, return false.
 */
bool signalZ::is_condition_p() {
cout<<"condition is P???"<< sig_condition <<endl;
	return (get_condition()==P);
}



/**
 * Function to check if list of signal patterns is empty
 * @return true if list of patterns is empty
 */
bool signalZ::is_pattern_list_empty() {
	return s_p.empty();
}



/**
 * Method to obtain the most recent signal pattern
 * @return the most recent signal pattern
 */
string signalZ::get_last_pattern() {
	if(is_pattern_list_empty()) {
		throw ViolatedAssertion("There are no string patterns to return");
	}
	
//	return s_p.back();
	/**
	 * The data structure used should be a queue...
	 * The first string added to the list of signal patterns should be used
	 * first
	 */
	
	// Obtain the first signal/string pattern
	string temp=s_p.front();
	// Remove the first signal/string pattern
	s_p.pop_front();
	// Return what was the previous first signal/string pattern
	return temp;
}


// Obtain the values of the signal vector
string signalZ::get_sig_values() {
	return sig;
}


// Get the number of assigned signal patterns to this signal vector
int signalZ::get_num_patterns() {
	return s_p.size();
}


// Get the assigned string patterns of this signal
str_list signalZ::get_sig_patterns() {
	return s_p;
}


/**
 * Function to convert the waveform properties into signal values
 * @deprecated
 */
void signalZ::convert_wave_prop() {
	// Pattern vector...
	string last_pattern;

	// Does the signal have any pattern?
	if(!is_pattern_list_empty()) {
		// Yes... Grab the most recent pattern and append it to the value
		last_pattern=s_p.back();
		
		// Append each logic value appropriately into the signal's value
		for(int j=0; j<last_pattern.size(); j++) {
			/**
			 * For each logic value, process it as intervals within a period
			 * That is, if a logic low/high value is encountered for a period,
			 * an appropriate number of intervals within that period is kept
			 * low/high
			 */
			 
//			for(int i=0; i<file_analyzer::period_t; i+=file_analyzer::MEASURE_OF_TIME) {
			for(int i=0; i<sig_period; i+=file_analyzer::MEASURE_OF_TIME) {
				// For this time interval, add its pattern value
				sig = sig + last_pattern[j];
			}
		}
	}
}


// Get the period of the signal
int signalZ::get_sig_period() {
	return sig_period;
}



// Set the period of the signal; @param i is the period
void signalZ::set_sig_period(int i) {
	sig_period=i;
}



/**
 * Function to process the waveform of the signal for a clock cycle
 * @param logic is the value of the signal for the clock cycle
 */
void signalZ::sig_waveform(string logic) {
	// Is its condition equal to "P"
	if((logic==HASH) && (get_num_patterns()>0)) {
cout<<"The logic is PPPPPPPPPPPPPPPPPPPPPP:"<<endl;
		sig = sig + get_last_pattern();
cout<<"Val of sig:"<<sig<<"::"<<endl;
		return;
	}else if((logic==HASH) && (get_num_patterns()<=0)) {
		return;
	}

	/**
	 * For each interval of the clock period, indicate the value of the signal
	 * This is because I have chosen to represent the logic of the signals,
	 * including the clocks, in terms of time intervals within a clock period
	 * This yields more accurate representation of the signal vectors
	 * However, the author notes that the length of the time interval used in
	 * the quantization of the signal vector was manually determined.
	 * Its value was a posteriori chosen based on the observation of the clock
	 * periods by the author...
	 * The author notes that the value should be chosen based on the automated
	 * parsing of the input text file. In the interest of completing this software
	 * promptly, the author did not provide another function to automatically
	 * parse the file once to determine the clock periods for the signals. The
	 * author also notes that this missing function would parse the input text
	 * file, or enumerate its parsed string tokens, an additional time, and
	 * would not add to the complexity of the software
	 */
	 
	/**
	 * The number of timing intervals is determined by the quotient of the
	 * clock period (dividend) and the length of each timing interval (the
	 * divisor)
	 * That is, number of intervals = clock period / length of timing interval
	 */
cout<<"The period is:"<<get_sig_period()<<":::"<<endl;
	// For each time interval in the clock period
	for(int a=0; a<get_sig_period(); a+=file_analyzer::MEASURE_OF_TIME) {
		// Append the logic value of to the signal vector
		sig = sig + logic;
//cout<<"incrementing SIG:"<<sig<<":::"<<endl;
	}
cout<<"val of SIG:"<<sig<<":::"<<endl;
}



/**
 * Function to process the waveform of the clock for one cycle
 * @param logic is the logic value of the clock for that cycle
 * IMPORTANT ASSUMPTION: Assume that when a clock signal has a condition of
 * "P", it will use the logic value of "1" as its condition... since it would
 * have no prior condition, or value to consider
 * I have chosen that over ground, since I can load a clock period as opposed
 * to a grounded logic value for a clock period
 */
void signalZ::clk_waveform(string logic) {
	// If this clock has no waveform properties...
	if(wpl.size() < 1) {
		// It cannot be a clock...
		cerr << "Clock signal name is:"<< get_name()<<"<:::"<< endl;
		throw ViolatedAssertion("Clock has missing waveform properties.");
	}

	// Current logic value of the signal...
	string current_val="";

	// Enumerate the waveform properties...
	wp_l_p wpp = wpl.begin();
	
	// If the waveform properties for this clock does not start at time 0...
	if((*wpp).time != ZERO) {
		// Waveform properties of the clock are invalid...
		cerr << "Clock signal name is:"<< get_name()<<"<:::"<< endl;
		throw ViolatedAssertion("Clock has invalid waveform properties.");
	}	

	// For each time interval in the clock period
	for(int a=0; a<get_sig_period(); a+=file_analyzer::MEASURE_OF_TIME) {
		// Is this signal value HIGH?
		if((logic == H) || (logic == ONE)) {
			/**
			 * Process the values of this clock signal according to its
			 * waveform properties
			 */
			
			
			/**
			 * Cumbersome workaround for a compiling error regarding the
			 * conversion from a string to a C string, which is a character
			 * sequence 
			 */
			current_val=(*wpp).time;

			// If this time checkpoint is not reached...
			if((a<atoi(current_val.c_str())) && (wpp != wpl.end())) {
				// Proceed to the next available waveform property
				wpp++;
				
				/**
				 * If the logic value for this waveform property is high
				 * or don't care
				 */
				if(((*wpp).logic_val == H) || ((*wpp).logic_val == ONE)) {
					
					// Append a high logic value to the signal vector
cout<<"Value of SiGNAL before adding ONE:"<<sig<<">>>"<<endl;
					sig = sig + ONE;
cout<<"Value of SiGNAL:"<<sig<<">>>"<<endl;
				// Is the logic value unknown?
				}else if((*wpp).logic_val == U) {
					// Yes, append a logic unknown value to it
cout<<"Value of SiGNAL before adding Z:"<<sig<<">>>"<<endl;
					sig = sig + Z;
cout<<"Value of SiGNAL:"<<sig<<">>>"<<endl;
				}else{
					/**
					 * Append a logic LOW value to the signal vector
					 * That is, if the signal has a logic value that is not
					 * high, low, unknown, or don't-care, assume that it will
					 * be treated as a logic low by default
					 */
cout<<"Value of SiGNAL before adding ZERO:"<<sig<<">>>"<<endl;
					sig = sig + ZERO;
cout<<"Value of SiGNAL:"<<sig<<">>>"<<endl;
				}
			}else{
				/**
				 * The last timing checkpoint has been defined...
				 * Append logic high values to the signal
				 */
cout<<"Timing properties of signal have been defined:"<<sig<<">>>"<<endl;
				sig = sig + ONE;
cout<<"New Value of SiGNAL:"<<sig<<">>>"<<endl;
			}
			
			
		}else if((logic == L) || (logic == ZERO)
			|| (logic == X) || (logic == U)) {
			
			/**
			 * Else, the value for this signal is either a logic LOW, in high
			 * impedance, or undefined...
			 * Such values shall remain constant throughout the clock cycle
			 * Add the logic values to the signal vector based on the number
			 * of timing intervals within the clock period/cycle
			 */

			// Append the logic value of to the signal vector
cout<<"Value of SiGNAL is ZERO/U/X:"<<sig<<">>>"<<endl;
			sig = sig + logic;
cout<<"LOW Value of SiGNAL:"<<sig<<">>>"<<endl;
		}else{
			// Logic value for the signal is in appropriate... Report error!
			cerr<<"Signal name is:"<<get_name()<<", and its logic value is:";
			cerr<<logic<<"..."<<endl;
			throw ViolatedAssertion("Logic value for this signal isn't valid");
		}
	}
}



/**
 * Function to set the logic for a signal vector, or set of signal values for
 * a signal
 * @param logic is the logic value for the signal
 * @return nothing
 */
void signalZ::sig_vector(string logic) {
	// Is this a hash character?
	if((logic != HASH) && (!isalnum(logic[logic.size()-1])) ) {
cout<<"logic is:"<<logic<<">:::"<<endl;
		// Drop the last character, which is not alphanumeric
		logic.erase(logic.size()-1,1);
cout<<"logic IS NOw::"<<logic<<"<:::"<<endl;
	}


	// Current enumerated value of the signal
	string current_sig_val="";
cout<<"Logic Val of Sig:::"<<logic<<":::"<<endl;
	// For each signal value in the signal vector
	for(int a=0; a<logic.size(); a++) {
		// Process this current signal value
		
		/**
		 * Assign the signal value to a temp variable
		 * Note that the signal value is always a single character
		 */
		current_sig_val = logic.substr(a,1);
		
		// Is this a clock signal?
		if(is_clk()) {
			/**
			 * Yes, process its clock waveform according to its waveform
			 * properties
			 */
cout<<"Set the val for clock:"<<current_sig_val<<">>>"<<endl;
			clk_waveform(current_sig_val);
		}else{
			/**
			 * This signal is not a clock signal... Process this non-clock
			 * signal accordingly
			 */
cout<<"Set the val for signal:"<<current_sig_val<<">>>"<<endl;
			sig_waveform(current_sig_val);
		}
	}
cout<<"------------------------------SigVecTOr: Has been processed"<<endl;
}



/**
 * Function to clear the waveform properties of this signal as another clock
 * signal waveform is loaded into it
 */
void signalZ::clr_waveform_properties() {
	wpl.clear();
}


/**
 * Function to determine if this signal is a clock
 * @return true if signal is a clock; else, signal is not a clock
 */
bool signalZ::is_clk() {
	if((get_name() == file_analyzer::SCANCLK)
		|| (get_name() == file_analyzer::MASTERCLK)) {
		
		return true;
	}else{
		return false;
	}
}