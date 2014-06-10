/**
 * This is written by Zhiyang Ong to parse a STIL file into a FLAT tabular
 * test pattern format that contains input test patterns for execution on
 * automatic test equipment
 *
 * Function to parse files for static analysis, and generation of output
 * file containing the test patterns for the automatic test equipment
 *
 * IMPORTANT ASSUMPTIONS:
 * #Assume that the signals rise/fall at the rising edge of the selected clock
 * #Assume that the patterns for the clocks masterClk and scanClk are
 *	insignificant
 *	This is because the first waveform block can be processed before the
 *	pattern for its clock can be defined. Thus, the clock can simply be defined
 *	only by its waveform properties.
 *	That is, define patterns for signals that are not associated as clocks
 * #It is acceptable to have signal vectors, or input patterns to each signal,
 *	that are different in length.
 *	This is because they vary in input frequency, and the signal inputs differ
 *	in length.
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
#include "file_analyzer.h"
#include "ViolatedAssertion.h"
#include "ViolatedPostcondition.h"
#include "ViolatedPrecondition.h"

#include "signalZ.h"

using namespace std;

// =======================================================================

// Declaration of constants...
// Maximum number of characters per line in the input text file
//const int MAX_NUM_OF_CHAR_PER_LINE=1000;
// Common search keywords
const string file_analyzer::INVALID = string("INVALID");
const string file_analyzer::ALLPINS = string("allPins");
const string file_analyzer::BASEWFT = string("baseWFT");
const string file_analyzer::MACRODEFS = string("Macrodefs");
const string file_analyzer::MACRO = string("Macro");
const string file_analyzer::PATTERN = string("Pattern");
const string file_analyzer::PERIOD = string("Period");
const string file_analyzer::PROCEDURE = string("Procedures");
const string file_analyzer::SCANLOAD = string("SCANLOAD");
const string file_analyzer::SCANUNLOAD = string("SCANUNLOAD");
const string file_analyzer::SCANPROC = string("scanProc");
const string file_analyzer::SCANIN = string("scanIn");
const string file_analyzer::SCANOUT = string("scanOut");
const string file_analyzer::SCANOUT1 = string("scanOut1");
const string file_analyzer::SCANOUT2 = string("scanOut2");
const string file_analyzer::SCANCLK = string("scanClk");
const string file_analyzer::SIGNALGROUPS = string("SignalGroups");
const string file_analyzer::SHIFT = string("Shift");
const string file_analyzer::TIMING = string("Timing");
const string file_analyzer::MASTERCLK = string("masterClk");
const string file_analyzer::WAVEFORMTABLE = string("WaveformTable");
const string file_analyzer::WAVEFORMS = string("Waveforms");
const string file_analyzer::V = string("V");
const string file_analyzer::W = string("W");
const string file_analyzer::C = string("C");
const string file_analyzer::P = string("P");
const string file_analyzer::HEADER = string("Header");
const string file_analyzer::PIPINS = string("piPins");
const string file_analyzer::POPINS = string("poPins");
const string file_analyzer::CLOCK = string("Clock");
const string file_analyzer::OPEN_BRACKET = string("{");
const string file_analyzer::CLOSE_BRACKET = string("}");
//const int INVALID_CHAR_INDEX = -9999999;

//static const string file_analyzer::UNIT_OF_TIME = "ns";
const string file_analyzer::UNIT_OF_TIME = string("ns");


//static const int file_analyzer::MEASURE_OF_TIME = 5;
//const int file_analyzer::MEASURE_OF_TIME = 5;


/*
00798780.pdf - pg 64, symbol table
develop algor to seach for tokens within a field/BLOCK
skip token/block
determine number of I/O pins
for each input pin, place the input vector in some format
convert Ls and Hs to numbers... 1s and 0s
likewise for output pin
*/

// ==========================================================================

// Default constructor
file_analyzer::file_analyzer() {
	in_file=INVALID;
	out_file=INVALID;
	period_t=INVALID_CHAR_INDEX;
}

// Standard constructor
file_analyzer::file_analyzer(string input_filename, string output_filename) {	
	in_file=input_filename;
	out_file=output_filename;
	/**
	 * Convert input filename to string in C so that the file I/O function
	 * in the C++ library can be utilised for opening the input file
	 */
	inputfile.open(in_file.c_str());
	// Assertion to check if the input file exist
	if(inputfile == NULL) {
		cerr << "Input file, with the filename " << in_file
			<< ", does not exist!" << endl;
		throw ViolatedPrecondition("Invalid file name");
	}
	/**
	 * Convert output filename to string in C so that the file I/O function
	 * in the C++ library can be utilised for opening the output file
	 */
	outputfile.open(out_file.c_str());
	// Assertion to check if the output file was opened properly
	if(outputfile == NULL) {
		cout << "Output file, with the filename " << out_file
			<< ", does not exist!" << endl;
	}
}

// =======================================================================

// Implement function definitions...

/**
 * Function to parse the STIL file, abtract the test patterns from the
 * signals filed, and parse them into the output file
 * @param i_file is the input file in STIL format
 * @param o_file is the output file containing the test patterns
 */
void file_analyzer::parse_input() {
	// Parse the input file...
	
	/**
	 * Array of characters to contain data obtained from the first
	 * n characters of each line
	 */
	char temp_buffer[MAX_NUM_OF_CHAR_PER_LINE];
	// Store the currently processed token
	char *cur_token;
	/**
	 * C++ string representation of the search key to utilize functions from
	 * the C++ STL
	 */
	string key;

	// Container for tokens in currently enumerated line of the input file
	string cstr = "";
	// While there are any more lines in the text file to be read
	while(!inputfile.eof()) {
		// Read the line's first 1000 characters
		inputfile.getline(temp_buffer,MAX_NUM_OF_CHAR_PER_LINE);
		// Convert the C string into a C++ string
		cstr = temp_buffer;
		// If this string is not empty
		if(!cstr.empty()) {
			// Set whitespace as teh delimiter for tokens
			cur_token = strtok(temp_buffer, " ");
			
			
			// Acquire the first token...
			key = cur_token;
			// Insert the token into a list of string tokens
			list_tokens.push_back(key);

			cur_token = strtok(NULL, " ");
			while (cur_token != NULL) {
				// Get the current token as an integer from the char array				
				// Move to the next available token
				key = cur_token;
				// Insert this token into a list of string tokens
				list_tokens.push_back(key);
				
				// Attempt to get the next string token
				cur_token = strtok(NULL, " ");
				/**
				 * Do NOT do any processing after this line since it can be null
				 */
			}
		}
	}
cout << "Size of list is:::" << list_tokens.size() << endl;
/*
cout<<"========================================================="<<endl;
str_l_p t_p=list_tokens.begin();
while(t_p != list_tokens.end()) {
	cout<<"\t\t CURRENT TOKEN~~~"<<(*t_p)<<endl;
	t_p++;
}
cout<<"========================================================="<<endl;
*/


	/**
	 * cout << "Size of list is:::" << list_tokens.size() << endl;
	 * Used to verify that the reading of tokens from the input file, and the
	 * subsequent insertion of tokens into lists is correct
	 *
	 * Can also be verified with the following UNIX commands:
	 * more [name_of_input_file] | wc -w
	 */
	
	/**
	 * Print the header of the output file to indicate the day, month, date,
	 * and year in which the test patterns were created in the STIL file
	 */
	process_header();
	
	// Acquire the signals of the circuit/system under test
	process_sig_grps();
cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
cout<<"process pattern block"<<endl;
cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
	/**
	 * Process the pattern block next, since this program is supposed to
	 * generate test patterns
	 */
	process_pattern_blk();
cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
cout<<"Dump the output to an output file..."<<endl;
	dump_output();
	// Close the input and output file streams...
	close_io_streams();
}



/**
 * Function to append a string to the output file
 * @param current_ln is the current string to be appended as a line to
 *	the output file
 * @return nothing
 * O(1); COMPLETED
 */
void file_analyzer::print_output_ln(string current_ln) {
	// Left align the data in the output file
	outputfile.setf(ios::left);
	outputfile << current_ln << endl;
}



/**
 * Function to close the input and output filestreams
 * O(1); COMPLETED
 */
void file_analyzer::close_io_streams() {
	// Close the input filestream
	inputfile.close();
	// Close the output filestream
	outputfile.close();
}


/**
 * Function to search for the key within this given line
 * @param search_key is the key to be searched for in this token
 * @param cur_token is the token being searched
 * @return true if search_key is found in cur_token
 *	else, return false
 * O(1); COMPLETED
 */
bool file_analyzer::search_key_found(string search_key, string cur_token) {
	if(cur_token.find(search_key) == string::npos) {
		return false;
	}else{
		return true;
	}
}



/**
 * Function to place each token in the input file into a list of strings to
 * facilitate manipulation/enumeration of the tokens
 * @return a list of tokens, which are represented as strings
 * O(n); COMPLETED
 */
str_l_p file_analyzer::find_token(string key) {
//cout << "\t\t KEY" << key << "!!!" << endl;
	// Enumerate the list of tokens from the beginning
	str_l_p p = list_tokens.begin();
	// While the last token has not been processed...
	while(p != list_tokens.end()) {
		if((*p) == key) {
//cout << "\t\t search key IS FOUND" << (*p) << endl;
			return p;
		}
		// Enumerate the next token...
		p++;
//cout << "find" << (*p) << "$$$" << endl;
	}
//cout << "\t\t Leave the field" << (*p) << endl;
	return list_tokens.end();
}


/**
 * Function to process the Pattern block
 * @return nothing
 */
