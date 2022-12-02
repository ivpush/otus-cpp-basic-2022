// database interface 
//
#pragma once

class recAuthor;
class recBook;
class recAnnotation;

// database interface
// to be useful when implement more databases
//

class iDatabase
{
public:
    virtual ~iDatabase() {}

    virtual int db_open (const char * parm) = 0;
    virtual int db_close () = 0;
    virtual const char * db_error () = 0;

    virtual int addAuthor (recAuthor& au) = 0;
    virtual int	readAuthor (recAuthor& au) = 0;

    virtual int addBook (recBook& bk) = 0;
    virtual int	readBook (recBook& bk) = 0;

    virtual int addAnnotation (recAnnotation& anno) = 0;
    virtual int	readAnnotation (recAnnotation& anno) = 0;

    virtual int addGenre (recGenre& genre) = 0;
    virtual int	readGenre (recGenre& genre) = 0;

    virtual int addBookGenre (recBookGenre& bkg) = 0;
    virtual int	readBookGenre (recBookGenre& bkg) = 0;
};

// Sqlite3 database interface
//

struct sqlite3;

class sqlite3db : public iDatabase 
{
    sqlite3* db;

    int create_tables();

public:
    int db_open (const char * parm) override;
    int db_close () override;
    const char * db_error () override;

    sqlite3db ()    {
        db = nullptr;
    }
    ~sqlite3db ()     {
        db_close ();
        db = nullptr;
    }

    int addAuthor (recAuthor& au) override;
    int	readAuthor (recAuthor& au) override;

    int addBook (recBook& bk) override;
    int	readBook (recBook& bk) override;

    int addAnnotation (recAnnotation& anno) override;
    int	readAnnotation (recAnnotation& anno) override;

    int addGenre (recGenre& genre) override;
    int	readGenre (recGenre& genre) override;

    int addBookGenre (recBookGenre& bkg) override;
    int	readBookGenre (recBookGenre& bkg) override;
};

