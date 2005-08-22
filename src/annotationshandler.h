/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

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



#ifndef ANNOTATIONSHANDLER_H__
#define ANNOTATIONSHANDLER_H__

#include <string>
#include <list>
using namespace std;

namespace gloox
{

  /**
   * A virtual interface which can be reimplemented to receive notes with help of
   * the Annotations object.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class AnnotationsHandler
  {
    public:
      /**
       * This describes a single note item.
       */
      struct annotationsListItem
      {
        std::string jid;            /**< The JID of the roster item this note is about */
        std::string cdate;          /**< Creation date of this note. */
        std::string mdate;          /**< Date of last modification of this note. */
        std::string note;           /**< The note. */
      };

      /**
       * A list of note items.
       */
      typedef std::list<annotationsListItem> AnnotationsList;

      /**
       * This function is called when notes arrive from the server.
       * @param aList A list of notes.
       */
      virtual void handleAnnotations( AnnotationsList &aList ) {};
  };

};

#endif // ANNOTATIONSHANDLER_H__
