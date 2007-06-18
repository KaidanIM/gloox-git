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

namespace gloox
{

  AMP::Rule::Rule( const std::string& condition,
                   const std::string& action,
                   const std::string& value )
  {
    if( condition == "deliver" )
      m_condition = AMP::ConditionDeliver;
    else if( condition == "expire-at" )
      m_condition = AMP::ConditionExpireAt;
    else if( condition == "match-resource" )
      m_condition = AMP::ConditionMatchResource;
    else
      m_condition = AMP::ConditionInvalid;

    if( action == "alert" )
      m_action = AMP::ActionAlert;
    else if( action == "error" )
      m_action = AMP::ActionError;
    else if( action == "drop" )
      m_action = AMP::ActionDrop;
    else if( action == "notify" )
      m_action = AMP::ActionNotify;
    else
      m_action = AMP::ActionInvalid;

    switch( m_condition )
    {
      case ConditionDeliver:
        if( value == "direct" )
          deliver = DeliverDirect;
        else if( value == "forward" )
          deliver = DeliverForward;
        else if( value == "gateway" )
          deliver = DeliverGateway;
        else if( value == "none" )
          deliver = DeliverNone;
        else if( value == "stored" )
          deliver = DeliverStored;
        else
          deliver = DeliverInvalid;
        break;
      case ConditionExpireAt:
        // parse time
        //  expireat.tm_ = val;
        break;
      case ConditionMatchResource:
        if( value == "any" )
          matchresource = MatchResourceAny;
        else if( value == "exact" )
          matchresource = MatchResourceExact;
        else if( value == "other" )
          matchresource = MatchResourceOther;
        else
          matchresource = MatchResourceInvalid;
        break;
      default:
      case ConditionInvalid: // shouldn't happen
        break;
    }
  }

  Tag* AMP::Rule::tag() const
  {
    Tag* rule = new Tag( "rule" );
    switch( m_condition )
    {
      case ConditionDeliver:
        rule->addAttribute( "condition", "deliver" );
        break;
      case ConditionExpireAt:
        rule->addAttribute( "condition", "expire-at" );
        break;
      case ConditionMatchResource:
        rule->addAttribute( "condition", "match-resource" );
        break;
      default:
      case ConditionInvalid: // shouldn't happen
        delete rule;
        return 0;
    }
    switch( m_action )
    {
      case ActionAlert:
        rule->addAttribute( "action", "alert" );
        break;
      case ActionError:
        rule->addAttribute( "action", "error" );
        break;
      case ActionDrop:
        rule->addAttribute( "action", "drop" );
        break;
      case ActionNotify:
        rule->addAttribute( "action", "notify" );
        break;
      default:
      case ActionInvalid: // shouldn't happen
        delete rule;
        return 0;
    }
    switch( m_condition )
    {
      case ConditionDeliver:
        switch( deliver )
        {
          case DeliverDirect:
            rule->addAttribute( "value", "direct" );
            break;
          case DeliverForward:
            rule->addAttribute( "value", "forward" );
            break;
          case DeliverGateway:
            rule->addAttribute( "value", "gateway" );
            break;
          case DeliverNone:
            rule->addAttribute( "value", "none" );
            break;
          case DeliverStored:
            rule->addAttribute( "value", "stored" );
            break;
          case DeliverInvalid:
            delete rule;
            return 0;
        }
        break;
      case ConditionExpireAt:
          // parse time
          //expireat.tm_ = val;
        break;
      case ConditionMatchResource:
        switch( matchresource )
        {
          case MatchResourceAny:
            rule->addAttribute( "value", "any" );
            break;
          case MatchResourceExact:
            rule->addAttribute( "value", "exact" );
            break;
          case MatchResourceOther:
            rule->addAttribute( "value", "other" );
            break;
          case MatchResourceInvalid:
            delete rule;
            return 0;
        }
        break;
      case ConditionInvalid: // shouldn't happen
        delete rule;
        return 0;
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
      const std::string& condition = tag->findAttribute( "condition" );
      const std::string& action = tag->findAttribute( "action" );
      const std::string& value = tag->findAttribute( "value" );
      m_rules.push_back( new Rule( condition, action, value ) );
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

}
