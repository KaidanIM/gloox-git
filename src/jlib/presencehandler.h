/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */



#ifndef PRESENCEHANDLER_H__
#define PRESENCEHANDLER_H__

#include <iksemel.h>
#include <string>

using namespace std;


/**
 * A virtual interface.
 * Derived classes can be registered as IqHandlers with the JClient.
 * Upon an incoming Presence packet @ref handlePresence() will be called.
 * @author Jakob Schroeter <js@camaya.net>
 */
class PresenceHandler
{
  public:
    /**
     * Reimplement this function if you want to be updated on
     * incoming presence notifications.
     * @param from The sender's jid
     * @param type The presence type
     * @param show The presence's status
     * @param msg The status message
     */
    virtual void handlePresence( iksid* from, iksubtype type, ikshowtype show, const char* msg ) {};
};

#endif // PRESENCEHANDLER_H__
