/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */



#ifndef SUBSCRIPTIONHANDLER_H__
#define SUBSCRIPTIONHANDLER_H__

#include <iksemel.h>
#include <string>

using namespace std;


/**
 * A virtual interface.
 * Derived classes can be registered as IqHandlers with the JClient.
 * Upon an incoming Subscription packet @ref handleSubscription() will be called.
 * @author Jakob Schroeter <js@camaya.net>
 */
class SubscriptionHandler
{
  public:
    /**
     * Reimplement this function if you want to be notified about
     * incoming subscriptions.
     * @param from The sender's jid
     * @param type The packet type
     * @param msg The subscription message (reason)
     */
    virtual void handleSubscription( iksid* from, iksubtype type, const char *msg ) {};
};

#endif // SUBSCRIPTIONHANDLER_H__
