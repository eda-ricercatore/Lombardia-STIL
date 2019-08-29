/**
 * This is written by Zhiyang Ong
 *
 * This Class ViolatedPostcondition is thrown when a postcondition is violated
 */
#include <iostream>
#include <string>
using namespace std;

class ViolatedPostcondition {
	public:
		ViolatedPostcondition();		
		ViolatedPostcondition(string error_message);
};
