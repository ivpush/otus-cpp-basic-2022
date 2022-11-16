// book2base - проектная работа для OTUS
//
// Classes:
// - fb2info
// - recAuthor
// - recBook
// - recAnnotation
//

#pragma once

const int   maxLineLen = 8192;              // max length for read tag

// db related info
//
struct sqlite3;

int db_open (const char * name, sqlite3** db);
int db_close (sqlite3* db);
const char * db_error (sqlite3 * db);

// class fb2info - информация из тегов fb2
//
class fb2info   {
// public:    
    // <title-info>
    std::string     author_first_name;      // <author> <first-name>
    std::string     author_middle_name;     //          <middle-name>
    std::string     author_last_name;       //          <last-name>
    std::string     book_title;             // <book-title>
    std::string     annotation;             // <annotation>
    std::string     lang;                   // <lang>
    std::string     src_lang;               // <src-lang>
    std::string     date;                   // <date>
    std::string     translator_first_name;  // <translator> <first-name>
    std::string     translator_middle_name; //              <middle-name>
    std::string     translator_last_name;   //              <last-name>
    // <publish-info>
    std::string     year;                   // <year>
    // other
    std::string     location;               // full path of book location
    bool            valid;                  // true - valid, false - invalid
public:
    fb2info();
    fb2info (const char * filename) {
        valid = read (filename);
    }
    fb2info (const fb2info & other) = delete;
    fb2info (fb2info && other) = delete;
    fb2info &operator=(const fb2info & other) = delete;
    fb2info &operator=(fb2info && other) = delete;
    ~fb2info() = default;

    bool read (const char * filename);      // read fb2 tags from file
    void clear ()       // clear all fields
    {
        author_first_name.clear();
        author_middle_name.clear();
        author_last_name.clear();
        book_title.clear();  
        annotation.clear();  
        lang.clear();        
        src_lang.clear();    
        date.clear();        
        translator_first_name.clear();
        translator_middle_name.clear();
        translator_last_name.clear();  
        year.clear();
        location.clear();
        valid = false;
    }
    bool isValid () {  return valid; }

    friend class recAuthor;
    friend class recBook;
    friend class recAnnotation;

    friend recAuthor getAuthor     (const fb2info& fb2);
    friend recAuthor getTranslator (const fb2info& fb2);
};

// class recAuthor - запись о авторе в БД
//
class recAuthor {
    int             au_id;          // author id
    std::string     au_firstname;   // author first name - имя
    std::string     au_midname;     // author middle name - отчество
    std::string     au_lastname;    // author last name - фамилия

public:
    recAuthor ();
    recAuthor (std::string fn, std::string mn, std::string ln)
    {
        au_id = 0;
        au_firstname = fn;
        au_midname   = mn;
        au_lastname  = ln;
    };
    recAuthor (const recAuthor& other) = delete;
    recAuthor (recAuthor&& other) = delete;
    ~recAuthor () = default;
    recAuthor &operator=(const fb2info& fb2)    // assign from fb2 info
    {
        au_id = 0;
        au_firstname = fb2.author_first_name;
        au_midname   = fb2.author_middle_name;
        au_lastname  = fb2.author_last_name;
    }

    int table_create (sqlite3 * db);
    int recWrite     (sqlite3 * db);
    int recRead      (sqlite3 * db);

    int getId ()     {  return au_id;  }

    void clear ()        // clear all fields
    {
        au_id = 0;
        au_firstname.clear ();
        au_midname.clear ();
        au_lastname.clear ();
    }

    bool valid ()        // validate all fields
    {
        if (au_id == 0
         && au_firstname.empty ()
         && au_midname.empty ()
         && au_lastname.empty () )
           return false;
        else
           return true;    
    }

    friend class recBook;
};

// class recBook - запись о книге в БД
//
class recBook  {
    int             book_id;        // book id
    int             book_au_id;     // book author id
    std::string     book_title;     // book title
    std::string     book_lang;      // book language
    std::string     book_src_lang;  // book source language
    int             book_transl_id; // book translater id
    int             book_year;      // book year
    std::string     book_path;      // book file pathname

public:
    recBook ();
    recBook (const recBook& other) = delete;
    recBook (recBook&& other) = delete;
    recBook (const fb2info& fb2)  // construct from fb2 info
    {
        book_id    = 0;
        book_au_id = 0;
        book_title = fb2.book_title;
        book_lang  = fb2.lang;
        book_src_lang = fb2.src_lang;
        book_transl_id = 0;
        book_year = atoi (fb2.year.c_str());
        book_path = fb2.location;
    }

    ~recBook () = default;

    int table_create (sqlite3 * db);
    int recWrite     (sqlite3 * db);
    int recRead      (sqlite3 * db);

    void setAuthorId (int idValue)
    {
        book_au_id = idValue;
    }

    void setTranslId (int idValue)
    {
        book_transl_id = idValue;
    }

    void clear ()       // clear all fields
    {
        book_id    = 0;
        book_au_id = 0;
        book_title.clear ();
        book_lang.clear ();
        book_src_lang.clear ();
        book_transl_id = 0;
        book_year = 0;
        book_path.clear ();
    }

    friend class recAnnotation;
};

// class recAnnotation - запись аннотации книги в БД
//
class recAnnotation {
    int             anno_book_id;   // book id the annotation is for
    std::string     anno_text;      // annotation itself (text) 

public:
    recAnnotation ();
    recAnnotation (const fb2info& fb2)
    {
        anno_book_id = 0;
        anno_text = fb2.annotation;
    }
    recAnnotation (const recAnnotation& other) = delete;
    recAnnotation (recAnnotation&& other) = delete;
    ~recAnnotation () = default;

    recAnnotation &operator=(const fb2info& fb2)    // assign from fb2 info
    {
        anno_text = fb2.annotation;
    }

    recAnnotation &operator=(const recBook& rb)    // assign from recBook
    {
        anno_book_id = rb.book_id;
        return *this;
    }

    int table_create (sqlite3 * db);
    int recWrite     (sqlite3 * db);
    int recRead      (sqlite3 * db);

    void clear ()        // clear all fields
    {
        anno_book_id = 0;
        anno_text.clear();
    }

    bool valid ()        // validate all fields
    {
        return !anno_text.empty() && anno_text.length() > 5;
    }
};



