// OTUS C++ basic course homework#3
// function for "guess number" game program

#include <iostream>
#include <string>

#include "guess-the-number.h"

// play_the_game (int max_bound);
//  max_bound - max bound of number range

void play_the_game (
    int max_bound           // max bound of number range
)
{
    std::string usrName;
    int         target_value;   // value to guess
    int         current_value;  // current value 
    int         nAttempts;      // current number of attempts

	std::srand (static_cast<int>(std::time (nullptr))); // use current time as seed for random generator

	target_value = std::rand () % max_bound;     // limit value by max bound

    std::cout << "Hi! Enter your name, please: ";
    std::getline (std::cin, usrName);			
	if (usrName.empty())				// empty input
	{
		std::cerr << "Empty input, user name is set to Unknown." << std::endl;
		usrName = "Unknown";			// set default name
	}
	else		// usrName can contain blank spaces - translate all blank spaces into underscore
	{
		for (size_t i = 0, n = usrName.size (); i < n; ++i)	// translate all blank spaces into underscore
	    	if (usrName [i] == ' ')
		    	usrName [i] = '_';
	}

	for (nAttempts = 1; ; ++nAttempts) 
    {
    	std::cout << "Enter your guess: " << std::endl;

		std::cin >> current_value;

		if (current_value < target_value) 
        {
			std::cout << "greater than entered " << current_value << std::endl;
		}
		else if (current_value > target_value) 
        {
			std::cout << "less than entered " << current_value << std::endl;
		}
		else 	// Bingo!!!
        {
			std::cout << "you win! attempts = " << nAttempts << std::endl;
			break;
		}
	};

    update_record (usrName, nAttempts);
}