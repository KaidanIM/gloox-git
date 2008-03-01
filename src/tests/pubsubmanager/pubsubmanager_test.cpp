#include "../../pubsubmanager.h"
#include "../../pubsubresulthandler.h"

using namespace gloox;

class RH : public PubSub::ResultHandler
{
  public:
    void handleItem( const JID&, const std::string&, const Tag* ) {}
    void handleItems( const JID&, const std::string&, const TagList*, const Error* ) {}
    void handleItemPublication( const JID&, const std::string&,
                                            const std::string&,
                                            const Error* ) {}
    void handleItemDeletation(  const JID&, const std::string&,
                                            const std::string&,
                                            const Error* ) {}
    void handleSubscriptionResult( const JID&, const std::string&,
                                               const std::string&,
                                               const JID&,
                                               const PubSub::SubscriptionType,
                                               const Error* ) {}
    void handleUnsubscriptionResult( const JID&, const std::string&,
                                                 const std::string&,
                                                 const JID&,
                                                 const Error* ) {}
    void handleSubscriptionOptions( const JID&, const JID&,
                                                const std::string&,
                                                const DataForm*,
                                                const Error*) {}
    void handleSubscriptionOptionsResult( const JID&, const JID&,
                                                      const std::string&,
                                                      const Error* ) {}
    void handleSubscribers( const JID&, const std::string&,
                                        const PubSub::SubscriberList*,
                                        const Error* ) {}
    void handleSubscribersResult( const JID&, const std::string&,
                                  const PubSub::SubscriberList*,
                                  const Error* ) {}
    void handleAffiliates( const JID&, const std::string&,
                                       const PubSub::AffiliateList*,
                                       const Error* ) {}
    void handleAffiliatesResult( const JID&, const std::string&,
                                 const PubSub::AffiliateList*,
                                 const Error* ) {}
    void handleNodeConfig( const JID&, const std::string&,
                                       const DataForm*,
                                       const Error* ) {}
    void handleNodeConfigResult( const JID&, const std::string&, const Error* ) {}
    void handleNodeCreation( const JID&, const std::string&, const Error* ) {}
    void handleNodeDeletation( const JID&, const std::string&, const Error* ) {}

    void handleNodePurge( const JID&, const std::string&, const Error* ) {}
    void handleSubscriptions( const JID&, const PubSub::SubscriptionMap*, const Error* ) {}
    void handleAffiliations( const JID&, const PubSub::AffiliationMap*, const Error* ) {}
    void handleDefaultNodeConfig( const JID&, const DataForm*, const Error*) {}

};

enum
{
  SubscriptionDefault,
  SubscriptionJID,
  SubscriptionSubType,
  SubscriptionDepth,

  UnsubscriptionDefault,
  UnsubscriptionJID,

  GetSubscriptionOptions,
  SetSubscriptionOptions,

  GetSubscriptionList,
  GetSubscriberList,
  SetSubscriberList,

  GetAffiliationList,
  GetAffiliateList,
  SetAffiliateList,

  GetNodeConfig,
  SetNodeConfig,
  DefaultNodeConfig,

  GetItemList,
  PublishItem,
  DeleteItem,

  CreateNodeLeaf,
  DeleteNodeCollection,
  PurgeNode,

  NodeAssociation,
  NodeDisassociation

};

static const JID jid( "aaa@bbb.ccc" );
static const std::string node( "node" );

static const Tag* tag;

namespace gloox
{

class ClientBase
{
  public:
    ClientBase() : failed( 0 ) {}
    void setTest( int test ) { m_context = test; }
    const std::string getID()
    {
      static const std::string id( "id" );
      return id;
    }
    const JID& jid() { return ::jid; }

    void send( const IQ& iq, IqHandler* = 0, int context = 0 );

    int failed;