void file_analyzer::process_pattern_blk() {
cout<<">>>>>>>>>>>>>>>>>>>>>>void file_analyzer::process_pattern_blk()"<<endl;
	/**
	 * Counter to indicate the number of tokens being processed
	 * Hence, they indicate the number of tokens that shall be skipped
	 * after a method/function call
	 * This avoids the need to keep track of the pointer location, upon the
	 * return of the method/function call
	 */
	int num_tkns=0;

	// Search for the keyword "Pattern"...
	str_l_p ptn = find_token(PATTERN);
cout<<"\t\tPATTERN\t\tFOUND\t\tPATTERN\t\tFOUND"<<endl;
	// Assertion...
	if(ptn == list_tokens.end()) {
		cerr<<"Keyword "<<PATTERN<<" is not found!"<<endl;
		throw ViolatedAssertion("Wrong keyword is used.");
	}
	
	// Pattern block has been located...
	
/*
Example of how to search for a string and access its neighbours	
	ptn++;
	ptn++;
	ptn++;
	if(search_key_found((*ptn), MACRO)) {
		cout << "find works" << endl;
	}else{
		cout << "fix the bug in find" << endl;
	}
*/	
	/**
	 * Process this Pattern block...
	 * Search for its open bracket...
	 */
	while((*ptn) != OPEN_BRACKET) {
		ptn++;
	}	// Open curly bracket is found...
	
	/**
	 * Open bracket is not added to simplify the processing of the block
	 * Just check if the list of parentheses is empty, instead of its size (1)
	 * and its content (open curly bracket)
	 */
	
cout << "!@#$%^&*()_+Str found is:::" << (*ptn) << endl;
	/**
	 * While the next token isn't a close bracket that ends this block...
	 * Process the contents of the pattern block
	 * The second condition of having the pointer not assigned to the begining
	 * of the list of tokens is an important terminating condition for this
	 * iterative procedure, since it had been found to loop back to the initial
	 * token of the list
	 */
	while((ptn != list_tokens.end()) && (ptn != list_tokens.begin())) {
//	while(ptn != list_tokens.end()) {
cout<<"My\tCurrent\tToken\tis=="<<(*ptn)<<"::::############"<<endl;
		// If this token is a close bracket that closes this block...
		if(search_key_found(CLOSE_BRACKET,(*ptn)) && (list_blocks.empty()) ) {
cout<<"CLOSE BRACKET#################################"<<endl;
cout << "Close Bracket Token is found===" << (*ptn) << endl;
cout<<"\t\tTOKEN\t\tFOUND\t\tTOKEN\t\tFOUND"<<endl;
			// End of Pattern block...
			break;
		}else if((*ptn)==MACRO) {
cout<<"MACRO#################################"<<endl;
			// Macro keyword is found... Process it
			
			// Get the pointer to the name of the macro...
			ptn++;
cout << "Macro Token is found===" << (*ptn)<<"><"<< endl;
			str_list sl = delimit_string(*ptn);
cout << "Macro TokenIZED===" << sl.front()<<"><"<< endl;
			// Process this macro definition
//			ptn=process_macro_blk(sl.front());
			process_macro_blk(sl.front());
cout<<"\t\tMACRO\t\tFOUND\t\tMACRO\t\tMACRO"<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<"--Macro has been processed --"<<endl;
cout<<"--Macro has been processed --"<<endl;
cout<<"--Macro has been processed --"<<endl;
cout<<"--Macro has been processed --"<<endl;
cout<<"--Macro has been processed --"<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
// BEGIN DEBUG HERE
// Skip to the next valid token; skip 3 tokens
//break;
		}else if(truncate_semicolon(*ptn)==SCANLOAD) {
cout<<"SCAN LOAD#################################"<<endl;
// RESUME DEBUGGING HERE... till the end of the function

			// scanProc keyword is found... Process it
			num_tkns=process_scanproc_blk(*ptn);
//			num_tkns++;
//			num_tkns++;
cout<<"NUm Tokens is:::"<<num_tkns<<endl;
cout<<"\t\tSCANLOAD\t\tFOUND\t\tSCANLOAD\t\tFOUND"<<endl;
cout<<"((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((("<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<"((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((("<<endl;
//check_scan_block();
		}else if(truncate_semicolon(*ptn)==SCANUNLOAD) {
cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
cout<<"SCAN UNLOAD#################################"<<endl;
// RESUME DEBUGGING HERE... till the end of the function

			// scanProc keyword is found... Process it
			num_tkns=process_scanproc_blk(*ptn);
			num_tkns++;
			num_tkns++;
cout<<"NUm Tokens is:::"<<num_tkns<<endl;
cout<<"\t\tSCANUNLOAD\t\tFOUND\t\tSCANUNLOAD\t\tFOUND"<<endl;
//check_scan_block();
		}else if((*ptn)==V) {
cout<<"VVVVVVV#####################::::"<<(*ptn)<<"::::############"<<endl;
			// V keyword is found... Process it
num_tkns=process_v_blk(ptn,V);
num_tkns=num_tkns*2;
num_tkns--;
num_tkns--;
//ptn--;
cout<<"\t\tVVV\t\tFOUND\t\tVVV\t\tFOUND"<<endl;
check_signal_condition();
check_signal_value();
		}
		
		// Skip the required number of tokens to process the next subpattern
		while(num_tkns>0) {
			num_tkns--;
cout<<"<<<\t<<<\t<<<\t<<< CURRENT TOKEN"<<(*ptn)<<"><"<<endl;
			ptn++;
		}
		
		// If all the tokens have been processed...
		if(ptn == list_tokens.end()) {
			// Exit...
			break;
		}
		
cout<<"<<<\t<<<\t<<<\t<<< NEXT TOKEN"<<(*ptn)<<"><"<<endl;
		// Else, process the next token...
		ptn++;
	}	// Pattern block has been processed
cout << "-------->>>>PATTERN BLOCK HAS BEEN PROCESSED" << endl;
cout<<"<<<<<<<<<<<<<<<<<<<<<<void file_analyzer::process_pattern_blk()"<<endl;
}



/**
 * Function to process the macro block
 * Since a call to process the macro definition is made, I do not have to
 * keep track of where did the macro definition end
 * That is, I also do not need to keep track of how many tokens have been
 * traversed in my macro definition
 * Consequently, I do not need to return a pointer to the last traversed
 * element
 * @return nothing  
 */
void file_analyzer::process_macro_blk(string macro_name) {
cout<<">>>>>>>>>void file_analyzer::process_macro_blk(string macro_name)"<<endl;
	/**
	 * Counter to indicate the number of tokens being processed
	 * Hence, they indicate the number of tokens that shall be skipped
	 * after a method/function call
	 * This avoids the need to keep track of the pointer location, upon the
	 * return of the method/function call
	 */
	int num_tkns=0;

cout << "\t MARCRONAME" << macro_name <<"*************************"<< endl;
	// Search for this macro definition 
	str_l_p  m_p = find_token(macro_name);
	
	// Assertion...
	if(m_p == list_tokens.end()) {
		cerr<<"Keyword "<<macro_name<<" is not found!"<<endl;
		throw ViolatedAssertion("Wrong keyword is used.");
	}
	
	// Determine the initial number of parentheses encasing the blocks...
	int num_brackets = list_blocks.size();
	// Proceed to the first token after the open curly bracket
cout << "1TOKEN IS@@@" << (*m_p) << endl;
	m_p++;
cout << "2TOKEN IS@@@" << (*m_p) << endl;
	m_p++;
cout << "3TOKEN IS@@@" << (*m_p) << endl;
	// Process this macro definition...
	while(m_p != list_tokens.end()) {
		// If this token is a close bracket that closes this block...
		if(search_key_found(CLOSE_BRACKET,(*m_p)) && (list_blocks.size()==num_brackets) ) {
		//if(search_key_found(CLOSE_BRACKET,(*m_p))) {
cout<<"END OF PATTERN BLOCK------>>>>>>>END OF PATTERN BLOCK"<<endl;
cout<<"<<<<<<<<<void file_analyzer::process_macro_blk(string macro_name)"<<endl;
			// End of Pattern block...
			return;
//		}else if((*m_p)==W) {
		}else if(search_key_found(W,(*m_p)) ) {
			// THE SIGNAL W is found... Process it
			m_p++;
			// Get the name of the macro...
			str_list sl = delimit_string(*m_p);
cout << "W !!TokenIZED===------>>>>>>------>>>>>>" << sl.front() << endl;
			// Process this macro definition
num_tkns=process_waveform_blk(sl.front());
num_tkns=0;
//==================================================================
//==================================================================
//==================================================================
//==================================================================
//==================================================================

// BEGIN DEBUG HERE

//==================================================================
//==================================================================
//==================================================================
// UNcomment this when the function process_v_blk has been defined
//break;
//		}else if((*m_p)==V) {

// $DISPLAY Waveform properties...
check_waveform();

cout<<"************************COMPLETE waVeFrOm prOcEsSInG"<<endl;
		}else if(search_key_found(V,(*m_p)) ) {
			// scanProc keyword is found... Process it
cout<<"~~~~~~~~~~~~~~~~~~~~~~V block is found"<<endl;
num_tkns=process_v_blk(m_p,V);
// Number of tokens exceeded count by 1
cout << "V !!TokenIZED===------>>>>>>------>>>>>>" << endl;
check_signal_condition();
check_signal_value();
//		}else if((*m_p)==C) {
		}else if(search_key_found(C,(*m_p)) ) {
			// V keyword is found... Process it
cout<<"~~~~~~~~~~~~~~~~~~~~~~C block is found"<<endl;
num_tkns=process_v_blk(m_p,C);
cout << "C !!TokenIZED===------>>>>>>------>>>>>>" << endl;
num_tkns--;
check_signal_condition();
check_signal_value();
		}
// Handle the blocks here...
		if(search_key_found(OPEN_BRACKET,(*m_p))) {
			list_blocks.push_back(*m_p);
cout<<"~~~~~~~~~~~~~~~~~~~~~~OPEN bracket is found"<<endl;
		}else if(search_key_found(CLOSE_BRACKET,(*m_p))) {
			list_blocks.pop_back();
cout<<"~~~~~~~~~~~~~~~~~~~~~~CLOSE bracket is found"<<endl;
		}
cout<<"\t\t\t\t\t Process next waveform tokeee"<<endl;
cout<<"THE NUMBER OF TOKENS that are skipped in this loop are<>"<<num_tkns<<"<>"<<endl;
		// Skip the processed tokens... move to the next unprocessed token
		while(num_tkns > 0) {
			num_tkns--;
			m_p++;
cout<<"SKIPPED TOKEN IS:::::::::::::::::::::::::::"<<(*m_p)<<"<>"<<endl;
		}
		
		// Else, process the next token...
		m_p++;
		
		if(search_key_found(PATTERN,(*m_p))) {
			cout<<"END OF PATTERN BLOCK------>>>>>>>END OF PATTERN BLOCK"<<endl;
cout<<"<<<<<<<<<void file_analyzer::process_macro_blk(string macro_name)"<<endl;
			// End of Pattern block...
			return;
		}
		
cout<<"\t\t\t\t\t Processed next "<<(*m_p)<<" waveform tokeee"<<endl;
cout<<"\t\t\t\t\t xxxxxxxxxxxxxxxxxxxxx\tNext Token\tDOnE"<<endl;
	}	// macro block has been processed
	
cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<process macro blk"<<endl;
	string err_msg="Close bracket for macro definition CANNOT BE FOUND!!!";
// UNCOMMENT THIS AFTER THE METHOD DEFINTIONS FOR C AND V
//	throw ViolatedAssertion(err_msg);
}






/**
 * Function to delimit a string from non-alphanumeric characters
 *
 * When processing the structure for the waveforms, do not process the signal
 * values... Only create its triggering and wave properties.
 *
 * @param delimit_str is the string to be delimited
 * @return the delimited string
 * O(n) computational time complexity; COMPLETED
 */
