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


#ifndef RESULTHANDLER_H__
#define RESULTHANDLER_H__

#include <string>
using namespace std;

/**
 * A virtual interface.
 * Derived classes can be registered as ResultHandlers.
 * @ref result() will be called by the Feeder Library when an result arrives.
 * @author Jakob Schroeter <js@camaya.net>
 */
class ResultHandler
{
  public:
    /**
     * reimplment this to receive incoming results.
     * @param data The result received.
     */
    virtual void handleResult( const string& result) {};

};

#endif // RESULTHANDLER_H__
