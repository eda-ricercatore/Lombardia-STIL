/**
 * This is written by Zhiyang Ong, student number 6004-9194-12,
 * for EE 658 Homework #4
 *
 * This Class ViolatedAssertion is thrown when an assertion is violated
 *
 * Exceptions can be thrown and caught to deal with the problem, while
 * assert statements will terminate the program.
 */
#include <iostream>
#include <string>
using namespace std;

class ViolatedAssertion {
	public:
		ViolatedAssertion();		
		ViolatedAssertion(string error_message);
};