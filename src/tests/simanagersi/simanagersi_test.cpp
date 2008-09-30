#include "../../tag.h"
#include "../../iq.h"
#include "../../iqhandler.h"
#include "../../base64.h"
#include "../../stanzaextensionfactory.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

gloox::JID g_jid( "foof" );

namespace gloox
{
  class Disco
  {
    public:
      Disco();
      ~Disco();
      void addFeature( const std::string& feature );
      void removeFeature( const std::string& feature );
  };
  Disco::Disco() {}
  Disco::~Disco() {}
  void Disco::addFeature( const std::string& /*feature*/ ) {}
  void Disco::removeFeature( const std::string& /*feature*/ ) {}

  class ClientBase
  {
    public:
      ClientBase() {}
      virtual ~ClientBase() {}
      Disco* disco();
      const JID& jid() const { return m_jid; }
      const std::string getID();
      virtual void send( IQ& ) = 0;
      virtual void send( const IQ&, IqHandler*, int ) = 0;
      virtual void trackID( IqHandler *ih, const std::string& id, int context ) = 0;
      void removeIqHandler( IqHandler* ih, int exttype );
      void registerIqHandler( IqHandler* ih, int exttype );
      void registerStanzaExtension( StanzaExtension* ext );
      void removeStanzaExtension( int ext );
      void removeIDHandler( IqHandler* ) {}
    private:
      Disco* m_disco;
      JID m_jid;
  };
  Disco* ClientBase::disco() { return m_disco; }
  void ClientBase::removeIqHandler( IqHandler*, int ) {}
  void ClientBase::registerIqHandler( IqHandler*, int ) {}
  void ClientBase::registerStanzaExtension( StanzaExtension* se ) { delete se; }
  void ClientBase::removeStanzaExtension( int ) {}
  const std::string ClientBase::getID() { return "id"; }
}
using namespace gloox;

#define CLIENTBASE_H__
#define DISCO_H__
#define SIMANAGER_TEST
#include "../../simanager.h"
#include "../../simanager.cpp"

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;
  Tag *t;

  // -------
  {
    name = "empty tag() test";
    SIManager::SI si;
    t = si.tag();
    if( t )
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }
    delete t;
    t = 0;
  }

  StanzaExtensionFactory sef;
  sef.registerExtension( new SIManager::SI() );
  // -------
  {
    name = "SIManager::SI/SEFactory test";
    Tag* f = new Tag( "iq" );
    new Tag( f, "query", "xmlns", XMLNS_SI );
    IQ iq( IQ::Set, JID(), "" );
    sef.addExtensions( iq, f );
    const SIManager::SI* se = iq.findExtension<SIManager::SI>( ExtSI );
    if( se == 0 )
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }
    delete f;
  }


  printf( "SIManager::SI: " );
  if( fail == 0 )
  {
    printf( "OK\n" );
    return 0;
  }
  else
  {
    printf( "%d test(s) failed\n", fail );
    return 1;
  }

}
