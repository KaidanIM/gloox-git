/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "siprofileft.h"

#include "siprofilefthandler.h"
#include "simanager.h"
#include "dataform.h"

namespace gloox
{

  SIProfileFT::SIProfileFT( SIManager* manager )
    : m_parent( manager ), m_handler( 0 )
  {
    if( m_parent )
      m_parent->registerProfile( XMLNS_SI_FT, this );
  }

  SIProfileFT::~SIProfileFT()
  {
    if( m_parent )
      m_parent->removeProfile( XMLNS_SI_FT );
  }

  void SIProfileFT::requestFT( const JID& to, const std::string& name, int size, const std::string& hash,
                               const std::string& desc, const std::string& date, const std::string& mimetype )
  {
    if( name.empty() || size <= 0 || !m_parent )
      return;

    Tag* file = new Tag( "file", "xmlns", XMLNS_SI_FT );
    file->addAttribute( "name", name );
    file->addAttribute( "size", size );
    if( !hash.empty() )
      file->addAttribute( "hash", hash );
    if( !date.empty() )
      file->addAttribute( "date", date );
    if( !desc.empty() )
      new Tag( file, "desc", desc );

    Tag* feature = new Tag( "feature", "xmlns", XMLNS_FEATURE_NEG );
    DataFormField* dff = new DataFormField( "stream-method", "", "", DataFormField::FieldTypeListSingle );
    StringMap sm;
    sm["s5b"] = XMLNS_BYTESTREAMS;
    sm["ibb"] = XMLNS_IBB;
    sm["oob"] = XMLNS_IQ_OOB;
    dff->setOptions( sm );
    DataForm df( DataForm::FormTypeForm );
    df.addField( dff );
    feature->addChild( df.tag() );

    m_parent->requestSI( this, to, XMLNS_SI_FT, file, feature, mimetype );
  }

  void SIProfileFT::acceptFT( const JID& to, const std::string& id, StreamType type )
  {
    if( !m_parent )
      return;

    Tag* feature = new Tag( "feature", "xmlns", XMLNS_FEATURE_NEG );
    DataFormField* dff = new DataFormField( "stream-method" );
    switch( type )
    {
      case FTTypeS5B:
        dff->setValue( XMLNS_BYTESTREAMS );
        break;
      case FTTypeIBB:
        dff->setValue( XMLNS_IBB );
        break;
      case FTTypeOOB:
        dff->setValue( XMLNS_IQ_OOB );
        break;
    }
    DataForm df( DataForm::FormTypeSubmit );
    df.addField( dff );
    feature->addChild( df.tag() );

    m_parent->acceptSI( to, id, 0, feature );
  }

  void SIProfileFT::declineFT( const JID& to, const std::string& id, SIManager::SIError reason,
                               const std::string& text )
  {
    if( !m_parent )
      return;

    m_parent->declineSI( to, id, reason, text );
  }

  void SIProfileFT::handleSIRequest( const JID& from, const std::string& id, const std::string& profile,
                                     Tag* si, Tag* ptag, Tag* /*fneg*/ )
  {
    if( profile != XMLNS_SI_FT || !ptag || !si )
      return;

    if( m_handler )
    {
      std::string desc;
      if( ptag->hasChild( "desc" ) )
        desc = ptag->findChild( "desc" )->cdata();
      const std::string& mt = si->findAttribute( "mime-type" );
      m_handler->handleFTRequest( from, id, ptag->findAttribute( "name" ), ptag->findAttribute( "size" ),
                                  ptag->findAttribute( "hash" ), ptag->findAttribute( "date" ),
                                  mt.empty() ? "binary/octet-stream" : mt, desc );
    }
  }

  void SIProfileFT::handleSIRequestResult( const JID& from, const std::string& sid,
                                           Tag* /*si*/, Tag* /*ptag*/, Tag* /*fneg*/ )
  {
    if( m_handler )
      m_handler->handleFTRequestResult( from, sid );
  }

  void SIProfileFT::handleSIRequestError( Stanza* stanza )
  {
    if( m_handler )
      m_handler->handleFTRequestError( stanza );
  }

}
