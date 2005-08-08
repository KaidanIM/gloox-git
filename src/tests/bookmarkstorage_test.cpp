#include "../jclient.h"
#include "../prep.h"
#include "../connectionlistener.h"
#include "../bookmarkhandler.h"
#include "../disco.h"
#include "../bookmarkstorage.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class BookmarkStorageTest : public BookmarkHandler, ConnectionListener
{
  public:
    BookmarkStorageTest() {};
    ~BookmarkStorageTest() {};

    void start()
    {
      setlocale( LC_ALL, "" );

      j = new JClient();
      j->setServer( "example.org" );
      j->setResource( "gloox" );
      j->setUsername( "hurkhurk" );
      j->setPassword( "hurkhurks" );
      j->setAutoPresence( true );
      j->setInitialPriority( 5 );

      j->registerConnectionListener(this );
      j->disco()->setVersion( "pbookmarkTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );

      j->setDebug( true );
      j->set_log_hook();

      b = new BookmarkStorage( j );
      b->registerBookmarkHandler( this );

      j->connect( true );

      delete( j );
    };

    virtual void onConnect()
    {
      b->requestBookmarks();
    };

    virtual void handleBookmarks( BookmarkList bList, ConferenceList cList )
    {
      printf( "received bookmarks...\n" );
      BookmarkList mybList;
      ConferenceList mycList;

      bookmarkListItem bItem;
      bItem.url = "http://camaya.net/gloox";
      bItem.name = "gloox";
      mybList.push_back( bItem );

      bItem.url = "http://jabber.cc";
      bItem.name = "public jabber services";
      mybList.push_back( bItem );

      conferenceListItem cItem;
      cItem.jid = "jdev@conference.jabber.org";
      cItem.name = "jabber development";
      cItem.nick = "myNick";
      mycList.push_back( cItem );

      cItem.jid = "jabberd@conference.jabber.org";
      cItem.name = "jabberd development";
      cItem.nick = "myOtherNick";
      cItem.password = "my password";
      mycList.push_back( cItem );

      b->storeBookmarks( mybList, mycList );
    };

  private:
    JClient *j;
    BookmarkStorage *b;
};

int main( int argc, char* argv[] )
{
  BookmarkStorageTest *t = new BookmarkStorageTest();
  t->start();
  delete( t );
}
