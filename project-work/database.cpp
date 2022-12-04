// database interface functions
//

#include <iostream>
#include <vector>

#include "book2base.hpp"
#include "database.hpp"

#include "sqlite3.h"

// sqlite3 interface
//

using Record  = std::vector<std::string>;
using Records = std::vector<Record>;

// function for reading data from db
//
int select_callback (void *p_data, int num_fields, char **p_fields, char **p_col_names)
{
	(void*) p_col_names;		// to avoid 'unused variable' warning
	Records* records = static_cast<Records*>(p_data);

  	try {
    	records->emplace_back(p_fields, p_fields + num_fields);
  	}
  	catch (...) {
    	// abort select on failure, don't let exception propogate thru sqlite3 call-stack
    	return 1;
  	}
  	return 0;
}

// db_open(parm) - open database
//  parm - string - database pathname
//  
int sqlite3db::db_open (const char * parm)
{
	int rc = sqlite3_open (parm, &db);

    if (rc != SQLITE_OK)        // cannot open db
    {
        std::cerr << "Cannot open database: " << parm << std::endl;
    }
    else                        // open db, try make some optimization
    {
        char* messageError = nullptr;

        // for optimization - speed up writing to Database
        //
        if (sqlite3_exec (db, "PRAGMA synchronous = OFF", NULL, NULL, &messageError) != SQLITE_OK)
        {
            std::cerr << "Error to set Pragma synchronous Off" << std::endl;
            std::cerr << messageError << std::endl;
            sqlite3_free (messageError);
        }
    
        rc = create_tables ();
    }

	return rc;
}

// db_close() - close db
//
int sqlite3db::db_close ()
{
    return sqlite3_close (db);
}

// db_error () - last error message
//
const char * sqlite3db::db_error ()
{
    return sqlite3_errmsg (db);
}

// create_tables () - create all tables
//
int sqlite3db::create_tables()
{
   	int rc = 0;
   	char* messageError = nullptr;

    // Create Authors table
    //
	char * sql = "CREATE TABLE IF NOT EXISTS Authors ("
       	         "id             INTEGER PRIMARY KEY   AUTOINCREMENT, "
           	     "firstname      TEXT  NOT NULL, "
               	 "middlename     TEXT, "
           	     "lastname       TEXT  NOT NULL "
				 " );" ;

    rc = sqlite3_exec (db, sql, NULL, 0, &messageError);
  
   	if (rc != SQLITE_OK)            // cannot create a table
    {
       	std::cerr << "Error Create Authors Table" << std::endl;
       	std::cerr << messageError << std::endl;
       	sqlite3_free (messageError);
       	return rc;
   	}

    // Create Books table
    //
	sql = "CREATE TABLE IF NOT EXISTS Books ("
          "id            INTEGER PRIMARY KEY AUTOINCREMENT, "
          "id_author 	INTEGER  NOT NULL, "
          "title         TEXT     NOT NULL, "
          "lang          TEXT, "
          "src_lang      TEXT, "
          "id_translator INTEGER, "
          "year			INTEGER, "
		  "location		TEXT"
		  " );";

    rc = sqlite3_exec (db, sql, NULL, 0, &messageError);
  
    if (rc != SQLITE_OK) 
	{
       	std::cerr << "Error Create Books Table" << std::endl;
       	std::cerr << messageError << std::endl;
       	sqlite3_free (messageError);
       	return rc;
   	}
    
    // Create Annotations table
    //
	sql = "CREATE TABLE IF NOT EXISTS Annotations ("
          "id_book     INTEGER PRIMARY KEY, "
       	  "annotation  TEXT "
		  " );" ;

    rc = sqlite3_exec (db, sql, NULL, 0, &messageError);
  
    if (rc != SQLITE_OK) 
	{
       	std::cerr << "Error Create Annotations Table" << std::endl;
       	std::cerr << messageError << std::endl;
       	sqlite3_free (messageError);
       	return rc;
   	}
    
    // Create Genre table - list of genres
    //
	sql = "CREATE TABLE IF NOT EXISTS Genres ("
          "id     INTEGER PRIMARY KEY AUTOINCREMENT, "
       	  "genre  TEXT "
		  " );" ;

    rc = sqlite3_exec (db, sql, NULL, 0, &messageError);
  
    if (rc != SQLITE_OK) 
	{
       	std::cerr << "Error Create Genres Table" << std::endl;
       	std::cerr << messageError << std::endl;
       	sqlite3_free (messageError);
       	return rc;
   	}
    
    // Create Genre-Book table - association of genre and book
    //
	sql = "CREATE TABLE IF NOT EXISTS BookGenres ("
          "id_book   INTEGER, "
       	  "id_genre  INTEGER "
		  " );" ;

    rc = sqlite3_exec (db, sql, NULL, 0, &messageError);
  
    if (rc != SQLITE_OK) 
	{
       	std::cerr << "Error Create BookGenres Table" << std::endl;
       	std::cerr << messageError << std::endl;
       	sqlite3_free (messageError);
       	return rc;
   	}

   	return rc;
}

