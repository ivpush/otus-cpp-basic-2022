// book2base - проектная работа для OTUS
//
// Functions for classes:
// - fb2info
// - recAuthor
// - recBook
// - recAnnotation
//

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "pugixml.hpp"

#include "sqlite3.h"

#include "book2base.hpp"
#include "utils.h"

using namespace pugi;

#include <string>

// strings' trimming
//

inline std::string trim (std::string& str)
{
	const char* ws = " \t\n\r\f\v";
    str.erase (str.find_last_not_of (ws) + 1);	//suffixing spaces
    str.erase (0, str.find_first_not_of (ws));  //prefixing spaces
    return str;
}

// db main functions
//
int db_open (const char * dbName, sqlite3** db)
{
	int rc = sqlite3_open (dbName, db);
    char* messageError;

    // for optimization - speed up writing to Database
    //
    rc = sqlite3_exec (*db, "PRAGMA synchronous = OFF", NULL, NULL, &messageError);
  
    if (rc != SQLITE_OK) {
        std::cerr << "Error to set Pragma synchronous Off" << std::endl;
        std::cerr << messageError << std::endl;
        sqlite3_free (messageError);
    }

	return rc;
}

int db_close (sqlite3 * db)
{
    return sqlite3_close (db);
}

const char * db_error (sqlite3 * db)
{
    return sqlite3_errmsg (db);
}

// functions for reading data from db

using Record  = std::vector<std::string>;
using Records = std::vector<Record>;

int select_callback (void *p_data, int num_fields, char **p_fields, char **p_col_names)
{
	(void*) p_col_names;		// to avoid 'unused variable' warning
	Records* records = static_cast<Records*>(p_data);

//	std::cerr << "Found num_fields = " << num_fields << std::endl;

  	try {
    	records->emplace_back(p_fields, p_fields + num_fields);
  	}
  	catch (...) {
    	// abort select on failure, don't let exception propogate thru sqlite3 call-stack
    	return 1;
  	}
  	return 0;
}

// Records select_stmt (sqlite3 * db, const char* stmt)
// {
//   Records records;
//   char *errmsg = nullptr;
//   int ret = sqlite3_exec(db, stmt, select_callback, &records, &errmsg);
//   if (ret != SQLITE_OK) {
//     std::cerr << "Error in select statement " << stmt << "[" << errmsg << "]\n";
//   }

//   return records;
// }

// ----- fb2info ---------------------------------------------------------------------------------------

bool fb2info::read (const char * filename)
{
	xml_document doc;
    
    pugi::xml_parse_result result = doc.load_file (filename, pugi::parse_default | pugi::parse_pi);

	if (result.status != status_ok) 
    {
        std::cerr << "Cannot load file - " << filename << std::endl;
        std::cerr << "Error description: " << result.description() << "\n";
        std::cerr << "Error offset: " << result.offset << " (error at [..." << result.offset << "]\n\n";
        return valid = false;
    }

    std::string sItem;
    unsigned char    utfBuf [maxLineLen];        // buffer for utf8 

    auto checkIt {
      [&](std::string& str)
	  {
        if (!sItem.empty())		// has some value
		{       
            if (result.encoding == encoding_win1251)    // decode into utf8
			{       
			    cp2utf ((unsigned char*)sItem.c_str(), utfBuf);
		        str = (char*) utfBuf;
            }
            else
                str = sItem;
			trim (str);	
        }
        else    // no value -> clear it
            str.clear();
      }
    };

    sItem = doc.child("FictionBook").child("description").child("title-info").child("book-title").text().as_string();
    checkIt (book_title);

    sItem = doc.child("FictionBook").child("description").child("title-info").child("author").child("first-name").text().as_string();
    checkIt (author_first_name);
    
    sItem = doc.child("FictionBook").child("description").child("title-info").child("author").child("middle-name").text().as_string();
    checkIt (author_middle_name);

    sItem = doc.child("FictionBook").child("description").child("title-info").child("author").child("last-name").text().as_string();
    checkIt (author_last_name);

//    sItem = doc.child("FictionBook").child("description").child("title-info").child("genre").text().as_string();

    sItem = doc.child("FictionBook").child("description").child("title-info").child("lang").text().as_string();
    checkIt (lang);

    sItem = doc.child("FictionBook").child("description").child("title-info").child("src-lang").text().as_string();
    checkIt (src_lang);

    sItem.erase();
    for (xml_node anno = doc.child("FictionBook").child("description").child("title-info").child("annotation").first_child(); anno; anno = anno.next_sibling())
        sItem += anno.text().as_string();
    checkIt (annotation);

    sItem = doc.child("FictionBook").child("description").child("title-info").child("translator").child("first-name").text().as_string();
    checkIt (translator_first_name);

    sItem = doc.child("FictionBook").child("description").child("title-info").child("translator").child("middle-name").text().as_string();
    checkIt (translator_middle_name);

    sItem = doc.child("FictionBook").child("description").child("title-info").child("translator").child("last-name").text().as_string();
    checkIt (translator_last_name);

    sItem = doc.child("FictionBook").child("description").child("title-info").child("date").text().as_string();
    checkIt (date);

//    sItem = doc.child("FictionBook").child("description").child("publish-info").child("publisher").text().as_string();

    sItem = doc.child("FictionBook").child("description").child("publish-info").child("year").text().as_string();
    checkIt (year);

	std::filesystem::path p = filename;
	p = std::filesystem::absolute(p);
    cp2utf ((unsigned char*) p.string().c_str(), utfBuf);
	location = (char *) utfBuf;

    return valid = true;
}

