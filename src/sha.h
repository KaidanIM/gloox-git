/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef SHA_H__
#define SHA_H__

#include <string>

namespace gloox
{
  class SHA
  {

    public:
      /**
       *
       */
      SHA();

      /**
       * Virtual Destructor.
       */
      virtual ~SHA();

      /**
       *
       */
      void reset();

      /**
       *
       */
      void finalize();

      /**
       * Returns the message digest
       */
      const std::string hex();

      /**
       * Provide input to SHA1
       */
      void feed( const unsigned char *message_array, unsigned length );
      void feed( const std::string& data );

    private:
      void process();
      void pad();
      inline unsigned shift( int bits, unsigned word );
      void init();

      unsigned H[5];
      unsigned Length_Low;
      unsigned Length_High;
      unsigned char Message_Block[64];
      int Message_Block_Index;
      bool m_finished;
      bool m_corrupted;

  };

}

#endif // SHA_H__
