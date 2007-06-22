/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "amp.h"
#include "tag.h"
#include "util.h"

namespace gloox
{

  static const char * conditionValues[] = {
    "deliver", "expire-at", "match-resource"
  };

  static const char * actionValues[] = {
    "alert", "error", "drop", "notify"
  };

  static const char * deliverValues[] = {
    "direct", "forward", "gateway", "none", "stored"
  };

  static const char * matchResourceValues[] = {
    "any", "exact", "other"
  };

  AMP::Rule::Rule( const std::string& condition,
                   const std::string& action,
                   const std::string& value )
  {
    m_condition = (ConditionType)util::lookup( condition, conditionValues,
                                 sizeof(conditionValues)/sizeof(const char *) );
    m_action = (ActionType)util::lookup( action, actionValues,
                                    sizeof(actionValues)/sizeof(const char *) );
    switch( m_condition )
    {
      case ConditionDeliver:
        deliver = (DeliverType)util::lookup( value, deliverValues,
                                   sizeof(deliverValues)/sizeof(const char *) );
      case ConditionExpireAt:
        // parse time
        //  expireat.tm_ = val;
        break;
      case ConditionMatchResource:
        matchresource = (MatchResourceType)util::lookup( value, matchResourceValues,
                                   sizeof(matchResourceValues)/sizeof(const char *) );
        break;
      default:
      case ConditionInvalid: // shouldn't happen
        break;
    }
  }

  Tag* AMP::Rule::tag() const
  {
    if( m_condition == ConditionInvalid || m_action == ActionInvalid
       || deliver == DeliverInvalid )
     return 0;
      
    Tag* rule = new Tag( "rule" );
    rule->addAttribute( "condition", util::lookup( m_condition, conditionValues,
                               sizeof(conditionValues)/sizeof(const char *) ) );
    rule->addAttribute( "action", util::lookup( m_action, actionValues,
                                  sizeof(actionValues)/sizeof(const char *) ) );

    switch( m_condition )
    {
      case ConditionDeliver:
        if( deliver == DeliverInvalid )
        {
          delete rule;
          return 0;
        }
        rule->addAttribute( "value", util::lookup( deliver, deliverValues,
                                  sizeof(deliverValues)/sizeof(const char*) ) );
        break;
      case ConditionExpireAt:
          // parse time
          //expireat.tm_ = val;
        break;
      case ConditionMatchResource:
        if( matchresource == MatchResourceInvalid )
        {
          delete rule;
          return 0;
        }
        rule->addAttribute( "value", "any" );
        break;
    }
    return rule;
  }

  AMP::AMP( const Tag *tag )
    : StanzaExtension( ExtAMP ), m_valid( false )
  {
    if( !tag || tag->name() != "amp" || !tag->hasAttribute( "xmlns", XMLNS_AMP ) )
      return;

    const Tag::TagList& rules = tag->children();
    Tag::TagList::const_iterator it = rules.begin();
    for( ; it != rules.end(); ++it )
    {
      m_rules.push_back( new Rule( tag->findAttribute( "condition" ),
                                   tag->findAttribute( "action" ),
                                   tag->findAttribute( "value" ) ) );
    }
    m_valid = true;
  }

  Tag* AMP::tag() const
  {
    if( !m_valid )
      return 0;

    Tag *amp = new Tag( "amp" );
    amp->addAttribute( "xmlns", XMLNS_AMP );
    RuleList::const_iterator it = m_rules.begin();
    for( ; it != m_rules.end(); ++it )
      amp->addChild( (*it)->tag() );

    return amp;
  }

  AMP::~AMP()
  {
    RuleList::iterator it = m_rules.begin();
    for( ; it != m_rules.end() ; ++it )
      delete (*it);
  }

}
