/**
 * This is written by Zhiyang Ong, student number 6004-9194-12,
 * for EE 658 Homework #4
 *
 * This Class ViolatedPrecondition is thrown when a precondition is violated
 */
#include <iostream>
#include <string>
using namespace std;

class ViolatedPrecondition {
	public:
		ViolatedPrecondition();		
		ViolatedPrecondition(string error_message);
};