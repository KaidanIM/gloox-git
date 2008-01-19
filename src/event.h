/*
  Copyright (c) 2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


namespace gloox
{

  class Stanza;

  /**
   * @brief An abstract base class for events.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class Event
  {

    public:
      /**
       * Event types.
       */
      enum EventType
      {
        PingPong,                     /**< */
        PingError                     /**< */
      };

      /**
       *
       */
      Event( EventType type ) : m_eventType( type ), m_stanza( 0 ) {}

      /**
       * Virtual Destructor.
       */
      virtual ~Event() {}

      /**
       *
       */
      EventType eventType() const { return m_eventType; }

      /**
       * Return a pointer to a Stanza-derived object.
       * @return A pointer to a Stanza that caused the event. May be 0.
       * @note You should @b not delete the Stanza object.
       */
      const Stanza* stanza() const { return m_stanza; }

    protected:
      EventType m_eventType;
      Stanza* m_stanza;

  };

}