  protected:
    int m_context;
};

static const std::string setheader =
    "<iq xmlns='jabber:client' to='aaa@bbb.ccc' id='id' type='set'>"
      "<pubsub xmlns='http://jabber.org/protocol/pubsub'>";

static const std::string getheader =
    "<iq xmlns='jabber:client' to='aaa@bbb.ccc' id='id' type='get'>"
      "<pubsub xmlns='http://jabber.org/protocol/pubsub'>";

static const std::string testValues [][2] =
{
  { "subscription basic",
    setheader + "<subscribe node='node' jid='aaa@bbb.ccc'/></pubsub></iq>" },

  { "subscription other",
    setheader + "<subscribe node='node' jid='some@jid.com'/></pubsub></iq>" },

  { "subscription items",
    setheader + "<subscribe node='node' jid='some@jid.com'/>"
    "<options>"
        "<x xmlns='jabber:x:data' type='submit'>"
        "<field type='hidden' var='FORM_TYPE'>"
                "<value>http://jabber.org/protocol/pubsub#subscribe_options</value>"
            "</field>"
            "<field var='pubsub#subscription_type'><value>items</value></field>"
        "</x>"
    "</options></pubsub></iq>" },

  { "subscription depth",
    setheader + "<subscribe node='node' jid='some@jid.com'/>"
    "<options>"
        "<x xmlns='jabber:x:data' type='submit'>"
            "<field type='hidden' var='FORM_TYPE'>"
                "<value>http://jabber.org/protocol/pubsub#subscribe_options</value>"
            "</field>"
            "<field var='pubsub#subscription_type'><value>items</value></field>"
            "<field var='pubsub#subscription_depth'><value>all</value></field>"
        "</x>"
    "</options></pubsub></iq>" },

  { "unsubscription default",
    setheader + "<unsubscribe node='node'/></pubsub></iq>" },

  { "unsubsription other",
    setheader + "<unsubscribe node='node' jid='some@jid.com'/></pubsub></iq>" },

  { "subscription options get",
    getheader + "<options node='node' jid='some@jid.com'/></pubsub></iq>" },

  { "subscription options set",
    setheader + "<options node='node' jid='some@jid.com'>"
         "<x xmlns='jabber:x:data' type='submit'>"
             "<field type='hidden' var='FORM_TYPE'>"
                 "<value>http://jabber.org/protocol/pubsub#subscribe_options</value>"
             "</field>"
         "</x>"
     "</options></pubsub></iq>" },

  { "subscription list get",
    getheader + "<subscriptions/></pubsub></iq>" },

  { "subscriber list get",
    "<iq xmlns='jabber:client' to='aaa@bbb.ccc' id='id' type='get'>"
      "<pubsub xmlns='http://jabber.org/protocol/pubsub#owner'>"
          "<subscriptions node='node'/>"
      "</pubsub></iq>" },

  { "subscriber list set",
    "<iq xmlns='jabber:client' to='aaa@bbb.ccc' id='id' type='set'>"
        "<pubsub xmlns='http://jabber.org/protocol/pubsub#owner'>"
            "<subscriptions node='node'>"
                "<subscription jid='some@jid.com' subscription='none' subid='abc'/>"
            "</subscriptions>"
        "</pubsub></iq>" },

  { "affiliation list get",
    getheader + "<affiliations/></pubsub></iq>" },

  { "affiliate list get",
    "<iq xmlns='jabber:client' to='aaa@bbb.ccc' id='id' type='get'>"
      "<pubsub xmlns='http://jabber.org/protocol/pubsub#owner'>"
          "<affiliations node='node'/>"
      "</pubsub></iq>" },

  { "affiliate list set",
    "<iq xmlns='jabber:client' to='aaa@bbb.ccc' id='id' type='set'>"
        "<pubsub xmlns='http://jabber.org/protocol/pubsub#owner'>"
            "<affiliations node='node'>"
                "<affiliation jid='some@jid.com' affiliation='owner'/>"
            "</affiliations>"
        "</pubsub></iq>" },

  { "get node config",
    "<iq xmlns='jabber:client' to='aaa@bbb.ccc' id='id' type='get'>"
        "<pubsub xmlns='http://jabber.org/protocol/pubsub#owner'>"
            "<configure node='node'/>"
        "</pubsub></iq>" },

  { "set node config",
    "<iq xmlns='jabber:client' to='aaa@bbb.ccc' id='id' type='set'>"
        "<pubsub xmlns='http://jabber.org/protocol/pubsub#owner'>"
            "<configure node='node'>"
                "<x xmlns='jabber:x:data' type='submit'>"
                    "<field type='hidden' var='FORM_TYPE'>"
                        "<value>http://jabber.org/protocol/pubsub#node_config</value>"
                    "</field>"
                    "<field var='pubsub#title'><value>Princely Musings (Atom)</value></field>"
                "</x>"
            "</configure>"
        "</pubsub></iq>" },

  { "get default node config",
    "<iq xmlns='jabber:client' to='aaa@bbb.ccc' id='id' type='get'>"
        "<pubsub xmlns='http://jabber.org/protocol/pubsub#owner'>"
            "<default/>"
        "</pubsub></iq>" }
    };