int file_analyzer::process_waveform_blk(string wave) {
cout<<">>>>>>>>>>>>int file_analyzer::process_waveform_blk(string wave)"<<endl;
	
	/**
	 * Counter to indicate the number of tokens being processed by another
	 * method call
	 * Hence, they indicate the number of tokens that shall be skipped
	 * after a method/function call
	 * This avoids the need to keep track of the pointer location, upon the
	 * return of the method/function call
	 */
	int num_tokens_traversed=0;
	 
	
	// Number of tokens traverse by this function...
	int num_tkns=0;
	
	
	// Search for the pointer to the keyword
	str_l_p w = find_token(wave);
	
	// Assertion...
	if(w == list_tokens.end()) {
		cerr<<"Keyword "<<wave<<" is not found!"<<endl;
		throw ViolatedAssertion("Wrong keyword is used.");
	}
	
	// Determine the initial number of parentheses encasing the blocks...
	int num_brackets = list_blocks.size();
	// Proceed to the first token after the open curly bracket
cout << "1TOKEN" << (*w) << endl;
	w++;
	num_tkns++;
cout << "2TOKEN" << (*w) << endl;
	w++;
	num_tkns++;
cout << "3TOKEN" << (*w) << endl;
	// Process this macro definition...
	while(w != list_tokens.end()) {
		// If this token is a close bracket that closes this block...
		if(search_key_found(CLOSE_BRACKET,(*w)) && (list_blocks.size()==num_brackets) ) {
cout<<"<<<<<<<<<<<<int file_analyzer::process_waveform_blk(string wave)"<<endl;
			// End of Pattern block...
			return num_tkns;
		}else if(search_key_found(CLOSE_BRACKET,(*w))) {
cout<<"Reached definition of next set of waveform properties"<<endl;
cout<<"<<<<<<<<<<<<int file_analyzer::process_waveform_blk(string wave)"<<endl;
			// End of Pattern block...
			return num_tkns;
//		}else if((*w)==PERIOD) {
		}else if(search_key_found(PERIOD,(*w)) ) {
			// The period of the waveform is found... Process it
			w++;
			num_tkns++;
			// Get the value of the period...
			str_list sl = delimit_string(*w);

			string cycle_time = sl.front();
			cycle_time=chop_ns(cycle_time);
cout << "Cycle time is&&&" << cycle_time << endl;

			// Assign the period of the waveform...
			period_t = atoi(cycle_time.c_str());
			update_sig_period(period_t);
cout << "\t\t\tNEW Cycle time is&&&" << period_t << endl;
			// Skip 2 tokens to deal with signal values
			w++;
			num_tkns++;
cout << "single&&incre" << (*w) << endl;
			w++;
			num_tkns++;
cout << "double&&incre" << (*w) << endl;
//cout << "\t\t\t Uh Oh!!!"<<endl;
		// Completed...
//		}else if((*w)==PIPINS) {
		}else if(search_key_found(PIPINS,(*w)) ) {
			// PIPINS keyword is found... Process it
cout<<"\t\t\tProcess INPUT signal waveform properties"<<endl;
			num_tokens_traversed=process_pipins(w);
num_tokens_traversed++;
cout<<"\t\t\tProcessED INput signal waveform properties"<<endl;
//break;
//		}else if((*w)==POPINS) {
		}else if(search_key_found(POPINS,(*w)) ) {
			// POPINS keyword is found... Process it
cout<<"\t\t\tProcess output signal waveform properties"<<endl;
			num_tokens_traversed=process_popins(w);
num_tokens_traversed++;
cout<<"\t\t\t!!!Processed output signal waveform properties"<<endl;
//			process_v_blk();
//break;
//		}else if((*w)==MASTERCLK) {
		}else if(search_key_found(MASTERCLK,(*w)) ) {
			// MASTERCLK keyword is found... Process it
//			process_v_blk();
cout<<"\t\t\t\t\t Get dEtAIls of tHE mAsTEr cLoCK"<<endl;
			num_tokens_traversed=process_clk(w,MASTERCLK);
num_tokens_traversed++;
num_tokens_traversed++;
num_tokens_traversed++;
cout<<"\t\t\t\t\t mAsTEr cLoCK hAs BeEN pRoCeSSeD"<<endl;
//break;
//		}else if((*w)==SCANCLK) {
		}else if(search_key_found(SCANCLK,(*w)) ) {
			// SCANCLK keyword is found... Process it
//			process_v_blk();
cout<<"\t\t\t\t\t Get dEtAIls of tHE SCAN cLoCK"<<endl;
			num_tokens_traversed=process_clk(w,SCANCLK);
cout<<"\t\t\t\t\t SCAN cLoCK hAs BeEN pRoCeSSeD"<<endl;
//break;
num_tokens_traversed++;
num_tokens_traversed++;
num_tokens_traversed++;
		}
		
		if(search_key_found(OPEN_BRACKET,(*w))) {
			list_blocks.push_back(*w);
		}else if(search_key_found(CLOSE_BRACKET,(*w))) {
			list_blocks.pop_back();
		}
		
		
		// Skip all tokens that have already been processed
		while(num_tokens_traversed>0) {
			num_tokens_traversed--;
			w++;
			num_tkns++;
cout<<"Token skipped:"<< (*w) <<"><=="<<endl;
		}
//		w++;
		
		// Else, process the next token...
cout<<"\t\t\t\tyet to crash dump"<<endl;
		w++;
		num_tkns++;
cout<<"Tokie WWWis:"<<(*w)<<endl;
cout<<"\t\t\t\t crash dump???"<<endl;
	}	// macro block has been processed
	
cout<<"ENDmacroBlK"<<endl;
string err_str;
err_str="Macro definition is not properly defined: missing close brackets";
throw ViolatedAssertion(err_str);
}



/**
 * Function to delimit a string from non-alphanumeric characters
 * @param delimit_str is the string to be delimited
 * @return the delimited string as a list of string tokens
 * O(n); COMPLETED
 */
str_list file_analyzer::delimit_string(string delimit_str) {
cout<<">>>>>>>>>>>>>>>str_list file_analyzer::delimit_string(string delimit_str)"<<endl;
	// Storage of delimited strings containing values to be processed
	str_list list_str;
	// Temporary string to process current token...
	string temp;
	// Pointer to initial character of current string
	int initial_char=0;
	/**
	 * Flag to indicate that a string commencing with a non-alphanumeric
	 * character can process its first substring/token with only alphanumeric
	 * characters
	 */
	bool initial_invalid_token=false;
	// Flag indicating if subsequent valid tokens can be processed
	bool process_tokens = false;
	
	// Indicator of whether matching apostrophes are found
	int num_apostrophes=0;

	// For each character in the string...
	for (int i=0; i < delimit_str.size(); i++) {
		// Are there any alphanumeric characters?
		if (!isalnum(delimit_str[i])) {
			// This current character is not alphanumeric
			// If this substring only contains alphanumeric characters...
			if((initial_char != INVALID_CHAR_INDEX) && (i>0) ) {
				// Obtain current substring...
				temp = delimit_str.substr(initial_char,(i-initial_char));
				// Reassign the initial string for the next string token later...
				initial_char = INVALID_CHAR_INDEX;
				
				
				// Process substrings containing only alphanumeric characters?
				if(initial_invalid_token) {
					process_tokens=true;
				}
				
				
				/**
				 * Has the substring containing only non-alphanumeric characters
				 * that commence the string been processed?
				 */
				if((!isalnum(delimit_str[0])) && (list_str.empty()) 
					&& (!initial_invalid_token)) {
					// No...
					initial_invalid_token=true;
				}
				
				
				// Is this a valid alphanumeric token to be added to the set?
				if(process_tokens || (isalnum(delimit_str[0]))) {
					// Yes. Add this string to the list of string tokens
					list_str.push_back(temp);
				}
			}

		}else{
			// Else, this is an alphanumeric character
			
			/**
			 * Is this part of a substring that only contains alphanumeric
			 * characters?
			 */
			if(initial_char==INVALID_CHAR_INDEX) {
				/**
				 * No.
				 * Reassign initial character for next substring that only
				 * contains alphanumeric characters...
				 */
				initial_char=i;
			}
		}
		
		// Delete apostrophes from the string...
		if(delimit_str[i] == '\'') {
			num_apostrophes++;
			delimit_str.erase(i,1);
			i--;
		}
	}


	// Process substrings that have an alphanumeric last character...
	// Obtain current substring
	if(isalnum(delimit_str[(delimit_str.size()-1)]) 
		&& (initial_char != INVALID_CHAR_INDEX)) {
		temp = delimit_str.substr(initial_char,(delimit_str.size()-initial_char));

		// Add this string to the list of string tokens
		list_str.push_back(temp);
	}

	// Do the apostrophes match?
	if((num_apostrophes%2) != 0) {
		throw ViolatedAssertion("Matching apostrophes NOT found!!!");
	}
cout<<"<<<<<<<<<<<<<<<str_list file_analyzer::delimit_string(string delimit_str)"<<endl;
	// Return the list of delimited tokens...
	return list_str;
}



/**
 * Function to convert the signal value that's "duration" long (in time units)
 * @param duration is the length in time that this phase of the signal last
 * @param logic_value is the boolean logic value that a signal should take
 * @return a string indicating a sequence of quantized values for this signal
 * i.e., a signal of 30ns duration is broken up into a sequence of values in
 * the defined quantized units of time... "MEASURE_OF_TIME" amount of time in
 * UNIT_OF_TIME units of time
 * O(n) computational time complexity; COMPLETED
 */
string file_analyzer::time_to_string(int duration, int logic_value) {
	// Can the duration be represented as quantized units of time
	if((duration%MEASURE_OF_TIME) != 0) {
		string err="The duration cannot be measured in quantized units of time";
		throw ViolatedPrecondition(err);
	}
	
	// String indicating the logic values of the signal for the duration
	string signal_values="";
	// Logic value during a selected 5ns interval
	string logic_v=int_to_str(logic_value);
	for(int i=0; i<duration; i=i+MEASURE_OF_TIME) {
		//signal_values=signal_values+""+logic_value;
		signal_values=signal_values.insert(signal_values.size(),logic_v);
	}
	
	return signal_values;
}




/**
 * Function to convert an integer into a string
 * @param num is an integer to be converted into a string
 * @return a string representation of the integer
 * O(n) computational time complexity; COMPLETED
 */
string file_analyzer::int_to_str(int num) {
	// String representing this integer, num
	string i_to_s = "";
	
	// Remainder from the modulo of 10
	int modulus;
	// If this number is greater than the modulus 10
	if(num>9) {
		// Obtain the remainder...
		modulus = num%10;
		// Add its remainder as the most significant digit
		i_to_s=i_to_s.insert(0,int_to_str(modulus));
		// Get the next most significant digit
		return i_to_s.insert(0,int_to_str(num/10));
	}else{
		// The remainder is equal to the number...
		modulus = num;
		// Insert the lowest order digits...
		switch(modulus) {
			case 0:
				return i_to_s.insert(0,"0");
				break;
			case 1:
				return i_to_s.insert(0,"1");
				break;
			case 2:
				return i_to_s.insert(0,"2");
				break;
			case 3:
				return i_to_s.insert(0,"3");
				break;
			case 4:
				return i_to_s.insert(0,"4");
				break;
			case 5:
				return i_to_s.insert(0,"5");
				break;
			case 6:
				return i_to_s.insert(0,"6");
				break;
			case 7:
				return i_to_s.insert(0,"7");
				break;
			case 8:
				return i_to_s.insert(0,"8");
				break;
			case 9:
				return i_to_s.insert(0,"9");
				break;
			default:
				cerr << "Remainder of modulo should be a digit!" << endl;
				throw ViolatedAssertion("Digit NOT found!!!");
		}
	}
}



/**
 * Function to drop the units of time from the cycle time
 * @param clk_time is the cycle time
 * @return the cycle time without its units
 * O(1) computational time complexity; COMPLETED
 */
string file_analyzer::chop_ns(string clk_time) {
	// Remove its last two characters
	return clk_time.erase(clk_time.size()-UNIT_OF_TIME.size(),2);
}



/**
 * Function to process the input pins of the module under test
 * This is included in the waveform block
 * @param p is the pointer to start of this block...
 * @return the number of tokens enumerated by this token
 * O(n) computational time complexity; COMPLETED
 */