// ----- recAuthor ---------------------------------------------------------------------------------------

// create recAuthor (Author) from fb2 info
//
recAuthor getAuthor ( const fb2info& fb2)
{
    return recAuthor (fb2.author_first_name, fb2.author_middle_name, fb2.author_last_name);
}

// create recAuthor (Translator) from fb2 info
//
recAuthor getTranslator (const fb2info& fb2)
{
    return recAuthor (fb2.translator_first_name, fb2.translator_middle_name, fb2.translator_last_name);
}

// create authors table
//
int	recAuthor::table_create (sqlite3* db)
{
	char * sql = "CREATE TABLE IF NOT EXISTS Authors ("
       	         "id             INTEGER PRIMARY KEY   AUTOINCREMENT, "
           	     "firstname      TEXT  NOT NULL, "
               	 "middlename     TEXT, "
           	     "lastname       TEXT  NOT NULL "
				 " );" ;

   	int exit = 0;
   	char* messageError = nullptr;

    exit = sqlite3_exec (db, sql, NULL, 0, &messageError);
  
   	if (exit != SQLITE_OK) {
       	std::cerr << "Error Create Authors Table" << std::endl;
       	std::cerr << messageError << std::endl;
       	sqlite3_free (messageError);
   	}
    
   	return exit;
}

// write author info into database
//
int	recAuthor::recWrite (sqlite3 * db)
{
   	int exit = 0;

	const char* sql = "insert into Authors (firstname, middlename, lastname) values (?, ?, ?)";
	sqlite3_stmt *stmt = nullptr;

   	exit = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_text (stmt, 1, au_firstname.c_str(), -1, SQLITE_TRANSIENT);
   	sqlite3_bind_text (stmt, 2, au_midname.c_str(), -1, SQLITE_TRANSIENT);
   	sqlite3_bind_text (stmt, 3, au_lastname.c_str(), -1, SQLITE_TRANSIENT);

   	exit = sqlite3_step (stmt);

   	sqlite3_reset (stmt);

   	if (exit != SQLITE_OK && exit != SQLITE_DONE) 
	{
		char * sql_cmd = sqlite3_expanded_sql (stmt);
		std::cerr << "ERROR! [ " << exit << " ]. Insert command is:" << std::endl;
		std::cerr <<  sql_cmd << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
       	std::cerr << "Error Insert into Authors Table" << std::endl;
		sqlite3_free (sql_cmd);
   	}

   	sqlite3_finalize (stmt);
  
	exit = recRead (db);

   	return exit;
}

