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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/



#ifndef PRIVATEXMLHANDLER_H__
#define PRIVATEXMLHANDLER_H__

#include <iksemel.h>
#include <string>

using namespace std;

/**
 * A virtual interface.
 * Derived classes can be registered as PrivateXMLHandlers with the PrivateXML object.
 * Upon an incoming PrivateXML packet @ref handlePrivateXML() will be called.
 * @author Jakob Schroeter <js@camaya.net>
 */
class PrivateXMLHandler
{
  public:
    /**
     * Reimplement this function to receive the private XML that was requested earlier using
     * @c PrivateXML::requestXML().
     * @param xmlns The XML namespace of the storage packet.
     * @param tag The tag of the storage packet.
     * @param pak The complete packet.
     */
    virtual void handlePrivateXML( const string& xmlns, const string& tag, ikspak* pak ) {};

};

#endif // PRIVATEXMLHANDLER_H__
