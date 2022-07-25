// OTUS C++ basic course homework#3
// main for "guess number" game program

#include <iostream>
#include <string>

#include "guess-the-number.h"

// usage() - prints out help info.
//

void usage(char * prog)
{
  std::string strProg = prog;

  std::size_t pos = strProg.find_last_of ("/\\");

  std::cout << std::endl << "Usage: " << std::endl << "\t" << strProg.substr(pos+1) << " [[-max <max number>] | [-table] | [-level 1|2|3]]" << std::endl << std::endl;
}

// the main function for "guess the number" game
//
// command line parameters
// guess_the_number [[-max <max number>] | [-table] | [-level 1|2|3]]
//

int main (
    int argc        // number of args
  , char **argv     // args itself
) 
{
    std::string parm;
    int max_bound = 100;    // default max bound for number range
    bool max_is_setup = false;

    for (int i = 1; i < argc; ++i)
    {
        parm = argv[i];
        if (parm == "-max")    // this is '-max' parameter
        {
            if (max_is_setup || argc < i + 2)  // not enough number of parms
            {                                  // or -level already presents 
                usage (argv[0]);
                return (1);  
            }
            ++i;
            max_bound = std::atoi (argv[i]);
            max_is_setup = true;
        }  
        else if (parm == "-table")    // this is '-table' parameter
        {
            print_table ();
            return (0);
        }  
        else if (parm == "-level")    // this is '-level' parameter
        {
            if (max_is_setup || argc < i + 2)  // not enough number of parms
            {                                  // or -max already presents 
                usage (argv[0]);
                return (1);  
            }
            ++i;
            switch (std::atoi (argv[i]))
            {
              case 1:
                max_bound = 10;
                break;
              case 2:
                max_bound = 50;
                break;
              case 3:
                max_bound = 100;
                break;
              default:
                std::cerr << "Error: wrong level is specified: " << argv[i] << "." << std::endl;
                std::cerr << "Changed to default value of 3." << std::endl;
                break;
            }
            max_is_setup = true;
        }  
        else    // wrong parameter
        {
            usage (argv[0]);
            return (1);  
        }
    }

    play_the_game (max_bound);    // play the game
    
    print_table ();               // print out the best results

    return 0;
}
