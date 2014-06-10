/**
 * This is written by Zhiyang Ong, student number 6004-9194-12,
 * for EE 658 Homework #4
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