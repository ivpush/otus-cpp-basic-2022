// book2base - проектная работа для OTUS
//
// Function to process 1 book
//

#include <string>
#include <filesystem>

#include "sqlite3.h"

#include "book2base.hpp"

bool auTableCreated = false;
bool bkTableCreated = false;
bool anTableCreated = false;

bool book_processing (sqlite3 *db, const char* fileName)
{
    fb2info info (fileName);

    if (!info.isValid())
        return false;

    // here we have a valid book data 
    recAuthor   author = getAuthor (info);  // Author information

    if (!auTableCreated)        // table has not been created
    {
        if (author.table_create (db)) 
            return false;
        auTableCreated = true;
    }

    if (author.recRead (db))    // not found
        author.recWrite (db);   // add a new rec

    recBook     book (info);    // Book information

    book.setAuthorId (author.getId());  // set Author Id for book
        
    // translator - a kind of authors
    recAuthor   translator = getTranslator (info);

    if (translator.valid())             // has valid fields
    {
        if (translator.recRead (db))    // not found
            translator.recWrite (db);   // add a new rec

        book.setTranslId (translator.getId());  // set Translator Id for book
    }

    if (!bkTableCreated)            // table has not been created
    {
        if (book.table_create (db))
            return false;
        bkTableCreated = true;
    }

    if (book.recRead (db))          // not found
        book.recWrite (db);         // add a new rec

    recAnnotation   anno (info);    // Annotation

    anno = book;                    // book_id

    if (!anTableCreated)            // table has not been created
    {
        if (anno.table_create (db))
            return false;
        anTableCreated = true;
    }

    if (anno.valid())               // has valid fields
    {
        if (anno.recRead (db))      // not found
            anno.recWrite (db);     // add a new rec
    }

    return true;
}