    void ClientBase::send( const IQ& iq, IqHandler*, int )
    {
      tag = iq.tag();
      if( !tag || tag->xml() != testValues[m_context][1] )
      {
        printf( "test failed: %s\n", testValues[m_context][0].c_str() );
        printf( "- %s\n", testValues[m_context][1].c_str() );
        if( tag )
          printf( "- %s\n", tag->xml().c_str() );
        ++failed;
      }
      delete tag;
    }


}

#define CLIENTBASE_H__
#include "../../pubsubmanager.cpp"

JID jid2( "some@jid.com" );

int main()
{
  ClientBase* cb = new ClientBase();
  PubSub::Manager* psm = new PubSub::Manager( cb );
  RH* rh = new RH();

  cb->setTest( SubscriptionDefault );
  psm->subscribe( jid, node, rh );

  cb->setTest( SubscriptionJID );
  psm->subscribe( jid, node, rh, jid2 );

  cb->setTest( SubscriptionSubType );
  psm->subscribe( jid, node, rh, jid2, PubSub::SubscriptionItems );

  cb->setTest( SubscriptionDepth );
  psm->subscribe( jid, node, rh, jid2, PubSub::SubscriptionItems, 0 );

  cb->setTest( UnsubscriptionDefault );
  psm->unsubscribe( jid, node, rh );

  cb->setTest( UnsubscriptionJID );
  psm->unsubscribe( jid, node, rh, jid2 );

  cb->setTest( GetSubscriptionOptions );
  psm->getSubscriptionOptions( jid, jid2, node, rh );

  DataForm* df = new DataForm( TypeSubmit );
  df->addField( DataFormField::TypeHidden, "FORM_TYPE",
      "http://jabber.org/protocol/pubsub#subscribe_options" );
  cb->setTest( SetSubscriptionOptions );
  psm->setSubscriptionOptions( jid, jid2, node, *df, rh );
  delete df;

  cb->setTest( GetSubscriptionList );
  psm->getSubscriptions( jid, rh );

  cb->setTest( GetSubscriberList );
  psm->getSubscribers( jid, node, rh );

  PubSub::SubscriberList sl;
  sl.push_back( PubSub::Subscriber( jid2, PubSub::SubscriptionNone, "abc" ) );
  cb->setTest( SetSubscriberList );
  psm->setSubscribers( jid, node, sl, rh );

  cb->setTest( GetAffiliationList );
  psm->getAffiliations( jid, rh );

  cb->setTest( GetAffiliateList );
  psm->getAffiliates( jid, node, rh );

  PubSub::AffiliateList al;
  al.push_back( PubSub::Affiliate( jid2, PubSub::AffiliationOwner ) );
  cb->setTest( SetAffiliateList );
  psm->setAffiliates( jid, node, al, rh );

  cb->setTest( GetNodeConfig );
  psm->getNodeConfig( jid, node, rh );

  df = new DataForm( TypeSubmit );
  df->addField( DataFormField::TypeHidden, "FORM_TYPE",
      "http://jabber.org/protocol/pubsub#node_config" );
  df->addField( DataFormField::TypeNone, "pubsub#title", "Princely Musings (Atom)" );
  cb->setTest( SetNodeConfig );
  psm->setNodeConfig( jid, node, *df, rh );
  delete df;

  cb->setTest( DefaultNodeConfig );
  psm->getDefaultNodeConfig( jid, PubSub::NodeLeaf, rh );


  delete rh;
  delete psm;




  if( cb->failed )
    printf( "PubSub::Manager: %d test(s) failed\n", cb->failed );
  else
    printf( "PubSub::Manager: OK\n" );

  delete cb;

}

