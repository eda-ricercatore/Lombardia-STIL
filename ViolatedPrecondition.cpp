/**
 * This is written by Zhiyang Ong
 *
 * This Class ViolatedPrecondition is thrown when a precondition is violated
 */
#include <iostream>
#include <string>
#include "ViolatedPrecondition.h"
using namespace std;

ViolatedPrecondition::ViolatedPrecondition() {
	cout << "An precondition has been violated" << endl;
}
		

ViolatedPrecondition::ViolatedPrecondition(string error_message) {
	cout << "An precondition has been violated" << endl;
	cout << error_message << endl;
}
