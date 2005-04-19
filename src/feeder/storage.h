/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
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
    bool store( const Result& result );

  private:
    MYSQL mysql;
};

#endif // STORAGE_H__
