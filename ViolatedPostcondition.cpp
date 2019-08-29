/**
 * This is written by Zhiyang Ong
 *
 * This Class ViolatedPostcondition is thrown when a postcondition is violated
 */
#include <iostream>
#include <string>
#include "ViolatedPostcondition.h"
using namespace std;

ViolatedPostcondition::ViolatedPostcondition() {
	cout << "An postcondition has been violated" << endl;
}
		

ViolatedPostcondition::ViolatedPostcondition(string error_message) {
	cout << "An postcondition has been violated" << endl;
	cout << error_message << endl;
}
