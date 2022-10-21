// OTUS C++ Developer basic course
// homework #8
// - Read files and prints top k word by frequency
//

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include <chrono>
#include <filesystem>
#include <thread>

#include <future>
#include <string>
#include <locale>

const size_t TOPK = 10;

using Counter = std::map<std::string, std::size_t>;

// set string into lower case
//
inline std::string to_lower (
    std::string str      // string to be set in lower case
  ) 
{
    std::transform (str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// count words in stream
//
void count_words (
      char * fName              // input file name
    , Counter * pCounter
  ) 
{
    std::ifstream stream {fName};
    if (!stream.is_open())      // throw an exception if cannot open file
    {
        throw std::runtime_error{"Failed to open"};
    }

    std::for_each (std::istream_iterator<std::string>(stream),
                   std::istream_iterator<std::string>(),
                   [&pCounter](const std::string &s) 
                        { 
                            ++(*pCounter)[to_lower(s)];
                        }
                  );    
}

// print top k counters
//
void print_topk (
    std::ostream& stream        // the stream to output to
  , const Counter& counter      // counter - map (word, count)
  , const size_t k              // number of top counting words to be outputted
  ) 
{
    std::vector<Counter::const_iterator> words;
    words.reserve (counter.size());
    for (auto it = std::cbegin(counter); it != std::cend(counter); ++it) 
        words.emplace_back (it);

    std::partial_sort (         // partial sort to get k most frequent words
        std::begin(words), std::begin(words) + k, std::end(words),
        [](auto lhs, auto &rhs) { return lhs->second > rhs->second; }
      );

    std::for_each (             // print out k most frequent words
        std::begin(words), std::begin(words) + k,
        [&stream](const Counter::const_iterator &pair) { stream << std::setw(6) << pair->second << " " << pair->first << '\n'; }
      );
}

// usage
//
inline void usage (char * progPath)
{
    std::cerr << "\nUsage: " << std::filesystem::path(progPath).stem().string() << " [FILES...]\n" << std::endl;
}

// main 
//
int main (int argc, char *argv[]) 
{
    if (argc < 2) 
    {
        usage (argv[0]);
        return EXIT_FAILURE;
    }

    std::vector<std::future<void>> tTask;   // counter tasks - one task per file
    tTask.reserve (argc - 1);               // constrained by number of parameters
    std::vector<Counter *> tDict;           // Dict - one Dict per file
    tDict.reserve (argc - 1);               // constrained by number of parameters

    auto start = std::chrono::high_resolution_clock::now ();     // start time = current time
    
    for (int i = 1; i < argc; ++i)          // start a task for each files 
    {
        tDict.emplace_back (new Counter);   // get a new dict for a new task
        tTask.emplace_back (std::async (std::launch::async, count_words, argv[i], tDict[i-1]));  // start a new task
    }

    --argc;     // number of file names 

    Counter freq_dict;                      // total words counter 
    bool success = true;
    std::vector<std::string> filesFailed;   // vector to accumulate failed files

    for (int i = 0; i < argc; ++i)          // go through all tasks
    {
        try {
            tTask[i].get();              // wait for task completion
        } catch (std::exception &e) {
            filesFailed.emplace_back (std::string(argv[i+1]) + " - " + e.what()); 
            success = false;
        }
        if (success)
            for (const auto& [word, count] : *tDict[i])     // accumulate words counts
                freq_dict [word] += count;

        delete tDict[i];        // release memory
    }

    if (success)    // print out the results
    {
        print_topk (std::cout, freq_dict, TOPK);

        auto end = std::chrono::high_resolution_clock::now ();       // end time = current time
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds> (end - start);
        std::cout << argc << " file(s) processed for elapsed time of " << elapsed_ms.count () << " microseconds\n";
    }
    else            // print out the list of failed files
    {
        std::cerr << std::endl << std::filesystem::path(argv[0]).stem().string() << ": failed with counting for following files:";

        for (const auto& f : filesFailed) 
            std::cerr << "\n\t" << f;
        std::cerr << "\n" << std::endl;
    }

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
