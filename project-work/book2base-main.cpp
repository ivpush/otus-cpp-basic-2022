// book2base - проектная работа для OTUS
//

#include <iostream>
#include <filesystem>

#include <chrono>   // библиотека для работы с метками времени 

#include "book2base.hpp"

namespace fs = std::filesystem;

bool book_processing (sqlite3* db, const char* fn);

void usage (char * progPath)
{
    std::cerr << "\nUsage: " << std::filesystem::path(progPath).stem().string() << " db-pathname [input-pathname ...] \n";
    std::cerr << "       By default the current directory is uaed as input_pathname.\n" << std::endl;
}

int main (int argc, char ** argv)
{
  	setlocale (LC_ALL, "rus");   // for cyrillics 

    if (argc < 2)       // if no parms then print out usage message
    {
        usage (argv[0]);
        return 1;
    }

    sqlite3 *db = nullptr;          // data base

    char *	dbName = argv [1];
    int rc = 0;

    if (db_open (dbName, &db))       // db open failed
    {
      std::cerr << "ERROR! Can't open database: " <<  dbName << "\n";
      std::cerr << "[ " << db_error (db) << " ]" << std::endl;
      db_close (db);
      return 2;
    }

    int nCount = 0;
    auto startTime = std::chrono::system_clock::now();  // текущее время

    if ( argc < 3)    // there are argv[0] and argv[1] only - default current directory
    {
        std::cerr << "Use current directory by default." << std::endl;
        for (const auto &entry : fs::recursive_directory_iterator (".")) 
        {
            if (entry.path().extension() == ".fb2") 
            {
               book_processing (db, entry.path().string().c_str());
               ++nCount;
            }
        }
    }
    else      // there are more parms - pathnames
    {
        fs::path p;
        fs::file_status st;  

        for ( int i = 2; i < argc; ++i)
        {
          p = argv [i];

          if (fs::exists(p))                           // if exists
          {
             st = fs::status (p); 
             if (fs::is_regular_file (st)              // is regular
              && p.extension() == ".fb2")              // is fb2 file
              {
                book_processing (db, p.string().c_str());
                ++nCount;
              }
             else if (fs::is_directory (st)) 
             {
                for (const auto &entry : fs::recursive_directory_iterator (p)) 
                {
                   if (entry.path().extension() == ".fb2") 
                   {
                      book_processing (db, entry.path().string().c_str());
                      ++nCount;
                   }
                }
             }
          }
          else 
             std::cerr << "File " << p << " doesn't exists." << std::endl;  
        }
    }
    
    auto endTime = std::chrono::system_clock::now();    // текущее время
    auto millis  = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);    // получаем значение разницы в миллисекундах

    std::cerr << "Number of performed books: " << nCount << ".\n";
    std::cerr << "Elapsed time is " << millis.count() << " milliseconds" << std::endl;

    db_close (db);

    return rc;
}