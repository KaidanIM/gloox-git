/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
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
     * Reimplemented from InfoHandlerFeeder
     */
    virtual void itemAdded( const string& jid );

    /**
     * Reimplemented from InfoHandlerFeeder
     */
    virtual void itemRemoved( const string& jid );

    /**
     * Reimplemented from InfoHandlerFeeder
     */
    virtual void roster( RosterHelper::RosterMap roster );

    /**
     * Reimplemented from InfoHandlerFeeder
     */
    virtual void itemChanged( const string& jid, int status );

    /**
     * Reimplemented from InfoHandlerFeeder
     */
    virtual bool subscriptionRequest( const string& jid, const string& msg );

  private:
    Feeder* c;
};

#endif // FEEDERTEST_H__
