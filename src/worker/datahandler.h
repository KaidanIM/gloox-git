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


#ifndef DATAHANDLER_H__
#define DATAHANDLER_H__


/**
 * A virtual interface.
 * Derived classes can be registered as DataHandlers.
 * data() will be called by the Worker if it has Data available.
 * @author Jakob Schroeter <js@camaya.net>
 */
class DataHandler
{
  public:
    /**
     * reimplment this to receive incoming data.
     * @param data The data received.
     */
    virtual void data( const char* data) {};

};

#endif // DATAHANDLER_H__