// read author info from database
//
int	recAuthor::recRead (sqlite3 * db)
{
   	int exit = 0;

	const char* sql = 				// recAuthor should have non-empty firstname and lastname 
		au_midname.empty() ?		//	middlename may be omitted / empty
			"select * from Authors where firstname = ? and lastname = ?" :
			"select * from Authors where firstname = ? and middlename = ? and lastname = ?";
	sqlite3_stmt *stmt = nullptr;

   	exit = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_text (stmt, 1, au_firstname.c_str(), -1, SQLITE_TRANSIENT);
	if (!au_midname.empty())		// there is a valid middlename
	{
   		sqlite3_bind_text (stmt, 2, au_midname.c_str(), -1, SQLITE_TRANSIENT);
   		sqlite3_bind_text (stmt, 3, au_lastname.c_str(), -1, SQLITE_TRANSIENT);
	}
	else 	// middlename is empty
   		sqlite3_bind_text (stmt, 2, au_lastname.c_str(), -1, SQLITE_TRANSIENT);

	Records records;		// select result
	char *errmsg;

	char * sql_cmd = sqlite3_expanded_sql (stmt);

	exit = sqlite3_exec (db, sql_cmd, select_callback, &records, &errmsg);
	if (exit != SQLITE_OK) 
	{
		std::cerr << "ERROR while Select command execution:" << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "Error is [" << errmsg << "]" << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
		sqlite3_free (errmsg);
   	}

	sqlite3_free (sql_cmd);
   	sqlite3_finalize (stmt);		// destroy stmt object
  
	if (exit == SQLITE_OK)		// && records.size() >= 1)
	{
		if (records.size() > 0)
		{
			au_id        = atoi (records[0].at(0).c_str());
			au_firstname = records[0].at(1);
			au_midname   = records[0].at(2);
			au_lastname  = records[0].at(3);
		}
		else 
			exit = SQLITE_NOTFOUND;
	}

   	return exit;
}

// ----- recBook ---------------------------------------------------------------------------------------

// create books table
//
int	recBook::table_create (sqlite3* db)
{
	char * sql = "CREATE TABLE IF NOT EXISTS Books ("
       	         "id            INTEGER PRIMARY KEY AUTOINCREMENT, "
           	     "id_author 	INTEGER  NOT NULL, "
               	 "title         TEXT     NOT NULL, "
                 "lang          TEXT, "
                 "src_lang      TEXT, "
          	     "id_translator INTEGER, "
                 "year			INTEGER, "
				 "location		TEXT"
				 " );";

   	int exit = 0;
   	char* messageError = nullptr;

    exit = sqlite3_exec (db, sql, NULL, 0, &messageError);
  
    if (exit != SQLITE_OK) 
	{
       	std::cerr << "Error Create Books Table" << std::endl;
       	std::cerr << messageError << std::endl;
       	sqlite3_free (messageError);
   	}
    
   	return exit;
}

// write book info into database
//
int	recBook::recWrite (sqlite3 * db)
{
   	int exit = 0;

	const char* sql = "insert into Books (id_author, title, lang, src_lang, id_translator, year, location) values (?, ?, ?, ?, ?, ?, ?)";
	sqlite3_stmt *stmt = nullptr;

   	exit = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_int  (stmt, 1, book_au_id);
   	sqlite3_bind_text (stmt, 2, book_title.c_str(), -1, SQLITE_TRANSIENT);
   	sqlite3_bind_text (stmt, 3, book_lang.c_str(), -1, SQLITE_TRANSIENT);
   	sqlite3_bind_text (stmt, 4, book_src_lang.c_str(), -1, SQLITE_TRANSIENT);
   	sqlite3_bind_int  (stmt, 5, book_transl_id);
   	sqlite3_bind_int  (stmt, 6, book_year);
   	sqlite3_bind_text (stmt, 7, book_path.c_str(), -1, SQLITE_TRANSIENT);

   	exit = sqlite3_step  (stmt);
   	sqlite3_reset (stmt);
  
   	if (exit != SQLITE_OK && exit != SQLITE_DONE) 
	{
		char * sql_cmd = sqlite3_expanded_sql (stmt);
		std::cerr << "Insert command string is:" << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
       	std::cerr << "Error Insert into Books Table" << std::endl;
		sqlite3_free (sql_cmd);
   	}

   	sqlite3_finalize (stmt);

	exit = recRead (db);

   	return exit;
}

