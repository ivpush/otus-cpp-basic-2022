// book2base - проектная работа для OTUS
//
// Function to process 1 book
//

#include <string>
#include <filesystem>

#include <iostream>

#include "book2base.hpp"
#include "database.hpp"

bool book_processing (iDatabase *db, const char* fileName)
{
    fb2info info (fileName);

    if (!info.isValid())
        return false;

    // here we have a valid book data 
    recAuthor   author = getAuthor (info);  // Author information

    if (db->readAuthor(author))     // not found
        db->addAuthor(author);      // add a new rec

    recBook     book (info);        // Book information

    book.set_auth_id (author.get_id());  // set Author Id for book
        
    // translator - a kind of authors
    recAuthor   translator = getTranslator (info);

    if (translator.valid())             // has valid fields
    {
        if (db->readAuthor(translator)) // not found
            db->addAuthor(translator);  // add a new rec

        book.set_transl_id (translator.get_id());  // set Translator Id for book
    }

    if (db->readBook (book))    // not found
        db->addBook (book);     // add a new rec

    recAnnotation   anno (info);    // Annotation

    anno.set_book_id (book.get_id());

    if (anno.valid())               // has valid fields
    {
        if (db->readAnnotation (anno))  // not found
            db->addAnnotation (anno);   // add a new rec
    }

    for (std::string sGenre : info.get_genre())
    {
        recGenre        genre (sGenre.c_str());      // Genre

        if (db->readGenre (genre))      // not found
            db->addGenre (genre);       // add a new rec

        recBookGenre    bkGenre (book.get_id(), genre.get_id ());  // Book genre

        if (bkGenre.valid())
        {
            if (db->readBookGenre (bkGenre))    // not found
                db->addBookGenre (bkGenre);     // add a new rec
        }
    }

    return true;
}