// Authors - write, read, etc...
//

// add/write author info into database
//
int	sqlite3db::addAuthor (recAuthor& au)
{
   	int rc = 0;

	const char* sql = "insert into Authors (firstname, middlename, lastname) values (?, ?, ?)";
	sqlite3_stmt *stmt = nullptr;

   	rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_text (stmt, 1, au.get_firstname(), -1, SQLITE_TRANSIENT);
   	sqlite3_bind_text (stmt, 2, au.get_midname(), -1, SQLITE_TRANSIENT);
   	sqlite3_bind_text (stmt, 3, au.get_lastname(), -1, SQLITE_TRANSIENT);

   	rc = sqlite3_step (stmt);

   	sqlite3_reset (stmt);

   	if (rc != SQLITE_OK && rc != SQLITE_DONE) 
	{
		char * sql_cmd = sqlite3_expanded_sql (stmt);
		std::cerr << "ERROR! [ " << rc << " ]. Insert command is:" << std::endl;
		std::cerr <<  sql_cmd << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
       	std::cerr << "Error Insert into Authors Table" << std::endl;
		sqlite3_free (sql_cmd);
   	}

   	sqlite3_finalize (stmt);
  
   	return readAuthor (au);
}

// read author info from database
//
int	sqlite3db::readAuthor (recAuthor& au)
{
   	int rc = 0;

	const char* sql = 				// recAuthor should have non-empty firstname and lastname 
		*(au.get_midname()) == '\0' ?		//	middlename may be omitted / empty
			"select * from Authors where firstname = ? and lastname = ?" :
			"select * from Authors where firstname = ? and middlename = ? and lastname = ?";
	sqlite3_stmt *stmt = nullptr;

   	rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_text (stmt, 1, au.get_firstname(), -1, SQLITE_TRANSIENT);
	if (*(au.get_midname()) != 0)		// there is a valid middlename
	{
   		sqlite3_bind_text (stmt, 2, au.get_midname(),  -1, SQLITE_TRANSIENT);
   		sqlite3_bind_text (stmt, 3, au.get_lastname(), -1, SQLITE_TRANSIENT);
	}
	else 	// middlename is empty
   		sqlite3_bind_text (stmt, 2, au.get_lastname(), -1, SQLITE_TRANSIENT);

	Records records;		// select result
	char *errmsg;

	char * sql_cmd = sqlite3_expanded_sql (stmt);

	rc = sqlite3_exec (db, sql_cmd, select_callback, &records, &errmsg);
	if (rc != SQLITE_OK) 
	{
		std::cerr << "ERROR while Select command execution:" << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "Error is [" << errmsg << "]" << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
		sqlite3_free (errmsg);
   	}

	sqlite3_free (sql_cmd);
   	sqlite3_finalize (stmt);		// destroy stmt object
  
	if (rc == SQLITE_OK)		// && records.size() >= 1)
	{
		if (records.size() > 0)
		{
			au.set_id  (atoi (records[0].at(0).c_str()));
			au.set_firstname (records[0].at(1));
			au.set_midname   (records[0].at(2));
			au.set_lastname  (records[0].at(3));
		}
		else 
			rc = SQLITE_NOTFOUND;
	}

   	return rc;
}

// Books - write, read, etc...
//

// write book info into database
//
int	sqlite3db::addBook (recBook& book)
{
   	int rc = 0;

	const char* sql = "insert into Books (id_author, title, lang, src_lang, id_translator, year, location) values (?, ?, ?, ?, ?, ?, ?)";
	sqlite3_stmt *stmt = nullptr;

   	rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_int  (stmt, 1, book.get_auth_id());
   	sqlite3_bind_text (stmt, 2, book.get_title(), -1, SQLITE_TRANSIENT);
   	sqlite3_bind_text (stmt, 3, book.get_lang(), -1, SQLITE_TRANSIENT);
   	sqlite3_bind_text (stmt, 4, book.get_src_lang(), -1, SQLITE_TRANSIENT);
   	sqlite3_bind_int  (stmt, 5, book.get_transl_id());
   	sqlite3_bind_int  (stmt, 6, book.get_year());
   	sqlite3_bind_text (stmt, 7, book.get_location(), -1, SQLITE_TRANSIENT);

   	rc = sqlite3_step  (stmt);
   	sqlite3_reset (stmt);
  
   	if (rc != SQLITE_OK && rc != SQLITE_DONE) 
	{
		char * sql_cmd = sqlite3_expanded_sql (stmt);
		std::cerr << "Insert command string is:" << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
       	std::cerr << "Error Insert into Books Table" << std::endl;
		sqlite3_free (sql_cmd);
   	}

   	sqlite3_finalize (stmt);

   	return readBook (book);
}