// read book info from database
//
int	recBook::recRead (sqlite3 * db)
{
   	int exit = 0;

	const char* sql = book_transl_id == 0 ?
			"select * from Books where id_author = ? and title = ? " :
			"select * from Books where id_author = ? and title = ? and id_translator = ?";
	sqlite3_stmt *stmt = nullptr;

   	exit = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_int  (stmt, 1, book_au_id);
	sqlite3_bind_text (stmt, 2, book_title.c_str(), -1, SQLITE_TRANSIENT);
	if (book_transl_id != 0)		// there is a valid translator id
   		sqlite3_bind_int (stmt, 3, book_transl_id);

	char*	sql_cmd = sqlite3_expanded_sql (stmt);

	Records records;		// select result
	char *errmsg = nullptr;

	exit = sqlite3_exec (db, sql_cmd, select_callback, &records, &errmsg);

	if (exit != SQLITE_OK) 
	{
		std::cerr << "ERROR while Select command execution:" << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "Error is [" << errmsg << "]" << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
   	}

	sqlite3_free (sql_cmd);

   	exit = sqlite3_finalize (stmt);		// destroy stmt object
  
	if (exit == SQLITE_OK)		// && records.size() >= 1)
	{
		if (records.size() > 0)
		{
			book_id        = atoi (records[0].at(0).c_str());
			book_au_id     = atoi (records[0].at(1).c_str());
			book_title     = records[0].at(2);
			book_lang      = records[0].at(3);
			book_src_lang  = records[0].at(4);
			book_transl_id = atoi (records[0].at(5).c_str());
			book_year      = atoi (records[0].at(6).c_str());
			book_path      = records[0].at(7);
		}
		else 
			exit = SQLITE_NOTFOUND;
	}

   	return exit;
}

// ----- recAnnotation ---------------------------------------------------------------------------------------

// create annotations table
//
int	recAnnotation::table_create (sqlite3* db)
{
	char * sql = "CREATE TABLE IF NOT EXISTS Annotations ("
       	         "id_book     INTEGER PRIMARY KEY, "
               	 "annotation  TEXT        NOT NULL "
				 " );" ;
   	int exit = 0;
   	char* messageError = nullptr;

    exit = sqlite3_exec (db, sql, NULL, 0, &messageError);
  
    if (exit != SQLITE_OK) 
	{
       	std::cerr << "Error Create Books Table" << std::endl;
       	std::cerr << messageError << std::endl;
       	sqlite3_free (messageError);
   	}
    
   	return exit;
}

// write annotation info into database
//
int	recAnnotation::recWrite (sqlite3 * db)
{
   	int exit = 0;

	const char* sql = "insert into Annotations (id_book, annotation) values (?, ?)";
	sqlite3_stmt *stmt = nullptr;

   	exit = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);
   	sqlite3_bind_int  (stmt, 1, anno_book_id);
   	sqlite3_bind_text (stmt, 2, anno_text.c_str(), -1, SQLITE_TRANSIENT);
   	exit = sqlite3_step (stmt);

   	sqlite3_reset (stmt);

   	sqlite3_finalize (stmt);
  
   	if (exit != SQLITE_OK && exit != SQLITE_DONE) 
	{
       	std::cerr << "Error Insert into Annotations Table" << std::endl;
   	}
    
   	return exit;
}

// read annotation info from database
//
int	recAnnotation::recRead (sqlite3 * db)
{
   	int exit = 0;

	const char* sql = "select * from Annotations where id_book = ?";
	sqlite3_stmt *stmt = nullptr;

   	exit = sqlite3_prepare_v2 (db, sql, -1, &stmt, 0);

   	sqlite3_bind_int  (stmt, 1, anno_book_id);

	char*	sql_cmd = sqlite3_expanded_sql (stmt);

	Records records;		// select result
	char *errmsg;

	exit = sqlite3_exec (db, sql_cmd, select_callback, &records, &errmsg);

	if (exit != SQLITE_OK) 
	{
		std::cerr << "ERROR while Select command execution:" << std::endl;
		std::cerr << sql_cmd << std::endl;
		std::cerr << "Error is [" << errmsg << "]" << std::endl;
		std::cerr << "----------------------------------------------------------------" << std::endl;
   	}

	sqlite3_free (sql_cmd);

   	exit = sqlite3_finalize (stmt);		// destroy stmt object
  
	if (exit == SQLITE_OK)		// && records.size() >= 1)
	{
		if (records.size() > 0)
		{
			anno_book_id  = atoi (records[0].at(0).c_str());
			anno_text     = records[0].at(1);
		}
		else 
			exit = SQLITE_NOTFOUND;
	}

   	return exit;
}

