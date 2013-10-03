/*
  Copyright (c) 2013 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "jinglefiletransfer.h"

#include "tag.h"
#include "gloox.h"
#include "util.h"

#include <cstdlib>

namespace gloox
{

  namespace Jingle
  {

    FileTransfer::FileTransfer( const std::string& d )
    {
    }

    FileTransfer::FileTransfer( const Tag* tag )
    {
      if( !tag || tag->xmlns() != XMLNS_JINGLE_FILE_TRANSFER )
        return;

      std::string name = tag->name();
      if( name == "description" )
      {
        const Tag* c = tag->findTag( "offer|request" );
        if( c )
        {
          parseFileList( c->findChildren( "file" ) );
          name = c->name();
        }
      }
      else if( name == "checksum" || name == "abort" || name == "received" )
      {
        parseFileList( tag->findChildren( "file" ) );
      }


    }

    void FileTransfer::parseFileList( const TagList& files )
    {
      TagList::const_iterator it = files.begin();
      for( ; it != files.end(); ++it )
      {
        File f;
        Tag *t = (*it)->findChild( "name" );
        f.name = t ? t->cdata() : EmptyString;
        t = (*it)->findChild( "desc" );
        f.desc = t ? t->cdata() : EmptyString;
        t = (*it)->findChild( "date" );
        f.date = t ? t->cdata() : EmptyString;
        t = (*it)->findChild( "size" );
        f.size = t ? atoi( t->cdata().c_str() ) : -1;
        t = (*it)->findChild( "range" );
        if( t )
        {
          f.range = true;
          f.offset = t->hasAttribute( "offset" ) ? atoi( t->findAttribute( "offset" ).c_str() ) : -1;
        }
        t = (*it)->findChild( "hash", XMLNS, XMLNS_HASHES );
        if( t )
        {
          f.hash_algo = t->findAttribute( "algo" );
          f.hash = t->cdata();
        }
        m_files.push_back( f );
      }
    }

    const std::string& FileTransfer::filterString() const
    {
      static const std::string filter = "description[@xmlns='" + XMLNS_JINGLE_FILE_TRANSFER + "']"
                                        "|abort[@xmlns='" + XMLNS_JINGLE_FILE_TRANSFER + "']"
                                        "|received[@xmlns='" + XMLNS_JINGLE_FILE_TRANSFER + "']"
                                        "|checksum[@xmlns='" + XMLNS_JINGLE_FILE_TRANSFER + "']";
      return filter;
    }

    Plugin* FileTransfer::newInstance( const Tag* tag ) const
    {
      return new FileTransfer( tag );
    }

    Tag* FileTransfer::tag() const
    {
      Tag* r = 0;

      switch( m_type )
      {
        case Offer:
        {
          r = new Tag( "description", XMLNS, XMLNS_JINGLE_FILE_TRANSFER );
          Tag* o = new Tag( r, "offer" );
          FileList::const_iterator it = m_files.begin();
          for( ; it != m_files.end(); ++it )
          {
            Tag* f = new Tag( o, "file" );
            new Tag( f, "date", (*it).date );
            new Tag( f, "name", (*it).name );
            new Tag( f, "desc", (*it).desc );
            new Tag( f, "size", util::long2string( (*it).size ) );
            Tag* h = new Tag( f, "hash", XMLNS, XMLNS_HASHES );
            h->addAttribute( "algo", (*it).hash_algo );
            h->setCData( (*it).hash );
            if( (*it).range )
              new Tag( f, "range" );
          }
          break;
        }
        case Request:
        {
          r = new Tag( "description", XMLNS, XMLNS_JINGLE_FILE_TRANSFER );
          Tag* o = new Tag( r, "request" );
          FileList::const_iterator it = m_files.begin();
          for( ; it != m_files.end(); ++it )
          {
            Tag* f = new Tag( o, "file" );
            new Tag( f, "date", (*it).date );
            new Tag( f, "name", (*it).name );
            new Tag( f, "desc", (*it).desc );
            new Tag( f, "size", util::long2string( (*it).size ) );
            Tag* h = new Tag( f, "hash", XMLNS, XMLNS_HASHES );
            h->addAttribute( "algo", (*it).hash_algo );
            h->setCData( (*it).hash );
            if( (*it).range )
              new Tag( f, "range", "offset", (*it).offset ? util::long2string( (*it).offset ) : EmptyString );
          }
          break;
        }
        case Abort:
        {
          r = new Tag( "abort", XMLNS, XMLNS_JINGLE_FILE_TRANSFER );
          FileList::const_iterator it = m_files.begin();
          Tag* f = new Tag( r, "file" );
          new Tag( f, "date", (*it).date );
          new Tag( f, "name", (*it).name );
          new Tag( f, "desc", (*it).desc );
          new Tag( f, "size", util::long2string( (*it).size ) );
          Tag* h = new Tag( f, "hash", XMLNS, XMLNS_HASHES );
          h->addAttribute( "algo", (*it).hash_algo );
          h->setCData( (*it).hash );
          break;
        }
        case Checksum:
        {
          r = new Tag( "checksum", XMLNS, XMLNS_JINGLE_FILE_TRANSFER );
          FileList::const_iterator it = m_files.begin();
          for( ; it != m_files.end(); ++it )
          {
            Tag* f = new Tag( r, "file" );
            new Tag( f, "date", (*it).date );
            new Tag( f, "name", (*it).name );
            new Tag( f, "desc", (*it).desc );
            new Tag( f, "size", util::long2string( (*it).size ) );
            Tag* h = new Tag( f, "hash", XMLNS, XMLNS_HASHES );
            h->addAttribute( "algo", (*it).hash_algo );
            h->setCData( (*it).hash );
            if( (*it).range )
              new Tag( f, "range" );
          }
          break;
        }
        case Received:
        {
          r = new Tag( "received", XMLNS, XMLNS_JINGLE_FILE_TRANSFER );
          FileList::const_iterator it = m_files.begin();
          Tag* f = new Tag( r, "file" );
          Tag* h = new Tag( f, "hash", XMLNS, XMLNS_HASHES );
          h->addAttribute( "algo", (*it).hash_algo );
          h->setCData( (*it).hash );
          break;
        }
      }

      return r;
    }

  }

}