// read book info from database
//
int	sqlite3db::readBook (recBook& book)
{
   	int rc = 0;

	const char* sql = book.get_transl_id() == 0 ?
			"select * from Books where id_author = ? and title = ? " :
			"select * from Books where id_author = ? and title = ? and id_translator = ?";
	sqlite3_stmt *stmt = nullptr;

   	rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_int  (stmt, 1, book.get_auth_id());
	sqlite3_bind_text (stmt, 2, book.get_title(), -1, SQLITE_TRANSIENT);
	if (book.get_transl_id() != 0)		// there is a valid translator id
   		sqlite3_bind_int (stmt, 3, book.get_transl_id());

	char*	sql_cmd = sqlite3_expanded_sql (stmt);

	Records records;		// select result
	char *errmsg = nullptr;

	rc = sqlite3_exec (db, sql_cmd, select_callback, &records, &errmsg);

	if (rc != SQLITE_OK) 
	{
		std::cerr << "ERROR while Select command execution:" << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "Error is [" << errmsg << "]" << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
   	}

	sqlite3_free (sql_cmd);

   	rc = sqlite3_finalize (stmt);		// destroy stmt object
  
	if (rc == SQLITE_OK)		// && records.size() >= 1)
	{
		if (records.size() > 0)
		{
			book.set_id (atoi (records[0].at(0).c_str()));
			book.set_auth_id (atoi (records[0].at(1).c_str()));
			book.set_title (records[0].at(2).c_str());
			book.set_lang (records[0].at(3).c_str());
			book.set_src_lang (records[0].at(4).c_str());
			book.set_transl_id (atoi (records[0].at(5).c_str()));
			book.set_year (atoi (records[0].at(6).c_str()));
			book.set_location (records[0].at(7).c_str());
		}
		else 
			rc = SQLITE_NOTFOUND;
	}

   	return rc;
}

// Annotations - write, read, etc...
//

// write annotation info into database
//
int	sqlite3db::addAnnotation (recAnnotation& anno)
{
   	int rc = 0;

	const char* sql = "insert into Annotations (id_book, annotation) values (?, ?)";
	sqlite3_stmt *stmt = nullptr;

   	rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);
   	sqlite3_bind_int  (stmt, 1, anno.get_book_id());
   	sqlite3_bind_text (stmt, 2, anno.get_text(), -1, SQLITE_TRANSIENT);
   	rc = sqlite3_step (stmt);

   	sqlite3_reset (stmt);

   	rc = sqlite3_finalize (stmt);
  
   	if (rc != SQLITE_OK && rc != SQLITE_DONE) 
	{
       	std::cerr << "Error Insert into Annotations Table - " << rc << std::endl;
		char * sql_cmd = sqlite3_expanded_sql (stmt);
		std::cerr << "Insert command string is:" << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
		sqlite3_free (sql_cmd);
   	}
    
   	return readAnnotation (anno);
}

// read annotation info from database
//
int	sqlite3db::readAnnotation (recAnnotation& anno)
{
   	int rc = 0;

	const char* sql = "select * from Annotations where id_book = ?";
	sqlite3_stmt *stmt = nullptr;

   	rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_int  (stmt, 1, anno.get_book_id());

	char*	sql_cmd = sqlite3_expanded_sql (stmt);

	Records records;		// select result
	char *errmsg = nullptr;

	rc = sqlite3_exec (db, sql_cmd, select_callback, &records, &errmsg);

	if (rc != SQLITE_OK) 
	{
		std::cerr << "ERROR while Select command execution: " << rc << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "Error is [" << errmsg << "]" << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
   	}

	sqlite3_free (sql_cmd);

   	rc = sqlite3_finalize (stmt);		// destroy stmt object
  
	if (rc == SQLITE_OK)		// && records.size() >= 1)
	{
		if (records.size() > 0)
		{
			anno.set_book_id (atoi (records[0].at(0).c_str()));
			anno.set_text (records[0].at(1).c_str());
		}
		else 
			rc = SQLITE_NOTFOUND;
	}

   	return rc;
}