int file_analyzer::process_pipins(str_l_p p) {
cout<<">>>>>>>>>>>>>>>>>>>>>>int file_analyzer::process_pipins(str_l_p p)"<<endl;
	/**
	 * Flush the current waveform properties to load the new waveform properties
	 * of this signal or set of signals
	 * This obviates the need to clear the waveform block each time before this
	 * function is called.
	 */
	clear_wave_properties(PIPINS);
	
	
	
	
	// Count the number of tokens that have been enumerated in the block
	int num_tokens=0;
	// Temporary holder used for delimiting tokens
	string temp_holder;
	// List returned from delimiting string token
	str_list t_l_p;
//cout<<"list_blocks.size()"<<list_blocks.size()<<endl;
	// Temporary storage of wave properties for currently enumerated signals
	signalZ::wave_prop ww;
	// Determine the initial number of parentheses encasing the blocks...
//	int num_brackets = list_blocks.size();
	// Proceed to the first token after the open curly bracket

	// Get the first triggering conditions of the signal...
	p++;
cout<<"1st move current numToKEN:"<<(*p)<<"<:::"<<endl;
	num_tokens++;
	p++;
cout<<"2nd move current numToKEN:"<<(*p)<<"<:::"<<endl;
	num_tokens++;
	// Truncate whitespace...
	temp_holder = truncate_whitespace(*p);

	// Assign this trigger to all input pins...
	// Get the pointer to the first input signal
	sig_l_p ip_p = list_of_ip_signals.begin();
	// Flag to determine if this signal should be even...
	int even_or_odd=1;
	
	// For each input signal in the system/circuit
	while(ip_p != list_of_ip_signals.end()) {
		// If signal is even, set it to be so...
		if((even_or_odd%2)==0) {
			(*ip_p)->toggle();
cout<<"Signal is even"<<endl;
		}else{
cout<<"Signal is odd"<<endl;
		}
	
		// Assign trigger to currently enumerated input signal
		(*ip_p)->set_trigger(temp_holder);
cout<<"------- Set the trigger to be:"<<temp_holder<<": for the signal name:"<<(*ip_p)->get_name()<<":><"<<endl;
		// Proceed to the next input signal
		ip_p++;
		even_or_odd++;
	}
	
	
	// Get the first waveform property of the signal...
	p++;
	num_tokens++;
	p++;
	num_tokens++;
	

	// While there are more available input signals to enumerate...
	while(p != list_tokens.end()) {
		// If this token is a close bracket that closes this block...
		//if(search_key_found(CLOSE_BRACKET,(*p)) && (list_blocks.size()==num_brackets) ) {
		if(search_key_found(CLOSE_BRACKET,(*p))) {
cout<<"End of while >>><<< loop"<<list_blocks.size()<<endl;
cout<<"<<<<<<<<<<<<<<<<<<<<<<int file_analyzer::process_pipins(str_l_p p)"<<endl;
			// End of Pattern block...
			return num_tokens;
		}else{
			/**
			 * Do it for each input signal, and set their time and logic value
			 * for its wave properties
			 */

			// Truncate whitespace of signal's waveform property (event/time)
			temp_holder = truncate_whitespace(*p);
			// Delimit the string for non-alphanumeric characters...
			t_l_p=delimit_string(temp_holder);
			// Chop its units of time...
			temp_holder=t_l_p.front();
			temp_holder=chop_ns(temp_holder);
			// Add it to the time property of these signals' waveform
			ww.time=temp_holder;
			// And push it into list_blocks...
//			list_blocks.push_back(temp_holder);
cout<<"1QWERTY:::"<< temp_holder << endl;
			// Get its corresponding value...
			p++;
			num_tokens++;
			// Truncate whitespace of signal's waveform property (value)
			temp_holder = truncate_whitespace(*p);
			// Remove its semi-colon suffix...
			temp_holder=truncate_semicolon(temp_holder);
			// Add it to the logic value for these signals' waveform property
			ww.logic_val=temp_holder;
			// And push it into list_blocks...
//			list_blocks.push_back(temp_holder);
			
			
			// Add this waveform property to all input signals
			// Get the pointer to the first input signal
			ip_p = list_of_ip_signals.begin();
			// For each input signal in the system/circuit
			while(ip_p != list_of_ip_signals.end()) {
				// Assign waveform property to currently enumerated input signal
				(*ip_p)->add_wave_prop(ww);
				// Proceed to the next input signal
				ip_p++;
			}
		
				
cout<<"2QWERTY:::"<< temp_holder << endl;
		}
		
		// Process the next token...
		p++;
		num_tokens++;
	}
cout<<"End of while loop>>><<<"<<endl;
}


/**
 * Function to place the signals of the circuit into their appropriate groups
 * The groups are for input signals and output signals
 * This implementation (release in April 2007) does not provide coverage of
 * bidirectional signals
 * O(n) traverse/enumerate the list to access the right block of signal
 * definitions, and to process each signal that is found.
 * COMPLETED
 */
void file_analyzer::process_sig_grps() {
cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>void file_analyzer::process_sig_grps()"<<endl;
	// Find the groups of signals
	str_l_p  w = find_token(SIGNALGROUPS);
	
	// Assertion...
	if(w == list_tokens.end()) {
		cerr<<"Keyword "<<SIGNALGROUPS<<" is not found!"<<endl;
		throw ViolatedAssertion("Wrong keyword is used.");
	}
	
	// Determine the initial number of parentheses encasing the blocks...
	int num_brackets = list_blocks.size();
	// Proceed to the first token after the open curly bracket

	// Skip the keyword for the group of signals...
	w++;
	// Skip the opening parenthesis for the block...
	w++;

	// Process this group of signals definition...
	while(w != list_tokens.end()) {
		// If this token is a close bracket that closes this block...
//		if(search_key_found(CLOSE_BRACKET,(*w)) && (list_blocks.size()==num_brackets) ) {
		if(search_key_found(CLOSE_BRACKET,(*w)) || search_key_found(ALLPINS,(*w))) {
			// End of Pattern block...
cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<void file_analyzer::process_sig_grps()"<<endl;
			return;
		}else if((*w)==PIPINS) {
			// Skip its keyword token... PIPINS
			w++;
			// Skip the equal/assignment symbol...
			w++;
			// Get the list of input signals...
			str_list sl = delimit_string(*w);
			// Get the pointer to the first element in this list
			str_l_p p = sl.begin();

			/**
			 * Create input signals with the appropriate names from the list
			 * For each signal, create an instance of signalZ with its
			 * appropriate name
			 */
			while(p != sl.end()) {
				// Create an instance of signal
				temp_sig = new signalZ(*p);
				// Add this instance to the list of input signals
cout<<"\t\t\t\t\tINPUTput sigNaLS are:::"<< (*p) <<endl;
				list_of_ip_signals.push_back(temp_sig);
				// Process next available input signal
				p++;
			}
cout<<"---->Number of input signals:::"<<list_of_ip_signals.size()<<endl;
//		}else if((*w)==POPINS) {
		}else if(search_key_found(POPINS,(*w)) ) {
			// Skip its keyword token... POPINS
			w++;
			// Skip the equal/assignment symbol...
			w++;
			// Get the list of output signals...
			str_list sl = delimit_string(*w);
			// Get the pointer to the first element in this list
			str_l_p p = sl.begin();

			/**
			 * Create output signals with the appropriate names from the list
			 * For each signal, create an instance of signalZ with its
			 * appropriate name
			 */
			while(p != sl.end()) {
				// Create an instance of signal
				temp_sig = new signalZ(*p);
				// Add this instance to the list of output signals
cout<<"\t\t\t\t\tOUTput sigNaLS are:::"<< (*p) <<endl;
				list_of_op_signals.push_back(temp_sig);
				// Process next available output signal
				p++;
			}
cout<<"---->Number of output signals:::"<<list_of_op_signals.size()<<endl;
		}		
		
cout<<"]]]]]]][[[[[[[[[]]]]] VALUE of str Tokee"<< (*w) <<endl;
		// Else, process the next token...
		w++;
	}
cout<<"---->All input and output signals are processed..."<<endl;
throw ViolatedAssertion("Expected close bracket is MISSING!!!");
}



/**
 * Function to get the day, month, date, and year in which the test patterns
 * in the header file is generated
 * O(n) operation to search for the header tag; COMPLETED
 */
void file_analyzer::process_header() {
	// Find the header for the file...
	str_l_p  w = find_token(HEADER);
	
	// Assertion...
	if(w == list_tokens.end()) {
		cerr<<"Keyword "<<HEADER<<" is not found!"<<endl;
		throw ViolatedAssertion("Wrong keyword is used.");
	}
	
	// Determine the initial number of parentheses encasing the blocks...
	int num_brackets = list_blocks.size();
	// Proceed to the first token after the open curly bracket

	// Skip the keyword for the header...
	w++;
	// Skip the opening parenthesis for the block...
	w++;
	// Skip the title for the block...
	w++;
	// Skip the name for the block...
	w++;
	// Skip the format for the block...
	w++;
	// Skip the type of file for the block...
	w++;
	// Skip the word "File" for the block...
	w++;
	// Skip the field for the day of this file's generation...
	w++;


	/**
	 * Since the date/day/month/year fields are strings, and not a string,
	 * process each field as individual string tokens
	 * Also, it is noted that the delimit_string function does not handle
	 * double quotes well..."\""
	 * Hence, I am manually removing the double quotes for this small number
	 * of tokens... As opposed to modifying the delimit_string function.
	 */
	 
	 
	// Temporary string to contain the date/day/month/year fields
	string date_field;
	// Temporary string to contain current field
	string cur_field=(*w);
	// Remove the first character, which is a double quote
	cur_field.erase(0,1);
	// Concatenate the date/day/month/year field
	date_field.insert(date_field.size(),cur_field);
	date_field.insert(date_field.size()," ");
	
	// Process the month of this file generation
	w++;
	// Concatenate the date/day/month/year field
	date_field.insert(date_field.size(),(*w));
	date_field.insert(date_field.size()," ");
	
	// Process the date of this file generation
	w++;
	// Concatenate the date/day/month/year field
	date_field.insert(date_field.size(),(*w));
	date_field.insert(date_field.size()," ");
	
	// Process the year of this file generation
	w++;
	cur_field=(*w);
	// Remove the last two characters, which is a double quote and a semi-colon
	int s=cur_field.size()-1;
	cur_field.erase(s,1);
	s=cur_field.size()-1;
	cur_field.erase(s,1);


	// Concatenate the date/day/month/year field
	date_field.insert(date_field.size(),cur_field);
	
	// Print the respective date/day/month/year field to the output file
	print_output_ln(date_field);
//cout<<"%%%%%%%%%%string is"<<date_field<<endl;
}



/**
 * Function to truncate whitespace in the string s
 * @param s is the string which whitespace is to be truncated
 * O(n) computational time complexity; COMPLETED
 */
string file_analyzer::truncate_whitespace(string s) {
	for(int i=0; i<s.size(); i++) {
		// If enumerated character is whitespace, eliminate it
		if((s[i] == ' ') || (s[i] == '\t')) {
			s.erase(i,1);
			i--;
		}
	}
	
	return s;
}


/**
 * Function to output the signal names and their values into the output file
 * O(n) computational time complexity; COMPLETED
 */
void file_analyzer::dump_output() {
// ############################################################################
//signal *za = new signal("sigNamE");
//za = signal("sigNamE");
//signal *za;
//signalZ *za = new signalZ("sigNamE");

//signalZ za = new signalZ("sigNamE");
//za.print_output_ln(ofstream outputfile);
/*
za->add_char('L');
za->add_char('L');
za->add_values("1234qwerty56789iuytr357");
za->add_char('H');
*/
//za->print_output_ln(outputfile);
/*
signalZ *zz = new signalZ("TYREUIO");
zz->add_values("ZZw");
zz->add_char('1');
zz->add_char('0');
zz->add_values("a1234qwerty56789iuytr357b");
zz->add_char('P');

list_of_ip_signals.push_back(zz);
*/
//zz->print_output_ln(outputfile);
/*
zz = new signalZ("qw3r");
zz->add_values("gere");
zz->add_char('8');
zz->add_char('6');
zz->add_values("ydi123ukhkuh9");
zz->add_char('s');
*/

// Add current signal to the list of signals...
/*
list_of_ip_signals.push_back(za);
list_of_ip_signals.push_back(zz);
*/
print_output_ln(">>>Names and values of the input signals");
sig_l_p p = list_of_ip_signals.begin();
while(p != list_of_ip_signals.end()) {
	(*p)->print_output_ln(outputfile);
	p++;
}

print_output_ln(">>>Names and values of the output signals");

p = list_of_op_signals.begin();
while(p != list_of_op_signals.end()) {
	(*p)->print_output_ln(outputfile);
	p++;
}

//logic_values a = {"NaWE","DWIE"};
//print_output_ln(a.logic_name);
// ############################################################################
}



/**
 * Function to truncate the semi-colon appended to the end of the value/string
 * O(1) computational time complexity; COMPLETED
 */
string file_analyzer::truncate_semicolon(string s) {
	return s.erase((s.size()-1),1);
}



/**
 * Function to process the output pins of the module under test
 * This is included in the waveform block
 * @param p is the pointer to start of this block...
 * @return the number of tokens enumerated by this block
 * O(n) computational time complexity; COMPLETED
 */
