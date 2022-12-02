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

    std::string genreTmp;
	sItem.erase();
    for (xml_node genre = doc.child("FictionBook").child("description").child("title-info").child("genre").first_child(); genre; genre = genre.next_sibling())
	{
        sItem = genre.text().as_string();
    	checkIt (genreTmp);
		book_genre.push_back (genreTmp);
		genreTmp.clear();
	}

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
recAuthor getAuthor (fb2info& fb2)
{
    return recAuthor (fb2.get_auth_firstname(), fb2.get_auth_midname(), fb2.get_auth_lastname());
}

// create recAuthor (Translator) from fb2 info
//
recAuthor getTranslator (fb2info& fb2)
{
    return recAuthor (fb2.get_tran_firstname(), fb2.get_tran_midname(), fb2.get_tran_lastname());
}
