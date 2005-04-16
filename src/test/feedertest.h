/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 * published under the GPL
 */


#ifndef FEEDERTEST_H__
#define FEEDERTEST_H__

#include "../feeder/infohandler.h"

#include <string>
using namespace std;

class Feeder;

/**
 * A test program using the JClient class.
 * @author Jakob Schroeter <js@camaya.net>
 */

class FeederTest : public InfoHandlerFeeder
{

  public:
    /**
     * Constructor
     */
    FeederTest();

    /**
     * Destructor
     */
    virtual ~FeederTest();

    /**
     * Start the programm with this function.
     */
    void start();

    /**
     * reimplemented from InfoHandlerFeeder
     */
    virtual void connected();

    /**
     * reimplemented from InfoHandlerFeeder
     */
    virtual void disconnected();

    /**
     * reimplemented from InfoHandlerFeeder
     */
    virtual void rosterChanged( iksid* from, ikshowtype show );

  private:
    Feeder* c;
};

#endif // FEEDERTEST_H__