int file_analyzer::process_popins(str_l_p p) {
cout<<">>>>>>>>>>>>>>>>>>>>>int file_analyzer::process_popins(str_l_p p)"<<endl;
	/**
	 * Flush the current waveform properties to load the new waveform properties
	 * of this signal or set of signals
	 * This obviates the need to clear the waveform block each time before this
	 * function is called.
	 */
	clear_wave_properties(POPINS);



cout<<"$$$$$ENtereD processing of the output pins!::!!!"<<endl;
	// Number of tokens enumerated in this function
	int num_tokes=0;
	// Temporary holder used for delimiting tokens
	string temp_holder;
	// List returned from delimiting string token
	str_list t_l_p;
	// Temporary storage of wave properties for currently enumerated signals
	signalZ::wave_prop ww;
	// Proceed to the first token after the open curly bracket

	// Get the first triggering conditions of the signal...
	p++;
	num_tokes++;
cout<<"P's current value:"<<(*p)<<">::"<<endl;
	p++;
	if((*p).size()<3) {
		p--;
cout<<"CurVaLofP<3:"<<(*p)<<">::"<<endl;
	}else{
cout<<"CurVaLofP>2:"<<(*p)<<">::"<<endl;
	}
/*
	p++;
	num_tokes++;
*/
	// Truncate whitespace...
	temp_holder = truncate_whitespace(*p);
cout<<"$$$$$ temp HOLDER!!!"<<temp_holder<<endl;
	// Assign this trigger to all output pins...
	// Get the pointer to the first output signal
	sig_l_p op_p = list_of_op_signals.begin();
	// Flag to determine if this signal should be even...
	int even_or_odd=1;
cout<<"$$$$$ENtereD while loop!!!"<<endl;
	// For each output signal in the system/circuit
	while(op_p != list_of_op_signals.end()) {
		// If signal is even, set it to be so...
		if((even_or_odd%2)==0) {
			(*op_p)->toggle();
cout<<"Signal is even"<<endl;
		}else{
cout<<"Signal is odd"<<endl;		
		}
	
		// Assign trigger to currently enumerated output signal
		(*op_p)->set_trigger(temp_holder);
cout<<"+++++++ assign the trigger to be:"<<temp_holder<<": for the OUTput signal name:"<<(*op_p)->get_name()<<":><"<<endl;
		// Proceed to the next output signal
		op_p++;
		even_or_odd++;
	}
	
cout<<"$$$$$EXiteD while loop!!!"<<endl;
	// Get the first waveform property of the signal...

	p++;
	num_tokes++;
cout<<"$$$$$ ---- string!!!"<<(*p)<<endl;

	p++;
cout<<"aaCurrent val of p:"<<(*p)<<endl;
	num_tokes++;
/*
	p++;
cout<<"bbCurrent val of p:"<<(*p)<<endl;
	num_tokes++;
*/
cout<<"$$$$$ENtereD NEXT	----	while loop!!!"<<(*p)<<endl;
	while(p != list_tokens.end()) {
		// If this token is a close bracket that closes this block...
		//if(search_key_found(CLOSE_BRACKET,(*p)) && (list_blocks.size()==num_brackets) ) {
		if(search_key_found(CLOSE_BRACKET,(*p))) {
cout<<"outputEnd of while >>><<< loop"<<endl;
cout<<"<<<<<<<<<<<<<<<<<<<<<int file_analyzer::process_popins(str_l_p p)"<<endl;
			// End of Pattern block...
			return num_tokes;
		}else{
			/**
			 * For each output signal,set their time and logic values
			 * for their wave properties
			 */
cout<<"$$$$$GET VALUE oF p!!!"<<endl;
cout<<"val p:"<<(*p)<<"<:::"<<endl;
			// Truncate whitespace of signal's waveform property (event/time)
			temp_holder = truncate_whitespace(*p);
cout<<"$$$$$no pointer prob with p!!!"<<endl;
cout<<"val temp_holder:"<<temp_holder<<"<:::"<<endl;
			// Delimit the string for non-alphanumeric characters...
			t_l_p=delimit_string(temp_holder);
cout<<"$$$$$ NO error with string delimiter!!!"<<endl;
cout<<"$$$$$ sIZe of LiST:::"<<t_l_p.size()<<endl;
			// Chop its units of time...
			temp_holder=t_l_p.front();
cout<<"$$$$$ get 1st elem of the list of strings!!!"<<temp_holder<<endl;
			temp_holder=chop_ns(temp_holder);
cout<<"$$$$$ chop units of time!!!"<<endl;
			// Add it to the time property of these signals' waveform
			ww.time=temp_holder;
			// And push it into list_blocks...
			list_blocks.push_back(temp_holder);
cout<<"output1QWERTY:::"<< temp_holder << endl;
			// Get its corresponding value...
			p++;
			num_tokes++;
			// Truncate whitespace of signal's waveform property (value)
			temp_holder = truncate_whitespace(*p);
			// Remove its semi-colon suffix...
			temp_holder=truncate_semicolon(temp_holder);
			// Add it to the logic value for these signals' waveform property
			ww.logic_val=temp_holder;
			// And push it into list_blocks...
//			list_blocks.push_back(temp_holder);
			
			
			// Add this waveform property to all output signals
			// Get the pointer to the first output signal
			op_p = list_of_op_signals.begin();
			// For each output signal in the system/circuit
			while(op_p != list_of_op_signals.end()) {
				// Assign waveform property to currently enumerated output signal
				(*op_p)->add_wave_prop(ww);
				// Proceed to the next output signal
				op_p++;
			}
		
				
cout<<"output2QWERTY:::"<< temp_holder << endl;
		}
		
		// Process the next token...
		p++;
		num_tokes++;
	}
cout<<"outputEnd of while loop>>><<<"<<endl;
}



/**
 * Function to process the waveform for the master/slave clock of the system/circuit
 * This function should only modify the waveform properties of a specified
 * clock signal
 * @param p is the pointer to the keyword token for the master clock waveform
 * @param clk_type indicates the type of clock to search for, and utilize
 * @return the number of tokens enumerated by this block
 * O(n) computational time complexity; COMPLETED
 */
int file_analyzer::process_clk(str_l_p p, string clk_type) {
cout<<">>>>>>>>>>>>>>>int file_analyzer::process_clk(str_l_p p, string clk_type)"<<endl;
	if((clk_type != SCANCLK) && (clk_type != MASTERCLK)) {
		throw ViolatedPrecondition("Invalid clock type is used");
	}
	
	/**
	 * Flush the current waveform properties to load the new waveform properties
	 * of this signal or set of signals
	 * This obviates the need to clear the waveform block each time before this
	 * function is called.
	 */
	clear_wave_properties(clk_type);

	// Number of tokens enumerated in this function
	int num_tokes=0;
	// Temporary holder used for delimiting tokens
	string temp_holder;
	// List returned from delimiting string token
	str_list t_l_p;
	// Temporary storage of wave properties for currently enumerated signals
	signalZ::wave_prop ww;
	// Proceed to the first token after the open curly bracket

	// Get the first triggering conditions of the signal...
	p++;
cout<<"The value of P is initially:"<<(*p)<<"]::"<<endl;
	num_tokes++;
	p++;
cout<<"The value of P is now:"<<(*p)<<"]::"<<endl;
	num_tokes++;
	// Truncate whitespace...
	temp_holder = truncate_whitespace(*p);
cout<<"\t\t\t\t\t TempHolDEr:::"<< temp_holder <<"[[["<<endl;
	p++;
cout<<"The value of P is now:"<<(*p)<<"]::"<<endl;
	num_tokes++;
	p++;
cout<<"The value of P is now:"<<(*p)<<"]::"<<endl;
	num_tokes++;
	// Assign this trigger to the master/slave clock input pin...
	// Get the pointer to the first input signal...
	sig_l_p ip_p = list_of_ip_signals.begin();
	
	// Find the master/slave clock input signal in the system/circuit
	while(ip_p != list_of_ip_signals.end()) {
		// Is this input signal the master/slave clock?
		if((*ip_p)->get_name()== clk_type) {
			// Yes; get out of this loop
cout<<"Clock type is found!!!"<<endl;
			break;
		}
		
		// Proceed to the next input signal
		ip_p++;
	}
	

	// If this signal is not the master/slave clock...
	if((*ip_p)->get_name() != clk_type) {
		/**
		 * There exists an error, since the circuit should contain a
		 * master/slave clock
		 */
		throw ViolatedAssertion("Master clock does not exist!");
	}
cout<<"Name of Clock Signal:"<<(*ip_p)->get_name()<<"]]]"<<endl;
	// Set the condition of the clock signal
	(*ip_p)->set_trigger((*p));
cout<<"Trigger of Clock Signal:"<<(*ip_p)->get_trigger()<<"]]]"<<endl;	
	
	// Get the waveform properties of the signal...
	
	/**
	 * While the waveform properties of this signal have not been completely
	 * defined
	 */
	while(p != list_tokens.end()) {
		// If this token is a close bracket that closes this block...
		if(search_key_found(CLOSE_BRACKET,(*p))) {
			num_tokes++;
//			num_tokes++;
//			num_tokes++;
cout<<"The number of enumerated tokens for this process_clk function is:"<<num_tokes<<">s:::"<<endl;
cout<<"<<<<<<<<<<<<<<<int file_analyzer::process_clk(str_l_p p, string clk_type)"<<endl;
			// Return the number of tokens enumerated by this block...	
			return num_tokes;
		}else{
			/**
			 * Delimit the tokens... to obtain the waveform properties of the
			 * clock signal
			 */
			
			// Delimit the string for non-alphanumeric characters...
			t_l_p=delimit_string(*p);
cout<<"The pointer points to:"<<(*p)<<"###"<<endl;
cout<<"Size of delimited timing property:"<<t_l_p.size()<<"###"<<endl;
			/**
			 * Remove the units of time for this timing waveform property
			 * Add the timing property to the temporary waveform property struc
			 */
			ww.time=chop_ns(t_l_p.front());
			
			
			// Get the logic value for its waveform property
			p++;
cout<<"Logic Val of P:"<<(*p)<<"<:::"<<endl;
			// Delimit the string for non-alphanumeric characters...
			t_l_p=delimit_string(*p);
			// Add it to the waveform property
			ww.logic_val=t_l_p.front();
cout<<"Logic Val:"<<ww.logic_val<<"<:::"<<endl;
			
			// Add it to the list of waveform properties...
			(*ip_p)->add_wave_prop(ww);
		}

		// Proceed to the next available token in the input file...
		p++;
		// Increment the numbere of traversed tokens in the input file
		num_tokes++;
	}


/*
	p++;
	num_tokes++;
	p++;
	num_tokes++;
*/
// Double the number of enumerated tokens
//num_tokes=num_tokes*2;
// Skip the last non-bracket token
//num_tokes=num_tokes+3;


	/**
	 * A close bracket has not been found whilst enumerating to the end of the
	 * list...
	 * This implies an unbalanced set of brackets.
	 * Report an error!
	 */
	cerr<<"Search for close bracket to end waveform property definition for";
	cerr<<" clock signal has FAILED"<<endl;
	throw ViolatedAssertion("Brackets are not balanced for clock signal");
}


/**
 * Function to process the waveform for the signals of the system/circuit
 * @param p is the pointer to the keyword token for the signal vectors
 * @param vector_type is the type of signal vectors to be processed
 * @return the number of tokens enumerated by this block
 * O(n) computational time complexity; YET TO BE COMPLETED
 */
