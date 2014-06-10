/**
 * This is written by Zhiyang Ong to parse a STIL file into a FLAT tabular
 * test pattern format that contains input test patterns for execution on
 * automatic test equipment
 *
 * Function to parse files for static analysis, and generation of output
 * file containing the test patterns for the automatic test equipment
 */

// Import Header files from the C++ STL
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <climits>
#include <cfloat>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <iterator>


#ifndef __SIGNAL_H
#define __SIGNAL_H
using namespace std;


// List of strings
typedef list<string> str_list;
// Pointer to list of strings
typedef list<string>::iterator str_l_p;


// =======================================================================

//class file_analyzer;

// Class definition...
class signalZ {
	public:
		// Defining constants...
		static const string UNDEFINED;
		static const string L;
		static const string H;
		static const string U;
		static const string ONE;
		static const string ZERO;
		static const string X;
		static const string Z;
		static const string P;
		static const string HASH;
		static const string D_U;
		static const string X_H_L;
		static const string H_L;
		static const string LH;
		static const string XLH;
		static const string ZERO_ONE;
		/**
		 * Number of cycles needed to let the signal settle, or reach steady state
		 * This value is arbitrarily chosen, and will be system/ATE dependent;
		 * if it is required
		 */
		static const int NCYCLES = 3;
		static const int LOGIC_ZERO = 0;
		static const int LOGIC_ONE = 1;
		static const int LOGIC_HIGH_IMPEDANCE = 2;
		static const int LOGIC_UNDEFINED = 3;
		
		
		// Default constructor
		signalZ();
		// Standard Constructor...
		signalZ(string name);
		// Define headers for functions...
		void add_values(string s);
		void add_char(char c);
		string get_name();
		void print_output_ln(ofstream &outputfile);
		
		
		// Declare data structure to contain its waveform properties
		struct wave_prop {
			string time;
			string logic_val;
		};
		
		// List of waveform/timing properties of the signal...
		typedef list<wave_prop> wp_list;
		// Pointer to list of waveform/timing properties
		typedef list<wave_prop>::iterator wp_l_p;


		// Add the waveform/timing properties of the signal to the list of signals
		void add_wave_prop(wave_prop w);
		// Obtain the waveform/timing properties of these signal
		wp_list get_wave_prop();
		// Obtain the triggering condition of the signal...
		string get_trigger();
		// Set the triggering condition of the signal...
		void set_trigger(string t);
		// Toggle between the odd/even signals
		void toggle();
		// Is this signal odd?
		bool is_odd();
		// Set the initial condition for the signal
		void set_condition(string c);
		// Get the initial condition for the signal
		string get_condition();
		// Is the condition met?
		bool condition_met();
		// Get the current condition
		string current_condition();
		
		// Add pattern to the list of signal patterns
		void add_sig_patterns(string s);
		// Is the condition equal to "P"?
		bool is_condition_p();
		// Check if list of signal patterns is empty
		bool is_pattern_list_empty();
		// Obtain the most recent signal pattern
		string get_last_pattern();
		
		// Obtain the values of the signal vector
		string get_sig_values();
		// Get the number of assigned signal patterns to this signal vector
		int get_num_patterns();
		// Get the assigned string patterns of this signal
		str_list get_sig_patterns();
		
		
		// Convert the waveform properties into signal values; Deprecated
		void convert_wave_prop();
		
		// Get the period of the signal
		int get_sig_period();
		// Set the period of the signal
		void set_sig_period(int i);
		/**
		 * Additional functions to handle waveform properties and logic values
		 * for the clock
		 */
		void clk_waveform(string logic);
		void sig_vector(string logic);
		void sig_waveform(string logic);
		/**
		 * Clear the waveform properties of this signal as another clock signal waveform
		 * is loaded into it
		 */
		void clr_waveform_properties();
		bool is_clk();
		
		
	private:
		// Declaration of instance variables...
		// Name of signal
		string sig_name;
		// Values of signal
		string sig;
		// Triggering condition of the signal
		string trigger;

		// List of waveform/timing properties of the signal...
		wp_list wpl;
		/**
		 * Indicator of whether it is an odd/even signal in the list of input
		 * or output signals
		 *
		 * If it is an odd signal, it is true.
		 * Else, it is false
		 *
		 * Since the waveform signals may be defined based on a pair of values,
		 * the flag is necessary to alternate between odd and even signals
		 */
		bool sig_is_odd;
		/**
		 * Condition of signal that must be met for the signal's value to be
		 * set from the signal vector
		 */
		string sig_condition;
		// List of signal patterns
		str_list s_p;
		// Period of the signal
		int sig_period;
};
#endif