// Genres - write, read, etc...
//

// write genre info into database
//
int	sqlite3db::addGenre (recGenre& genre)
{
   	int rc = 0;

	const char* sql = "insert into Genres (genre) values (?)";
	sqlite3_stmt *stmt = nullptr;

   	rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);
//   	sqlite3_bind_int  (stmt, 1, genre.get_id ());
   	sqlite3_bind_text (stmt, 1, genre.get_name (), -1, SQLITE_TRANSIENT);
   	rc = sqlite3_step (stmt);

   	sqlite3_reset (stmt);

   	rc = sqlite3_finalize (stmt);
  
   	if (rc != SQLITE_OK && rc != SQLITE_DONE) 
	{
       	std::cerr << "Error Insert into Genres Table - " << rc << std::endl;
		char * sql_cmd = sqlite3_expanded_sql (stmt);
		std::cerr << "Insert command string is:" << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
		sqlite3_free (sql_cmd);
   	}
    
   	return readGenre (genre);
}

// read genre info from database
//
int	sqlite3db::readGenre (recGenre& genre)
{
   	int rc = 0;

	const char* sql = "select * from Genres where genre = ?";
	sqlite3_stmt *stmt = nullptr;

   	rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_text (stmt, 1, genre.get_name (), -1, SQLITE_TRANSIENT);

	char*	sql_cmd = sqlite3_expanded_sql (stmt);

	Records records;		// select result
	char *errmsg = nullptr;

	rc = sqlite3_exec (db, sql_cmd, select_callback, &records, &errmsg);

	if (rc != SQLITE_OK) 
	{
		std::cerr << "ERROR while Select command execution: " << rc << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "Error is [" << errmsg << "]" << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
   	}

	sqlite3_free (sql_cmd);

   	rc = sqlite3_finalize (stmt);		// destroy stmt object
  
	if (rc == SQLITE_OK)		// && records.size() >= 1)
	{
		if (records.size() > 0)
		{
			genre.set_id (atoi (records[0].at(0).c_str()));
			genre.set_name (records[0].at(1).c_str());
		}
		else 
			rc = SQLITE_NOTFOUND;
	}

   	return rc;
}

// BookGenres - write, read, etc...
//

// write bookgenre info into database
//
int	sqlite3db::addBookGenre (recBookGenre& genre)
{
   	int rc = 0;

	const char* sql = "insert into BookGenres (id_book, id_genre) values (?, ?)";
	sqlite3_stmt *stmt = nullptr;

   	rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);
   	sqlite3_bind_int (stmt, 1, genre.get_book_id ());
   	sqlite3_bind_int (stmt, 2, genre.get_id ());
   	rc = sqlite3_step (stmt);

   	sqlite3_reset (stmt);

   	rc = sqlite3_finalize (stmt);
  
   	if (rc != SQLITE_OK && rc != SQLITE_DONE) 
	{
       	std::cerr << "Error Insert into BookGenres Table - " << rc << std::endl;
		char * sql_cmd = sqlite3_expanded_sql (stmt);
		std::cerr << "Insert command string is:" << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
		sqlite3_free (sql_cmd);
   	}
    
   	return readBookGenre (genre);
}

// read bookgenre info from database
//
int	sqlite3db::readBookGenre (recBookGenre& genre)
{
   	int rc = 0;

	const char* sql = "select * from BookGenres where id_book = ? and id_genre = ?";
	sqlite3_stmt *stmt = nullptr;

   	rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_int (stmt, 1, genre.get_book_id ());
   	sqlite3_bind_int (stmt, 2, genre.get_id ());

	char*	sql_cmd = sqlite3_expanded_sql (stmt);

	Records records;		// select result
	char *errmsg = nullptr;

	rc = sqlite3_exec (db, sql_cmd, select_callback, &records, &errmsg);

	if (rc != SQLITE_OK) 
	{
		std::cerr << "ERROR while Select command execution: " << rc << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "Error is [" << errmsg << "]" << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
   	}

	sqlite3_free (sql_cmd);

   	rc = sqlite3_finalize (stmt);		// destroy stmt object
  
	if (rc == SQLITE_OK)		// && records.size() >= 1)
	{
		if (records.size() > 0)
		{
			genre.set_book_id (atoi (records[0].at(0).c_str()));
			genre.set_id      (atoi (records[0].at(1).c_str()));
		}
		else 
			rc = SQLITE_NOTFOUND;
	}

   	return rc;
}

