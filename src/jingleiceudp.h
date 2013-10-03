/*
  Copyright (c) 2013 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEICEUDP_H__
#define JINGLEICEUDP_H__

#include "jingleplugin.h"

#include <string>
#include <list>

namespace gloox
{

  class Tag;

  namespace Jingle
  {

    /**
     * @brief An abstraction of the signaling part of Jingle ICE-UDP Transport Method (XEP-0176).
     *
     * XEP Version: 1.0
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0.7
     */
    class ICEUDP : public Plugin
    {
      public:
        /**
         *
         */
        enum Type
        {
          Host,
          PeerReflexive,
          Relayed,
          ServerReflexive
        };

        /**
         * Describes a single transport candidate.
         */
        struct Candidate
        {
          std::string component;
          std::string foundation;
          std::string generation;
          std::string id;
          std::string ip;
          std::string network;
          int port;
          int priority;
          std::string protocol;
          std::string rel_addr;
          int rel_port;
          Type type;
        };

        /** A list of transport candidates. */
        typedef std::list<Candidate> CandidateList;

        /**
         *
         */
        ICEUDP( const std::string& pwd, const std::string& ufrag, CandidateList& candidates );

        /**
         *
         */
        ICEUDP( const Tag* tag = 0 );

        /**
         * Virtual destructor.
         */
        virtual ~ICEUDP() {}

        // reimplemented from Plugin
        virtual const std::string& filterString() const;

        // reimplemented from Plugin
        virtual Tag* tag() const;

        // reimplemented from Plugin
        virtual Plugin* newInstance( const Tag* tag ) const;

        // reimplemented from Plugin
        virtual Plugin* clone() const
        {
          return new ICEUDP( *this );
        }

      private:
        std::string m_pwd;
        std::string m_ufrag;
        CandidateList m_candidates;

    };

  }

}

#endif // JINGLEICEUDP_H__
