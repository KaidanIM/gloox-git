/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */



#ifndef IQHANDLER_H__
#define IQHANDLER_H__

#include <iksemel.h>
#include <string>

using namespace std;

/**
 * A virtual interface.
 * Derived classes can be registered as IqHandlers with the JClient.
 * Upon an incoming IQ packet @ref handleIq() will be called.
 * @author Jakob Schroeter <js@camaya.net>
 */
class IqHandler
{
  public:
    /**
     * Reimplement this function if you want to be notified about
     * incoming IQs.
     * @param xmlns The XML namespace of the IQ packet
     * @param pak The complete packet for convenience
     */
    virtual void handleIq( const char* xmlns, ikspak* pak ) {};
};

#endif // IQHANDLER_H__
