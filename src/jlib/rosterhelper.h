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


#ifndef ROSTERHELPER_H__
#define ROSTERHELPER_H__

#include <map>
using namespace std;
/**
 * A virtual interface.
 * This is a helper class only, holding the definition of @c RosterMap.
 * @todo Fix this.
 * @author Jakob Schroeter <js@camaya.net>
 */
class RosterHelper
{
  public:
    /**
     * @todo Move this to a better location.
     */
    typedef map<const string, int> RosterMap;
};

#endif // ROSTERHELPER_H__
