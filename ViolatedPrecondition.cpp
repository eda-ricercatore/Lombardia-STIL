/**
 * This is written by Zhiyang Ong, student number 6004-9194-12,
 * for EE 658 Homework #4
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