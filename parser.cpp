/**
 * This is written by Zhiyang Ong to parse a STIL file into a FLAT tabular
 * test pattern format that contains input test patterns for execution on
 * automatic test equipment
 * Main function...
 */

// Import Header files from the C++ STL
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <math.h>
#include "file_analyzer.h"

using namespace std;


// Instance variables
// Names for input and output file streams
string input_filename;
string output_filename;

// =======================================================================

// Start of main function...
int main(int argc, char *argv[]) {
	cout << "========================================================" << endl;
	cout << "Start parsing the STIL input file... And process it."<<endl;

	/*
	 * If the first two input arguments for this program is null,
	 * request for the user to rerun the program and exit.
	 */
	if((argv[1] == NULL) || (argv[2] == NULL)) {
		cout << "Please enter the names for the input and output files" << endl;
		cout << "when you rerun this program" << endl;
		cout << "i.e., [program name] [input filename] [output filename]" << endl;
		return 0;
	}else{
		// Assign the first input argument to the program as the input filename
		input_filename = argv[1];
		// Assign the 2nd input argument to the program as the output filename
		output_filename = argv[2];
	}
// Delete this block of code
	cout << "1st argument..." << endl;
	cout << input_filename << endl;
	cout << "2nd argument" << endl;
	cout << output_filename << endl;	
	

	// Prepare to parse the input file, process it, and produce the output file
	file_analyzer *fa = new file_analyzer(input_filename, output_filename);
	/**
	 * Parse the input file to produce an output text file indicating the
	 * test patterns for the automatic test equipment
	 */
	fa->parse_input();
	string be = fa->time_to_string(35, 1);
	
	// Try to see if the static method in SignalZ will work
/*
	signalZ::set_sig_period(756);
	cout<<"Val of period in SignalZ:"<<signalZ::get_sig_period()<<"<:::"<<endl;
*/	
	cout << "1Test pattern:::" << be << endl;
	be = fa->time_to_string(20, 0);
	cout << "2Test pattern:::" << be << endl;
	be = fa->int_to_str(5126759);
	cout << "3Test pattern:::" << be << endl;
//	signal *za = new signal("sigNamE",(*fa));

/*
cout << "delimit string" << endl;
	str_list a = fa->delimit_string("sa123d '23sd' 1e dm23 -=32-42 0-2359%^&%bgjhgbug&*^*(G");
	str_l_p p = a.begin();
	while(p != a.end()) {
		cout << "Current token is:::" << (*p) << endl;
		p++;
	}
*/
/*
cout << "delimit string" << endl;
cout << "string IS" << "'23sd'" << endl;
	str_list a = fa->delimit_string("'23sd'");
cout << "size of string" << a.size() << endl;
	str_l_p p = a.begin();
	while(p != a.end()) {
		cout << "Current token is:::" << (*p) << endl;
		p++;
	}

cout << "string IS" << "#$%^&  y1234y u23149 83u84 *(^&" << endl;
a = fa->delimit_string("#$%^&  y1234y 'u23149' 83u84 *(^&");
cout << "size of string" << a.size() << endl;
p = a.begin();
while(p != a.end()) {
	cout << "Current token is:::" << (*p) << endl;
	p++;
}

cout << "string IS" << "gauyg212 jhg324 'g24j3yg' 70981" << endl;
a = fa->delimit_string("gauyg212 jhg324 'g24j3yg' 'w'70981");
cout << "size of string" << a.size() << endl;
p = a.begin();
while(p != a.end()) {
	cout << "Current token is:::" << (*p) << endl;
	p++;
}

cout << "string IS" << "gasdy hugdqeuyo oygwe ''y19y''" << endl;
a = fa->delimit_string("gasdy hugdqeuyo oygwe ''y19y''");
cout << "size of string" << a.size() << endl;
p = a.begin();
while(p != a.end()) {
	cout << "Current token is:::" << (*p) << endl;
	p++;
}

cout<<"1TRUN:::"<<fa->truncate_whitespace("			giui		hyyiuh		")<<endl;
cout<<"2TRUN:::"<<fa->truncate_whitespace("guy	hiuh      hiui877")<<endl;
cout<<"3TRUN:::"<<fa->truncate_whitespace("#$%^&*SDFGH   t786 * T*		J	")<<endl;
*/
	cout << "========================================================" << endl;
	cout << "Note that the logic values are represented every ";
	cout << file_analyzer::MEASURE_OF_TIME << file_analyzer::UNIT_OF_TIME;
	cout << " in time." << endl;
	cout << "" << endl;
	cout << "Definition for logic values of signals:" << endl;
	cout << "Logic zero: "<< signalZ::LOGIC_ZERO << endl;
	cout << "Logic one: "<< signalZ::LOGIC_ONE << endl;
	cout << "Logic at high impedance: "<< signalZ::LOGIC_HIGH_IMPEDANCE << endl;
	cout << "Logic is undefined: "<< signalZ::LOGIC_UNDEFINED << endl;
	cout << "========================================================" << endl;
	
	
	// End of main function...
	return 0;
}