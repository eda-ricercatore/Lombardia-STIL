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

#include "signalZ.h"

#ifndef __FILE_ANALYZER_H
#define __FILE_ANALYZER_H
using namespace std;

// =======================================================================

// List of strings
typedef list<string> str_list;
// Pointer to list of strings
typedef list<string>::iterator str_l_p;

// List of signalZs
typedef list<signalZ *> sig_list;
// Pointer to list of signalZs
typedef list<signalZ *>::iterator sig_l_p;


// Class definition...
class file_analyzer {
	private:
		// Declaration of instance variables...
		// Name of input file
		string in_file;
		// Name of output file
		string out_file;
		// Input stream for the appropriate input filename
		ifstream inputfile;
		// Output stream for the appropriate output filename
		ofstream outputfile;
		// List of strings tokens found in the input file
		str_list list_tokens;
		// Stack for temporary storage of parentheses in processing blocks
		str_list list_blocks;
		// Duration of the period for the selected clock (using UNIT_OF_TIME)
		int period_t;
		// List of input signals in the test pattern or output file
		sig_list list_of_ip_signals;
		// List of output signals in the test pattern or output file
		sig_list list_of_op_signals;
		// Currently enumerated signal in the test pattern
		signalZ *temp_sig;
		// List of string pattern to be substituted in the scanproc defintion
		str_list str_patterns;


		// --------------------------------------------------------------
		

	public:
		// Initialize constants...
		// Quantization unit of time that is measured in nanoseconds
		static const int file_analyzer::MEASURE_OF_TIME = 5;
		/**
		 * Flag to indicate that the index of this character in a string
		 * is invalid
		 */
		static const int INVALID_CHAR_INDEX = -9999999;
		// Assumed maximum number of characters per line
		static const int MAX_NUM_OF_CHAR_PER_LINE=1000;
		static const string INVALID;
		static const string ALLPINS;
		static const string BASEWFT;
		static const string MACRODEFS;
		static const string MACRO;
		static const string PATTERN;
		static const string PERIOD;
		static const string PROCEDURE;
		static const string SCANLOAD;
		static const string SCANUNLOAD;
		static const string SCANPROC;
		static const string SCANIN;
		static const string SCANOUT;
		static const string SCANOUT1;
		static const string SCANOUT2;
		static const string SCANCLK;
		static const string SIGNALGROUPS;
		static const string SHIFT;
		static const string TIMING;
		static const string MASTERCLK;
		static const string WAVEFORMTABLE;
		static const string WAVEFORMS;
		static const string V;
		static const string W;
		static const string C;
		static const string P;
		static const string HEADER;
		static const string PIPINS;
		static const string POPINS;
		static const string CLOCK;
		static const string OPEN_BRACKET;
		static const string CLOSE_BRACKET;
		static const string UNIT_OF_TIME;
		
		// =========================================================

		// Default constructor
		file_analyzer();
		
		// Standard Constructor...
		file_analyzer(string input_filename, string output_filename);
		
		// =========================================================
		
		// Define headers for functions...
		
		// Functions to parse blocks/segments of code
		// To be completed
		void parse_input();
		void process_sig_grps();
		void process_header();
		// To be completed
		void process_pattern_blk();
		// To be completed
		void process_macro_blk(string macro_name);
		// To be completed
		int process_scanproc_blk(string s);
		// To be completed
		int process_v_blk(str_l_p p, string vector_type);
		int process_waveform_blk(string wave);
		int process_pipins(str_l_p p);
		int process_popins(str_l_p p);
		// Used for master and scan clocks
		int process_clk(str_l_p p, string clk_type);
		void set_cond_or_val(string sig_name, string sig_val, string sig_type);
		void update_indv_sig(string sig_name, string sig_val, string sig_type);
		void scan_process();
		// Clear waveform properties of all signals
		void clear_wave_properties(string key);
		
		
		// Functions to handle input and output processing
		void print_output_ln(string current_ln);
		void close_io_streams();
		void dump_output();
		
		
		// Functions for string manipulation/operation
		bool search_key_found(string search_key, string cur_token);
		string int_to_str(int num);
// The following method is useless... It is defined/implemented, but not used
string time_to_string(int duration, int logic_value);
		str_l_p find_token(string key);
		string truncate_semicolon(string s);
		str_list delimit_string(string delimit_str);
		string chop_ns(string clk_time);
		string truncate_whitespace(string s);
		
		
		// Functions for inter-class communication
		void transmit_patterns(str_list signame_n_pattern);
		void update_sig_period(int period_of_sig);
		
		
		// Functions for functional verification
		void check_waveform();
		void check_signal_condition();
		void check_signal_value();
		void check_scan_block();
};
#endif
