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


#ifndef PREP_H__
#define PREP_H__

#include <string>
using namespace std;

/**
 * This class offers static functions to stringprep the individual parts
 * of a JID. You should not need to use these functions directly. All the
 * necessary prepping is done for if you stick to the interfaces provided.
 * If you write your own enhancements, check with the spec.
 *
 * @note These functions depend on an installed LibIDN at compile time of gloox. If
 * LibIDN is not installed these functions return the string they are given
 * without any modification.
 * @author Jakob Schroeter <js@camaya.net>
 * @since 0.2
 */
class Prep
{
  public:
    /**
     * This function applies the Nodeprep profile of Stringprep to a string.
     * @param node The string to apply the profile to.
     * @return Returns the prepped string. In case of an error an empty string
     * is returned.
     */
    static string nodeprep( const string& node );

    /**
     * This function applies the Nameprep profile of Stringprep to a string.
     * @param domain The string to apply the profile to.
     * @return Returns the prepped string. In case of an error an empty string
     * is returned.
     */
    static string nameprep( const string& domain );

    /**
     * This function applies the Resourceprep profile of Stringprep to a string.
     * @param resource The string to apply the profile to.
     * @return Returns the prepped string. In case of an error an empty string
     * is returned.
     */
    static string resourceprep( const string& resource );

    /**
     * This function applies the idna() function to a string. I.e. it transform
     * internationalized domain names into plain ASCII.
     * @param domain The string to convert.
     * @return Returns the converted string. In case of an error an empty string
     * is returned.
     */
    static string idna( const string& domain );
};

#endif // PREP_H__
