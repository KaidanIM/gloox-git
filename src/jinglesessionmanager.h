/*
  Copyright (c) 2013 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLESESSIONMANAGER_H__
#define JINGLESESSIONMANAGER_H__

#include "macros.h"
#include "iqhandler.h"

#include <list>

namespace gloox
{

  class ClientBase;

  namespace Jingle
  {

    class Session;
    class SessionHandler;

    /**
     *
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0.5
     */
    class GLOOX_API SessionManager : public IqHandler
    {
      public:
        /**
         *
         */
        SessionManager( ClientBase* parent, SessionHandler* sh );

        /**
         *
         */
        virtual ~SessionManager();

        /**
         *
         */
        Session* createSession( const JID& callee, SessionHandler* handler );


        // reimplemented from IqHandler
        virtual bool handleIq( const IQ& iq );

        // reimplemented from IqHandler
        virtual void handleIqID( const IQ& /*iq*/, int /*context*/ ) {}

      private:
        typedef std::list<Jingle::Session*> SessionList;

        SessionList m_sessions;
        ClientBase* m_parent;
        SessionHandler* m_handler;

    };

  }

}

#endif // JINGLESESSIONMANAGER_H__
