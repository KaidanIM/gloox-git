/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */


#ifndef STORAGE_H__
#define STORAGE_H__

#include <mysql/mysql.h>

#include <string>
#include <list>

using namespace std;

class Result;

/**
 * This class implements a storage abstraction for the results.
 * @author Jakob Schroeter <js@camaya.net>
 */
class Storage
{
  public:
    /**
     * Constructor
     */
    Storage();

    /**
     * virtual Destructor
     */
    virtual ~Storage();

    /**
     * Stores a result in the selected backend.
     * @param result The result to store.
     * @return @c true if storing was successfull, @c false if storing failed
     */
    bool store( Result& result );

  private:
    MYSQL mysql;
};

#endif // STORAGE_H__
