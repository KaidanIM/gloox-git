/*
  Copyright (c) 2013 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEFILETRANSFER_H__
#define JINGLEFILETRANSFER_H__

#include "jingleplugin.h"
#include "tag.h"

#include <string>
#include <list>

namespace gloox
{

  namespace Jingle
  {

    /**
     * @brief An abstraction of the signaling part of Jingle File Tarnsfer (XEP-0234).
     *
     * XEP Version: 1.0
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0.7
     */
    class FileTransfer : public Plugin
    {
      public:

        /**
         *
         */
        enum Type
        {
          Offer,
          Request,
          Checksum,
          Abort,
          Received,
          Invalid
        };

        struct File
        {
          std::string name;
          std::string date;
          std::string desc;
          std::string hash;
          std::string hash_algo;
          long int size;
          bool range;
          long int offset;
        };

        typedef std::list<File> FileList;

        /**
         *
         */
        FileTransfer( Type type, FileList files );

        /**
         *
         */
        FileTransfer( const Tag* tag = 0 );

        /**
         * Virtual destructor.
         */
        virtual ~FileTransfer() {}

        // reimplemented from Plugin
        virtual const std::string& filterString() const;

        // reimplemented from Plugin
        virtual Tag* tag() const;

        // reimplemented from Plugin
        virtual Plugin* newInstance( const Tag* tag ) const;

        // reimplemented from Plugin
        virtual Plugin* clone() const
        {
          return new FileTransfer( *this );
        }

      private:

        void parseFileList( const TagList& files );

        Type m_type;
        FileList m_files;

    };

  }

}

#endif // JINGLEFILETRANSFER_H__
