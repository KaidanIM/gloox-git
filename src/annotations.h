/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#ifndef ANNOTATIONS_H__
#define ANNOTATIONS_H__

#include "annotationshandler.h"
#include "privatexml.h"
#include "privatexmlhandler.h"

#include <string>
#include <list>

namespace gloox
{

  class Tag;

  /**
   * This is an implementation of JEP-0145 (Annotations).
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class Annotations : public PrivateXML, PrivateXMLHandler
  {
    public:
      /**
       * Constructs a new Annotations object.
       * @param parent The ClientBase to use for communication.
       */
      Annotations( ClientBase *parent );

      /**
       * Virtual destructor.
       */
      virtual ~Annotations();

      /**
       * Use this function to store notes (annotations to contacts in a roster) on the server.
       * Make sure you store the whole set of annotations, not a 'delta'.
       * @param aList A list of notes to store.
       */
      void storeAnnotations( const AnnotationsHandler::AnnotationsList& aList );

      /**
       * Use this function to initiate retrieval of annotations. Use registerAnnotationsHandler()
       * to register an object which will receive the lists of notes.
       */
      void requestAnnotations();

      /**
       * Use this function to register a AnnotationsHandler.
       * @param ah The AnnotationsHandler which shall receive retrieved notes.
       */
      void registerAnnotationsHandler( AnnotationsHandler *ah );

      /**
       * Use this function to un-register the AnnotationsHandler.
       */
      void removeAnnotationsHandler();

      // reimplemented from PrivateXMLHandler
      virtual void handlePrivateXML( const std::string& tag, const std::string& xmlns, Tag *xml );

    private:
      AnnotationsHandler *m_annotationsHandler;
  };

};

#endif // ANNOTATIONS_H__
