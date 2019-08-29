/**
 * This is written by Zhiyang Ong
 *
 * This Class ViolatedAssertion is thrown when an assertion is violated
 *
 * Exceptions can be thrown and caught to deal with the problem, while
 * assert statements will terminate the program.
 */
#include <iostream>
#include <string>
#include "ViolatedAssertion.h"
using namespace std;

ViolatedAssertion::ViolatedAssertion() {
	cout << "An assertion has been violated" << endl;
}
		

ViolatedAssertion::ViolatedAssertion(string error_message) {
	cout << "An assertion has been violated" << endl;
	cout << error_message << endl;
}
