/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */



#ifndef MESSAGEHANDLER_H__
#define MESSAGEHANDLER_H__

#include <iksemel.h>
#include <string>

using namespace std;


/**
 * A virtual interface.
 * Derived classes can be registered as MessageHandlers with the JClient.
 * Upon an incoming Message packet @ref handleMessage() will be called.
 * @author Jakob Schroeter <js@camaya.net>
 */
class MessageHandler
{
  public:
    /**
     * Reimplement this function if you want to be notified about
     * incoming messages.
     * @param from The sender's jid
     * @param type The packets type
     * @param msg The actual message content
     */
    virtual void handleMessage( iksid* from, iksubtype type, const char *msg ) {};
};

#endif // MESSAGEHANDLER_H__
