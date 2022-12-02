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

class iDatabase;

bool book_processing (iDatabase *db, const char* path);

// class fb2info - информация из тегов fb2
//
class fb2info   
{
    // <title-info>
    std::string     author_first_name;      // <author> <first-name>
    std::string     author_middle_name;     //          <middle-name>
    std::string     author_last_name;       //          <last-name>
    std::string     book_title;             // <book-title>
    std::vector<std::string>    book_genre; // <genre>
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
        book_genre.clear();
        valid = false;
    }
    bool isValid () {  return valid; }

    const char * get_auth_firstname() {    return author_first_name.c_str();  }
    const char * get_auth_midname  () {    return author_middle_name.c_str();  }
    const char * get_auth_lastname () {    return author_last_name.c_str();  }
    
    const char * get_tran_firstname() {    return translator_first_name.c_str();  }
    const char * get_tran_midname  () {    return translator_middle_name.c_str();  }
    const char * get_tran_lastname () {    return translator_last_name.c_str();  }
    
    const char * get_title () {    return book_title.c_str();  }
    const char * get_annotation () {    return annotation.c_str();  }
    const char * get_lang () {    return lang.c_str();  }
    const char * get_src_lang () {    return src_lang.c_str();  }
    const char * get_year () {    return year.c_str();  }
    const char * get_location () {    return location.c_str();  }

    std::vector<std::string>& get_genre ()  { return book_genre; }
};

// class recAuthor - запись о авторе в БД
//
class recAuthor 
{
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
    recAuthor &operator=(fb2info& fb2)    // assign from fb2 info
    {
        au_id = 0;
        au_firstname = fb2.get_auth_firstname ();
        au_midname   = fb2.get_auth_midname ();
        au_lastname  = fb2.get_auth_lastname ();
    }

    int get_id ()     {  return au_id;  }
    const char * get_firstname() {    return au_firstname.c_str();  }
    const char * get_midname  () {    return au_midname.c_str();  }
    const char * get_lastname () {    return au_lastname.c_str();  }

    void set_id (int value)     {  au_id = value;  }
    void set_firstname(std::string& value) {    au_firstname = value;  }
    void set_midname  (std::string& value) {    au_midname = value;  }
    void set_lastname (std::string& value) {    au_lastname = value;  }

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
};

// class recBook - запись о книге в БД
//
class recBook  
{
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
    recBook (fb2info& fb2)  // construct from fb2 info
    {
        book_id    = 0;
        book_au_id = 0;
        book_title = fb2.get_title();
        book_lang  = fb2.get_lang();
        book_src_lang = fb2.get_src_lang();
        book_transl_id = 0;
        book_year = atoi (fb2.get_year());
        book_path = fb2.get_location();
    }

    ~recBook () = default;

    int get_id ()     {  return book_id;  }
    int get_auth_id ()     {  return book_au_id;  }
    int get_transl_id ()     {  return book_transl_id;  }
    const char * get_title () {  return book_title.c_str();  }
    const char * get_lang ()  {  return book_lang.c_str();  }
    const char * get_src_lang () {  return book_src_lang.c_str();  }
    int get_year ()     {  return book_year;  }
    const char * get_location () {  return book_path.c_str();  }

    void    set_id (int id)         {  book_id = id;  }
    void    set_auth_id (int id)    {  book_au_id = id;  }
    void    set_transl_id (int id)  {  book_transl_id = id;  }
    void    set_title (const char* s) {  book_title = s;  }
    void    set_lang (const char* s)  {  book_lang = s;  }
    void    set_src_lang (const char* s) {  book_src_lang = s;  }
    void    set_year (int year)     {  book_year = year;  }
    void    set_location(const char* s) {  book_path = s;  }

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
};

// class recAnnotation - запись аннотации книги в БД
//
class recAnnotation 
{
    int             anno_book_id;   // book id the annotation is for
    std::string     anno_text;      // annotation itself (text) 

public:
    recAnnotation ();
    recAnnotation (fb2info& fb2)
    {
        anno_book_id = 0;
        anno_text = fb2.get_annotation ();
    }
    recAnnotation (const recAnnotation& other) = delete;
    recAnnotation (recAnnotation&& other) = delete;
    ~recAnnotation () = default;

    recAnnotation &operator=(fb2info& fb2)    // assign from fb2 info
    {
        anno_text = fb2.get_annotation ();
        return *this;
    }

    int get_book_id ()       {  return anno_book_id;  }
    const char * get_text () {  return anno_text.c_str();  }

    void    set_book_id (int id)     {  anno_book_id = id;  }
    void    set_text (const char* s) {  anno_text = s;  }

    void clear ()        // clear all fields
    {
        anno_book_id = 0;
        anno_text.clear();
    }

    bool valid ()        // validate all fields
    {
        return ((anno_book_id != 0) && (!anno_text.empty()) && (anno_text.length() > 5));
    }
};

// class recGenre - запись о жанре в БД
//
class recGenre 
{
    int             genre_id;   // genre id 
    std::string     genre_name; // genre name (text) 

public:
    recGenre ();
    recGenre (const char* s) : genre_name (s), genre_id (0) { }
    recGenre (const recGenre& other) = delete;
    recGenre (recGenre&& other) = delete;
    ~recGenre () = default;

    int get_id ()       {  return genre_id;  }
    const char * get_name () {  return genre_name.c_str();  }

    void    set_id (int id)     {  genre_id = id;  }
    void    set_name (const char* s) {  genre_name = s;  }

    void clear ()        // clear all fields
    {
        genre_id = 0;
        genre_name.clear();
    }
};

// class recBookGenre - запись о жанре книги в БД
//
class recBookGenre 
{
    int             genre_id;   // genre id 
    int             book_id;    // book id 

public:
    recBookGenre (int idbook, int idgenre) : book_id (idbook), genre_id (idgenre)  { }
    recBookGenre (const recBookGenre& other) = delete;
    recBookGenre (recBookGenre&& other) = delete;
    ~recBookGenre () = default;

    int get_id ()       {  return genre_id;  }
    int get_book_id ()  {  return book_id;  }

    void    set_id      (int id)     {  genre_id = id;  }
    void    set_book_id (int id)     {  book_id  = id;  }

    void clear ()        // clear all fields
    {
        book_id  = 0;
        genre_id = 0;
    }

    bool valid ()        // validate all fields
    {
        return ((book_id != 0) && (genre_id != 0));
    }
};

recAuthor getAuthor     (fb2info& fb2);
recAuthor getTranslator (fb2info& fb2);


