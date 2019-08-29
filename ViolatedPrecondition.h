/**
 * This is written by Zhiyang Ong
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
