// OTUS C++ basic course homework#3
// high score functions for "guess number" game program

#include <iostream>
#include <fstream>

#include <string>

#include "guess-the-number.h"


// print_table ()
//	simply prints out the contents of high scores file.

int print_table ()
{
	std::ifstream in_file {high_scores_filename, std::ios_base::in};
	if (!in_file.is_open ()) 
	{
		std::cout << "Failed to open file for read: " << high_scores_filename << "!" << std::endl;
		return -1;
	}

	std::cout << "High scores table:" << std::endl;

	std::string sBuff;
	int			nBuff;

    while (true) 
	{
		// Read the username & high_score
		in_file >> sBuff;		// read username
		in_file >> nBuff;		// read high score
		in_file.ignore ();		// skip EOL

		if (in_file.fail ()) 
			break;

		// Print the information to the screen
		std::cout << sBuff << '\t' << nBuff << std::endl;
	}

    return 0;
}

// find_record (name, score)
// 		finds a high score record for specified name.
// 		returns an output parameter of current value of high score for specified name.
// 		returns streamoff of found record in gigh scores file.

std::streamoff find_record (std::string& name, int& nScore)
{
    nScore = INT_MAX;               // init with maximum int value

	std::ifstream in_file {high_scores_filename, std::ios_base::in};
	if (!in_file.is_open ()) 
		return 0;

	std::streamoff  nFind;
	std::string		sBuff;		// buffer for username
	int				nBuff;		// buffer for high score

    while (true) 
    {
        nFind = (std::streamoff) in_file.tellg ();
		// Read the username & high_score
		in_file >> sBuff;		// read username
		in_file >> nBuff;		// read high score
		in_file.ignore ();		// skip EOL

		if (in_file.fail ()) 
			break;

        if (sBuff == name)		// Found!!!!
        {
            nScore = nBuff;		// save previous value fromtable
            break;
        }
	}

	in_file.close ();

    return nFind;
}

// update_record (name, score)
//		finds a high score record for specified name.
//		if found then 
//			compare high score with found value.
//			if high score less then found value then
//				update record with better value.
//		if not found then
//			append a new record with specified values.

bool update_record (std::string& user, int score)
{
    int bestScore;
    std::streamoff nCurrPosInTable = find_record (user, bestScore);		// find previous value in table

    if (score >= bestScore)     // not best
        return false;           // nothing to update

	std::fstream out_file {high_scores_filename, std::ios_base::in | std::ios_base::out};
	if (!out_file.is_open ()) 	// maybe file doen't exist
    {
		out_file.open (high_scores_filename, std::ios_base::trunc | std::ios_base::out);
		if (!out_file.is_open())
		{
			std::cout << "Failed to open file for write: " << high_scores_filename << "!" << std::endl;
			return false;
		}
	}
	else		// file exists
	{
		// Update results with new values
    	if (bestScore == INT_MAX)   // no result in table - append to end of file
    		out_file.seekp (0, std::ios::end);
		else						// update existed record
    		out_file.seekp (nCurrPosInTable, std::ios::beg);
	}

	out_file << user << " ";
	out_file.width(4);
	out_file << score << std::endl;

	out_file.close ();

    return true;
}