int file_analyzer::process_v_blk(str_l_p p, string vector_type) {
cout<<">>>>>>>>>>>>>>>>int file_analyzer::process_v_blk(str_l_p p, string vector_type)"<<endl;
cout<<"ENter the vector/condition block"<<endl;
// DEBUG THIS FUNCTION
	// Number of tokens enumerated in this function
	int num_tokes=0;
	// Temporary holder used for delimiting tokens
	string temp_holder;
	// Temporary holder for the name of the currently enumerated signal
	string temp_sig_name;
	// Temporary holder for the value of the currently enumerated signal
	string temp_sig_value;
	// Pointer to currently enumerated signal in the list of signals
	sig_l_p p_s;
	// List returned from delimiting string token
	str_list t_l_p;
	// Pointer to token in list of names and logic values of signal vector
	str_l_p sv_p;
	// Temporary string token holder...
	string current_token;
	/**
	 * Flag to indicate if signal value is a hash, "#":
	 * true if value is a hash; else, false
	 */
	bool is_hash=false;
	
	/**
	 * Temporary holder for currently enumerated signal object
	 * Since NULL evaluates to zero, use a dummy signal object for comparison
	 * That is, if this signal's name is undefined, it is yet to be assigned
	 * to an existing input or output signal
	 */
	signalZ *temp_sig_obj = new signalZ();
	signalZ::wave_prop ww;
	// Error message printed in standard error output
	string err_msg;
	
	
	// Proceed to the first token after the open curly bracket

cout<<"1P's val is:::"<< (*p) <<endl;
	// Get the first group of signals to process...
	p++;
cout<<"2P's val is:::"<< (*p) <<endl;
	num_tokes++;
	p++;
cout<<"3P's val is:::"<< (*p) <<endl;
	num_tokes++;
cout<<"\t\t\t\t\t Sig-->TempHolDEr::: ENTER WHILE LOOP"<<endl;
	
	// Get the first waveform property of the signal...

	// Process the values for various input and and output signals
	while(p != list_tokens.end()) {
cout<<"*******The next token that P is PoINtInG tI IS:"<<(*p)<<endl;
		// If this token is a close bracket that closes this block...
		if(search_key_found(CLOSE_BRACKET,(*p))) {
			num_tokes++;
cout<<"sig of while >>><<< loop::::::::"<<num_tokes<<endl;
cout<<"<<<<<<<<<<<<<<<<int file_analyzer::process_v_blk(str_l_p p, string vector_type)"<<endl;
			// End of Pattern block...
			return num_tokes;
		}else{
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
			/**
			 * Attempt to delimit token...
			 * Set the logic values for the input and output signals...
			 */
cout<<"cURreNt StrInG toKEN:::"<<(*p)<<">>>>"<<endl;
current_token=(*p);
			// Reset the temporary signal object for the next token to be processed
			temp_sig_obj = new signalZ();
			
			// Delimit the string for non-alphanumeric characters...
			t_l_p=delimit_string(*p);

			// String token represents name of signal or signal vector
			temp_sig_name=t_l_p.front();
cout<<"String comparator<<<"<<current_token.substr(current_token.size()-2,1)<<":::"<<endl;
			// If the string token ends with a hash character "#" and ";"
			if("#" == current_token.substr(current_token.size()-2,1)) {
				// Set the is_hash flag to true for this signal
cout<<"Last character of current string is  a HASH@@@"<<endl;
				is_hash=true;
			}
cout<<"temp_sig_name IS~~~"<<temp_sig_name<<endl;
cout<<"&&&\ttemp sig size IS~~~"<<t_l_p.size()<<endl;
			/**
			 * Does this list of string tokens for this signal vector contain
			 * one element or three elements?
			 * List size of 1 indicates that the signal vector definition for
			 * each signal group or signal is character space delimited.
			 * List size of 3 indicates that the signal vector definition for
			 * each signal group or signal is not delimited
			 *
			 * Since the keyword "V" has definitions that include individual
			 * signals and groups of signals, it does not matter if it is a
			 * condition "C" or vector "V"; conditions only define vectors...
			 *
			 * For each signal vector, apply the (n)^{th} char to the (n)^{th}
			 * signal in that vector
			 *
			 * It is noted that lists of size 1 help define signals, while
			 * those of size 3 help define signals and signal vectors
			 */
			if(t_l_p.size() == 1)  {
				// Update the appropriate signal
				
				// Is the value of this signal a hash?
				if(is_hash) {
					/**
					 * Yes... Assign the value of the signal to be a "hash"
					 * Note that the # has already been deleted from the
					 * delimit operation... Hence, this replaces it
					 */
					temp_sig_value="#";
cout<<"Condition has HASH char:"<<temp_sig_value<<"<>"<<endl;
				}else{
					/**
					 * No... Skip the next two tokens to access its
					 * corresponding value
					 */
					p++;
					p++;
					// Assign the value of the signal to the current token
					temp_sig_value=(*p);
cout<<"Condition has NO hash char:"<<(*p)<<"<>"<<endl;
				}

				// Process logic values of corresponding signal(s)
// Shall I use the string object or the pointer to update the values?
// Try using pointers

				update_indv_sig(temp_sig_name,temp_sig_value,vector_type);

				// -----------------------------------------------------
			}else if(t_l_p.size() == 2) {
				
				// The first token is the name of signal or signal vector
cout<<"There exists 2 tokens"<<endl;
				/**
				 * Pointer to the first element in the list of string tokens
				 * The element is a signal or a signal vector
				 */
				sv_p=t_l_p.begin();
				temp_sig_name=(*sv_p);
				sv_p++;
				temp_holder=(*sv_p);
cout<<"set condition/value"<<endl;
cout<<"@~~~SIgNaME:"<<temp_sig_name<<":SigVal:"<<temp_holder<<":SigTyPe:"<<vector_type<<":::"<<endl;
				set_cond_or_val(temp_sig_name, temp_holder, vector_type);
cout<<"condition/value is set"<<endl;			
			}else{
				cerr<<"String token for signal vector definition is found"<<endl;
				throw ViolatedAssertion("Size of string token should be 1 or 3");
			}
			
			
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		}
cout<<"next condition/value"<<endl;
		// Process the next token...
		p++;
		num_tokes++;
	}
cout<<"BlKvEnd of while loop>>><<<"<<endl;	
}



/**
 * Function to set the values/conditions for this signal vector
 * @param sig_name is the name of this signal vector
 * @param sig_val is the value for the signal vector
 * @param sig_type indicates the values/conditions of the signals
 * @return nothing
 */
void file_analyzer::set_cond_or_val(string sig_name, string sig_val, string sig_type) {
cout<<">>>>>>>>>>>>>>>>>>void file_analyzer::set_cond_or_val(string sig_name, string sig_val, string sig_type)"<<endl;
cout<<"name of Signal vector:"<<sig_name<<"<:::"<<endl;
cout<<"values of Signal vector:"<<sig_val<<"<:::"<<endl;
	// Temporary pointer to a signal's given value in the signal vector
	int val_index=0;
	
	// Temporary storage of signals' list that is being traversed
	sig_list temp_s_l;
	// Pointer to temporary list of signals
	sig_l_p temp_s_l_p;
	// Error message printed with exceptions
	string err_msg;


	// Indicate the index i of the i^{th} traversed character
	int traversed_i=0;

cout<<"Set condition or value for this signal in set_cond_or_val blk"<<endl;
	// Check if element is an input signal vector
	if(sig_name==PIPINS) {
cout<<"Group of signals found is the input signals"<<endl;
		// Enumerate each input signal
		temp_s_l_p = list_of_ip_signals.begin();
		while(temp_s_l_p != list_of_ip_signals.end()) {
cout<<"NameOfSignal:"<<(*temp_s_l_p)->get_name()<<">:::"<<endl;
			if((val_index <= sig_val.size()) && (sig_type == C)) {
cout<<"Signal's logic value::"<<sig_val.substr(val_index,1)<<":::"<<endl;
cout<<"Signal's logic value::"<<sig_val<<":::"<<endl;
				(*temp_s_l_p)->set_condition(sig_val.substr(val_index,1));

cout<<"Go to the next signal"<<endl;
				val_index++;

			}else if(sig_type == V) {
				(*temp_s_l_p)->sig_vector(sig_val);
cout<<"Added the string:"<<sig_val<<">:::"<<endl;
			}else{
				throw ViolatedAssertion("Signal type is INVALID!!!");
			}

			// Proceed to process the next value in this signal vector
			temp_s_l_p++;
cout<<"Proceed to the next token in the input signal group"<<endl;
		}
	}else if(sig_name==POPINS) {
		// Or if element is an output signal vector
cout<<"Group of signals found is the output signals"<<endl;
		// Enumerate each output signal
		temp_s_l_p = list_of_op_signals.begin();
		while(temp_s_l_p != list_of_op_signals.end()) {
			
			if(val_index < sig_val.size()) {
cout<<"Signal's logic value::"<<sig_val.substr(val_index,1)<<":::"<<endl;
				if(sig_type == V) {
					(*temp_s_l_p)->sig_vector(sig_val.substr(val_index,1));
cout<<"Added the string:"<<sig_val.substr(val_index,1)<<">:::"<<endl;
				}else if(sig_type == C) {
cout<<"DelIMiTED COndiTiON:"<<sig_val.substr(val_index,1)<<":::"<<endl;
					(*temp_s_l_p)->set_condition(sig_val.substr(val_index,1));
/*
					if(sig_val.size() != 1) {
cout<<"SigSizE:"<<sig_val.size()<<":::"<<endl;
cout<<"String COndiTiON:"<<sig_val.substr(val_index,1)<<":::"<<endl;
cout<<"DelIMiTED COndiTiON:"<<sig_val<<":::"<<endl;
						throw ViolatedAssertion("Signal condition is INVALID!!!");
					}else{
cout<<"Condition of sig:"<<sig_val<<">:::"<<endl;
						(*temp_s_l_p)->set_condition(sig_val);
					}
*/
				}else{
					throw ViolatedAssertion("Signal type is INVALID!!!");
				}
cout<<"Go to the next signal"<<endl;
				val_index++;
			}else{
cout<<"Signal vector has more values than its number of elements!!!"<<endl;
			}

			// Proceed to process the next value in this signal vector
			temp_s_l_p++;
cout<<"Proceed to the next token in the output signal group"<<endl;
		}
	}else{
		/**
		 * This is not a signal vector, throw an Exception to warn the user of
		 * this error.
		 */ 
		cerr<<"Signal vector is missing"<<endl;
		throw ViolatedAssertion("Signal is found instead!");
	}
cout<<"<<<<<<<<<<<<<<<<<<void file_analyzer::set_cond_or_val(string sig_name, string sig_val, string sig_type)"<<endl;
}


/**
 * Function to process the scanProc block
 * @return the number of enumerated signals
 */
int file_analyzer::process_scanproc_blk(string s) {
cout<<">>>>>>>>>>>>>>>>>>>>>int file_analyzer::process_scanproc_blk(string s)"<<endl;
	// Keep track of enumerated string tokens in the file
	int enum_tokens=0;

	// Pointer to tokens in the delimited pattern vector
	str_l_p delimit_p;
	
	// Temporary storage of delimited pattern vector
	str_list delimited_pattern;
	
	// Enumerate the list of string tokens in search of s
	
	// Pointer to the keyword s
	str_l_p  tpp = find_token(s);
	
	// Assertion...
	if(tpp == list_tokens.end()) {
		cerr<<"Keyword "<<s<<" is not found!"<<endl;
		throw ViolatedAssertion("Wrong keyword is used.");
	}
	
	// Skip 4 tokens to reach the pattern vector
	// Skip to "Call"
	tpp++;
	enum_tokens++;
	// Skip to "scanProc"
	tpp++;
	enum_tokens++;
	// Skip to "{"
	tpp++;
	enum_tokens++;
	// Skip to "scanIn1" or "scanOut1"
	tpp++;
	enum_tokens++;
cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!		ENter the wHiLE LoOP"<<endl;
cout<<"CuRReNtToKeN:"<<(*tpp)<<endl;
	/**
	 * For each subsequent string token, delimit it and insert it as a pattern
	 * vector to the specified signal
	 * Repeat until a close bracket is found
	 */
	while(!search_key_found(CLOSE_BRACKET, (*tpp))) {
		// Delimit this pattern vector...
		delimited_pattern = delimit_string(*tpp);
		
		/**
		 * Note that the first token of this list of strings indicates the name of
		 * the signal, while the second token indicates the value of that signal
		 */
cout<<"$$$$$$$$$$$$$$$$$	Transmit Pattern"<<endl;		
		// Assign this enumerated pattern to the specified pattern
		transmit_patterns(delimited_pattern);
cout<<"$$$$$$$$$$$$$$$$$	Pattern Is Transmitted"<<endl;
		// Move to the next token
		tpp++;
cout<<"$$$$$$$$$$$$$$$$$	nExT ToKEn iS:"<< (*tpp) <<"<:::"<<endl;
		enum_tokens++;
	}
cout<<"~~~~~~~~~~~PRE~~~~~~~~SCAN~~~~~~~~~~~BLOCK~~~~~~~~~~~~TRACE~~~~~~~~~"<<endl;
check_scan_block();	
	
	// The signal patterns have been loaded... Process the scan block
	scan_process();
cout<<"<<<<<<<<<<<<<<<<<<int file_analyzer::process_scanproc_blk(string s)"<<endl;
	return enum_tokens;
}


