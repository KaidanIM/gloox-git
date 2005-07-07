/*
  iksemelmm -- c++ wrapper for iksemel xml/xmpp library

  Copyright (C) 2004 Igor Goryachieff <igor@jahber.org>
  Copyright (C) 2005 Jakob Schroeter <js@camaya.net>

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


#ifndef STREAM_H__
#define STREAM_H__

#include "parser.h"
#include "tree.h"

#include <iksemel.h>


/* documented in parser.h */
namespace Iksemel
{
  /**
   * This encapsulates a stream from Iksemel.
   */
  class Stream: public Parser, public Tree
  {
    public:
      Stream();
      Stream( char* );
      virtual ~Stream();

      void set_log_hook();

      int connect( iksparser* , const std::string&, int, const std::string& );
      int connect( const std::string&, int, const std::string& );
      int connect( const std::string&, const std::string& );

      int connect( iksparser* , int );
      int connect( int );

      void disconnect( iksparser* );
      void disconnect();

      int recv( iksparser* , int );
      int recv( int );
      int recv();

      int fd( iksparser* );
      int fd();

      int send( iksparser* , iks* );
      int send();
      int send( iks* x );

      int send( iksparser* , const std::string& );
      int send( const std::string& );

      int header( iksparser* , const std::string& );
      int header( const std::string& );

      bool has_tls();

      int start_tls( iksparser* );
      int start_tls();

      bool is_secure( iksparser* );
      bool is_secure();

      int start_sasl( iksparser* prs , enum ikssasltype type, char* username , char* pass );
      int start_sasl( enum ikssasltype type, char* username, char* pass );

      iks* make_resource_bind(iksid *id);

      virtual void on_stream( int type, iks* node ) {};
      virtual void on_log( const char* data, size_t size, int is_incoming ) {};

    protected:

    private:
      Stream( const Stream& );
      Stream& operator=( const Stream& );
  };
};

#endif // STREAM_H__