/**
 * Function to process the scan process macro defintion
 * @return nothing
 */
void file_analyzer::scan_process() {
cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>void file_analyzer::scan_process() {"<<endl;
	// Number of tokens traversed by called method
	int traversed_tokens=0;

	// Pointer to tokens in the delimited pattern vector
	str_l_p delimit_p;
	
	// Temporary storage of delimited pattern vector
	str_list delimited_pattern;

	// Pointer to the keyword "Procedures"
	str_l_p  tpp = find_token(PROCEDURE);
	
	// Assertion...
	if((tpp == list_tokens.end()) && ((*tpp)==PROCEDURE)) {
		cerr<<"Keyword "<<PROCEDURE<<" is not found!"<<endl;
		throw ViolatedAssertion("Wrong keyword is used.");
	}
cout<<"_________SCAN	PROCESS1; token tpp:"<<(*tpp)<<"<:::"<<endl;	
	// Skip past the open bracket: "{"
	tpp++;
cout<<"_________SCAN	PROCESS2; token tpp:"<<(*tpp)<<"<:::"<<endl;
	// Skip past the keyword "scanProc"
	tpp++;
cout<<"_________SCAN	PROCESS3; token tpp:"<<(*tpp)<<"<:::"<<endl;
	// Skip another 2 tokens to reach the macros
	// Skip to "{"
	tpp++;
cout<<"_________SCAN	PROCESS4; token tpp:"<<(*tpp)<<"<:::"<<endl;
	// Skip to "W"
	tpp++;
cout<<"_________SCAN	PROCESS5; token tpp:"<<(*tpp)<<"<:::"<<endl;
cout<<"_________SCAN	PROCESS6; enter the while loop"<<endl;
	// While the close bracket is not found...
	while(!search_key_found(CLOSE_BRACKET, (*tpp))) {
		// Get the delimited string token
		delimited_pattern = delimit_string(*tpp);
		// Pointer to tokens in the delimited pattern vector
//		delimit_p = delimited_pattern.begin();
		
		if(delimited_pattern.front()==W) {
			// Get the name of the waveform
			tpp++;
			// Process this waveform definition
cout<<"_________SCAN	PROCESS10; found waveform"<<endl;
cout<<"_________SCAN	PROCESS11; token tpp:"<<(*tpp)<<"<:::"<<endl;
			traversed_tokens=process_waveform_blk(truncate_semicolon(*tpp));
//******
traversed_tokens=0;
cout<<"_________SCAN	PROCESS12; token tpp:"<<(*tpp)<<"<:::"<<endl;
		}else if(delimited_pattern.front()==C) {
			// Process this signal definition
cout<<"_________SCAN	PROCESS13; found condition"<<endl;
			traversed_tokens=process_v_blk(tpp, C);
check_signal_condition();
check_signal_value();
//******
cout<<":::::::::::traversed_tokens:::::::"<<traversed_tokens<<"::::::::::::"<<endl;
//traversed_tokens=traversed_tokens*2;
tpp--;
cout<<"_________SCAN	PROCESS14; token tpp:"<<(*tpp)<<"<:::"<<endl;
		}else if(delimited_pattern.front()==V) {
			// Process this signal definition
cout<<"_________SCAN	PROCESS15; found value"<<endl;
			traversed_tokens=process_v_blk(tpp, V);
check_signal_condition();
check_signal_value();
cout<<"_________SCAN	PROCESS16; token tpp:"<<(*tpp)<<"<:::"<<endl;
		}else if(delimited_pattern.front()==SHIFT) {
cout<<"_________SCAN	PROCESS17; found shift"<<endl;
cout<<"_________SCAN	PROCESS18; token tpp:"<<(*tpp)<<"<:::"<<endl;
			// Skip to the signal definition...
			// Skip to the token "{"
			tpp++;
cout<<"_________SCAN	PROCESS19; token tpp:"<<(*tpp)<<"<:::"<<endl;
			// Skip to the token "V" representing
			tpp++;
cout<<"_________SCAN	PROCESS20; token tpp:"<<(*tpp)<<"<:::"<<endl;
			// Process this waveform definition
			traversed_tokens=process_v_blk(tpp,V);
			// Skip to the token "}"
			tpp++;
cout<<"_________SCAN	PROCESS21; token tpp:"<<(*tpp)<<"<:::"<<endl;
		}
cout<<"_________SCAN	PROCESS22; pre inner while loop"<<endl;
		// Skip past the processed tokens
		while(traversed_tokens>0) {
			traversed_tokens--;
			tpp++;
if((*tpp)==SHIFT){
tpp--;
}
cout<<"_________SCAN	PROCESS30; next T:"<<(*tpp)<<"<<::"<<endl;
		}

cout<<"_________SCAN	PROCESS; last part of while loop-token tpp:"<<(*tpp)<<"<:::"<<endl;		
		// Proceed to the next token
		tpp++;
cout<<"_________SCAN	PROCESS; last part of while loop-incre tpp:"<<(*tpp)<<"<:::"<<endl;
	}
cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<void file_analyzer::scan_process() {"<<endl;
}



/**
 * Function to transmit signal pattern to specified signal
 * @param signame_n_pattern contains two tokens: the name of the signal and
 *	the pattern for that signal
 *	If this list of strings has more than two tokens, except for the first two
 *	elements, ignore all elements of the list
 *	This implies that the list of string tokens must contain more than 2
 *	elements
 * @return nothing
 */
void file_analyzer::transmit_patterns(str_list signame_n_pattern) {
cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>void file_analyzer::transmit_patterns()"<<endl;
/*
Search for signal name, update pattern, and exit
*/
	/**
	 * Assert that the number of elements in signame_n_pattern must exceed 2
	 * It should at least contain 2 elements:
	 * The first element is the name of the signal to be processed
	 * The second element is the pattern of that signal
	 * All other elements can be ignored
	 */
	if(signame_n_pattern.size() < 2) {
		cerr<<"Input parameter to the function transmit_patterns is a "<<endl;
		cerr<<"list of strings, containing the name of a signal and its"<<endl;
		cerr<<"pattern. It may contain subsequent tokens as garbage"<<endl;
		throw ViolatedPrecondition("Input list of strings has < 2 tokens");
	}

	/**
	 * Pointer to the first element in the list of strings
	 * 1st token is the name of the signal to be processed
	 */
	str_l_p snp = signame_n_pattern.begin();
	
	/**
	 * Search for the signal, which name is the 1st token of signame_n_pattern
	 * Start searching among the list of input signals, before looking at the
	 * list of output signals 
	 *
	 * Access the first input signal...
	 */
	sig_l_p pat_p=list_of_ip_signals.begin();
	// Enumerate each input signal to search for indicated signal
	while(pat_p != list_of_ip_signals.end()) {
cout<<"ii$$$$$$$$$$$$$$$$$	Currently enumerating signal with name:"<<(*pat_p)->get_name()<<endl;
		// Is this currently enumerated signal equal to the indicated signal?
		if((*snp) == (*pat_p)->get_name()) {
			// Yes... Get its pattern
			snp++;
			// Add the signal pattern to this signal
			(*pat_p)->add_sig_patterns(*snp);
cout<<"ip SigPaTtERn:"<<(*snp)<<"<:::"<<endl;
			// Exit function... since no other function should be affected
			return;
		}
		
		// Proceed to the next input signal
		pat_p++;
	}

	/**
	 * The signal can not be found in the list of input signals
	 * Check if it is an output signal
	 * Access the first output signal...
	 */
	pat_p=list_of_op_signals.begin();
	// Enumerate each output signal to search for indicated signal
	while(pat_p != list_of_op_signals.end()) {
cout<<"oo$$$$$$$$$$$$$$$$$	Currently enumerating signal with name:"<<(*pat_p)->get_name()<<endl;
		// Is this currently enumerated signal equal to the indicated signal?
		if((*snp) == (*pat_p)->get_name()) {
			// Yes... Get its pattern
			snp++;
			// Add the signal pattern to this signal
			(*pat_p)->add_sig_patterns(*snp);
cout<<"op SigPaTtERn:"<<(*snp)<<"<:::"<<endl;
			// Exit function... since no other function should be affected
			return;
		}
		
		// Proceed to the next input signal
		pat_p++;
	}
	
	// Specified signal is not found...
	throw ViolatedPrecondition("Specified signal is not found!!!");
cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<void file_analyzer::transmit_patterns()"<<endl;
}



/**
 * Function to search for a signal, and update its values/patterns or set its
 * condition for execution
 * @param sig_name is the name of the signal
 * @param sig_val is the value/pattern of the signal
 * @param sig_type is the flag/type indicator for the processing of this signal
 * @return nothing
 */
void file_analyzer::update_indv_sig(string sig_name, string sig_val, string sig_type) {
cout<<">>>>>>>>>>>>>>>>>>>>>>>void file_analyzer::update_indv_sig(string sig_name, string sig_val, string sig_type)"<<endl;
	// Search for the signal
	
	
	// Is this signal an input signal?
	
	// Use a pointer to access the first input signal
	sig_l_p slp = list_of_ip_signals.begin();
	// While there are more input signals to process...
	while(slp != list_of_ip_signals.end()) {
		// Does this input signal matches with the desired input signal
		if((*slp)->get_name() == sig_name) {
			// Yes... Check if indicator is a condition
			if(sig_type == C) {
				// Set the condition of the signal
cout<<"Set the condition of this signal"<<endl;
				(*slp)->set_condition(sig_val);
cout<<"Condition of this signal"<<(*slp)->get_condition()<<"<:::"<<endl;
			}else if(sig_type == V) {
				/**
				 * Or a signal vector...
				 * Is the condition equal to P, & the list of patterns isn't
				 * empty?
				 * See other boolean case for situations that 
				 */
				if(((*slp)->get_condition()==P) && (!(*slp)->is_pattern_list_empty())) {
					// Set its condition to P
//					(*slp)->set_condition(P);
					/**
					 * Yes, add the last pattern to its values in the signal
					 * vector
					 * Note that if no patterns are defined, patterns cannot
					 * be added to the values of the signal vector
					 */
//### IMPORTANT
//					(*slp)->add_values((*slp)->get_last_pattern());
cout<<"PPP\tSet the pattern to be:"<<(*slp)->get_last_pattern()<<"<:::"<<endl;
cout<<"PPP\tCurVal:"<<(*slp)->get_sig_values()<<"<:::"<<endl;
					(*slp)->sig_vector((*slp)->get_last_pattern());
cout<<"PPP\tNEW CurVal:"<<(*slp)->get_sig_values()<<"<:::"<<endl;
//				}else if((*slp)->condition_met()) {
				}else{
					/**
					 * Else, add the signal's value as assigned
					 * If the pattern is P, and the signal pattern is not defined,
					 * add the value as normal
					 */
//### IMPORTANT
//					(*slp)->add_values(sig_val);
cout<<"The condition is:"<<(*slp)->get_condition()<<":COND"<<endl;
cout<<"Set the pattern to be:"<<sig_val<<"<:::"<<endl;
cout<<"CurVal:"<<(*slp)->get_sig_values()<<"<:::"<<endl;
					(*slp)->sig_vector(sig_val);
cout<<"NEW CurVal:"<<(*slp)->get_sig_values()<<"<:::"<<endl;
				}
			}else{
				// Report this error
				throw ViolatedAssertion("Type of signal indicator is invalid");
			}
cout<<"<<<<<<<<<<<<<<<<<<<<<<<void file_analyzer::update_indv_sig(string sig_name, string sig_val, string sig_type)"<<endl;
			// End of method
			return;
		}
		// Process the next available input signal
		slp++;
	}
	
	
	// Else, is this signal an output signal?
	// Use a pointer to access the first output signal
	slp = list_of_op_signals.begin();
cout<<"Look at the output signals for the system"<<endl;
	// While there are more output signals to process...
	while(slp != list_of_op_signals.end()) {
		// Does this output signal matches with the desired output signal
		if((*slp)->get_name() == sig_name) {
			// Yes... Check if indicator is a condition
			if(sig_type == C) {
				// Set the condition of the signal
cout<<"Condition is:"<<(*slp)->get_condition()<<":Updated to:"<<sig_val<<"<:::"<<endl;
				(*slp)->set_condition(sig_val);
cout<<"Condition is NOW:"<<(*slp)->get_condition()<<"<:::"<<endl;
			}else if(sig_type == V) {
				/**
				 * Or a signal vector...
				 * Is the condition equal to P, & the list of patterns isn't
				 * empty?
				 */
//				if((sig_val==P) && (!(*slp)->is_pattern_list_empty())) {
				if(((*slp)->get_condition()==P)
					&& (!(*slp)->is_pattern_list_empty())) {
					
					// Set its condition to P
//					(*slp)->set_condition(P);
					/**
					 * Yes, add the last pattern to its values in the signal
					 * vector
					 * Note that if no patterns are defined, patterns cannot
					 * be added to the values of the signal vector
					 */
//### IMPORTANT
//					(*slp)->add_values((*slp)->get_last_pattern());
cout<<"pattern IS:"<<(*slp)->get_last_pattern()<<"<::"<<endl;
					(*slp)->sig_vector((*slp)->get_last_pattern());
cout<<"pattern IS upDaTEd tO:"<<(*slp)->get_sig_values()<<"<::"<<endl;
//				}else if((*slp)->condition_met()) {
				}else{
					// Else, is the condition met
//### IMPORTANT
//					(*slp)->add_values(sig_val);
cout<<"SigValPattern is:"<<sig_val<<":::"<<endl;
					(*slp)->sig_vector(sig_val);
cout<<"SigValPattern is NOw updated TO:"<<(*slp)->get_sig_values()<<":::"<<endl;
				}
			}else{
				// Report this error
				throw ViolatedAssertion("Invalid type of signal indicator");
			}
cout<<"<<<<<<<<<<<<<<<<<<<<<<<void file_analyzer::update_indv_sig(string sig_name, string sig_val, string sig_type)"<<endl;
			// End of method
			return;
		}
		// Process the next available output signal
		slp++;
	}
	
	/**
	 * Otherwise, this signal does not belong to this circuit/system
	 * Report the error
	 */
	throw ViolatedAssertion("This is not an input/output signal!");
}


/**
 * Function to verify the processing of waveforms is corrrect by inspection
 * @return nothing
 */
void file_analyzer::check_waveform() {
cout<<"w\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"<<endl;
	// List of waveform properties for a signal
	signalZ::wp_list sl;
	// Pointer to the list of waveform properties for a signal
	signalZ::wp_l_p wp;
	// Pointer to the first element in the list of input signals
	sig_l_p sp = list_of_ip_signals.begin();
	// For each input signal...
	while(sp != list_of_ip_signals.end()) {
		// Indicate its waveform properties
		sl = (*sp)->get_wave_prop();
		wp=sl.begin();
		cout<<"For signal:"<<(*sp)->get_name();
		// Enumerate each signal waveform property...
		while(wp != sl.end()) {
			// Print the information in this waveform property
			cout<<":time:"<<(*wp).time;
			cout<<":logic value:"<<(*wp).logic_val;
			
			// Move to the next waveform property...
			wp++;
		}
		cout<<"<:::"<<endl;
		
		// Proceed to the next available signal
		sp++;
	}
	
	
	// Pointer to the first element in the list of output signals
	sp = list_of_op_signals.begin();
	// For each output signal...
	while(sp != list_of_op_signals.end()) {
		// Indicate its waveform properties
		sl = (*sp)->get_wave_prop();
		wp=sl.begin();
		cout<<"For signal:"<<(*sp)->get_name();
		// Enumerate each signal waveform property...
		while(wp != sl.end()) {
			// Print the information in this waveform property
			cout<<":time:"<<(*wp).time;
			cout<<":logic value:"<<(*wp).logic_val;
			
			// Move to the next waveform property...
			wp++;
		}
		cout<<"<:::"<<endl;
		
		// Proceed to the next available signal
		sp++;
	}
cout<<"w///////////////////////////////////////////////////////////////////"<<endl;
}


/**
 * Function to verify the processing of signal conditions is corrrect by
 * inspection
 * @return nothing
 */
void file_analyzer::check_signal_condition() {
cout<<"c\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"<<endl;
	// Pointer to the first element in the list of input signals
	sig_l_p sp = list_of_ip_signals.begin();
	// For each input signal...
	while(sp != list_of_ip_signals.end()) {
		// Print the signal's current condition...
		cout<<"For signal:"<<(*sp)->get_name()<<":condition:";
		cout<<(*sp)->get_condition()<<"<:::"<<endl;
		// Proceed to the next available signal
		sp++;
	}
	
	// Pointer to the first element in the list of output signals
	sp = list_of_op_signals.begin();
	// For each output signal...
	while(sp != list_of_op_signals.end()) {
		// Print the signal's current condition...
		cout<<"For signal:"<<(*sp)->get_name()<<":condition:";
		cout<<(*sp)->get_condition()<<"<:::"<<endl;
		// Proceed to the next available signal
		sp++;
	}
cout<<"c///////////////////////////////////////////////////////////////////"<<endl;
}


/**
 * Function to verify the processing of signal values is corrrect by inspection
 * @return nothing
 */
void file_analyzer::check_signal_value() {
cout<<"v\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"<<endl;
	// Pointer to the first element in the list of input signals
	sig_l_p sp = list_of_ip_signals.begin();
	// For each input signal...
	while(sp != list_of_ip_signals.end()) {
		// Print the signal's current condition...
		cout<<"For signal:"<<(*sp)->get_name()<<":value:";
		cout<<(*sp)->get_sig_values()<<"<:::"<<endl;
		// Proceed to the next available signal
		sp++;
	}
	
	// Pointer to the first element in the list of output signals
	sp = list_of_op_signals.begin();
	// For each output signal...
	while(sp != list_of_op_signals.end()) {
		// Print the signal's current condition...
		cout<<"For signal:"<<(*sp)->get_name()<<":value:";
		cout<<(*sp)->get_sig_values()<<"<:::"<<endl;
		// Proceed to the next available signal
		sp++;
	}
cout<<"v///////////////////////////////////////////////////////////////////"<<endl;
}


/**
 * Function to verify the processing of scan blocks is corrrect by inspection
 * Enumerate all input and output signals and determine if they have properly
 * been assigned patterns for further processing
 * @return nothing
 */
void file_analyzer::check_scan_block() {
cout<<"sb\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"<<endl;
	// Pointer to currently enumerated string/signal pattern
	str_l_p sp_p;
	// List of signal patterns for this signal
	str_list list_pat;
	

	// Pointer to the first element in the list of input signals
	sig_l_p sp = list_of_ip_signals.begin();
	// For each input signal...
	while(sp != list_of_ip_signals.end()) {
		// Print the signal's list of patterns...
		cout<<"Name of Signal<<<"<<(*sp)->get_name()<<"<<<";

		// Does it have any input signals?
		if((*sp)->get_num_patterns() > 0) {
			// Access the first available pattern...
			list_pat = (*sp)->get_sig_patterns();
			sp_p = list_pat.begin();
			while(sp_p != list_pat.end()) {
				// Print this currently enumerated pattern
				cout << "Pattern::<>::" << (*sp_p) << "::><::";
				
				// Access the next available pattern...
				sp_p++;
			}
		}
		
		cout << "" << endl;
		
		// Proceed to the next available signal
		sp++;
	}
	
	// Pointer to the first element in the list of output signals
	sp = list_of_op_signals.begin();
	// For each output signal...
	while(sp != list_of_op_signals.end()) {
		// Print the signal's list of patterns...
		cout<<"Name of Signal<<<"<<(*sp)->get_name()<<"<<<";
		// Does it have any input signals?

		// Does it have any input signals?
		if((*sp)->get_num_patterns() > 0) {
			// Access the first available pattern...
			list_pat = (*sp)->get_sig_patterns();
			sp_p = list_pat.begin();
			while(sp_p != list_pat.end()) {
				// Print this currently enumerated pattern
				cout << "Pattern::<>::" << (*sp_p) << "::><::";
				
				// Access the next available pattern...
				sp_p++;
			}
			cout<<"End of currently processed block"<<endl;
		}
		
		cout << "" << endl;
		
		// Proceed to the next available signal
		sp++;
	}
cout<<"sb///////////////////////////////////////////////////////////////////"<<endl;
	
}



/**
 * Function to pass the period of the dominant clock into all signals
 * @param period_of_sig is the period of the dominant clock
 * @return nothing
 */
void file_analyzer::update_sig_period(int period_of_sig) {
cout<<"Set the clock periods for input signals"<<endl;
	// Pointer to signals in the input list
	sig_l_p sgp = list_of_ip_signals.begin();
	// For each input signal
	while(sgp != list_of_ip_signals.end()) {
		// Update the period of this signal
		(*sgp)->set_sig_period(period_of_sig);
		
cout<<"InpUTSignalNaME;"<<(*sgp)->get_name();
cout<<"; iPSiGNaLPerIoD;"<<(*sgp)->get_sig_period()<<"<;;;"<<endl;
		
		// Proceed to the next available signal
		sgp++;
	}
	
cout<<"Set the clock periods for output signals"<<endl;	
	// Pointer to signals in the output list
	sgp = list_of_op_signals.begin();
	// For each output signal
	while(sgp != list_of_op_signals.end()) {
		// Update the period of this signal
		(*sgp)->set_sig_period(period_of_sig);
		
cout<<"OuTpUTSignalNaME;"<<(*sgp)->get_name();
cout<<"; OPSiGNaLPerIoD;"<<(*sgp)->get_sig_period()<<"<;;;"<<endl;		
		// Proceed to the next available signal
		sgp++;
	}
cout<<"The clock periods for input and output signals are set"<<endl;
}



/**
 * Function to clear the waveform properties of all signals
 * Access each signal and empty its list of waveform properties
 * @return nothing
 */
void file_analyzer::clear_wave_properties(string key) {
	// String pointer to lists of input and output signals
	sig_l_p pp;
	// Reset waveform properties for input signals?
	if(key == PIPINS) {
		// Access the first available input signal
		pp = list_of_ip_signals.begin();
		// If possible, access the next available input signal
		while(pp != list_of_ip_signals.end()) {
			// Empty its list of waveform properties
			(*pp)->clr_waveform_properties();
			
			// Access the next available input signal
			pp++;
		}
		// Each input signal has been cleared of its waveform properties
		
	// Reset waveform properties for output signals?
	}else if(key == POPINS) {
		// Access the first available output signal
		pp = list_of_op_signals.begin();
		// If possible, access the next available output signal
		while(pp != list_of_op_signals.end()) {
			// Empty its list of waveform properties
			(*pp)->clr_waveform_properties();
			
			// Access the next available output signal
			pp++;
		}
		// Each output signal has been cleared of its waveform properties
		
	// Reset waveform property for selected clock (input) signal?
	}else if((key == SCANCLK) || (key == MASTERCLK)) {
		// Access the first available input signal
		pp = list_of_ip_signals.begin();
		// If possible, access the next available input signal
		while(pp != list_of_ip_signals.end()) {
			if((*pp)->get_name() == key) {
				// Empty its list of waveform properties
				(*pp)->clr_waveform_properties();
				
				// Exit loop, since clock signal has been reset!
				break;
			}
			// Access the next available input signal
			pp++;
		}
		
		// Check that the appropriate clock signal has been found!
		if(((*pp)->get_name() == SCANCLK) && ((*pp)->get_name() == MASTERCLK)) {
			throw ViolatedAssertion("Name of searched signal do not match key");
		}
		
		// Each input signal has been cleared of its waveform properties
	}else{
		// Type of signal or group of signal searched for does not exist
		throw ViolatedAssertion("Invalid Type/Group of Signal is Searched For");
	}
}