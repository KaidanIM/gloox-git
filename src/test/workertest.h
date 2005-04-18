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


#ifndef WORKERTEST_H__
#define WORKERTEST_H__

#include "../worker/infohandler.h"
#include "../worker/datahandler.h"

#include <string>
using namespace std;

class Worker;

/**
 * A test program using the JClient class.
 * @author Jakob Schroeter <js@camaya.net>
 */

class WorkerTest : public InfoHandlerWorker, DataHandler
{

  public:
    /**
     * Constructor
     */
    WorkerTest();

    /**
     * Destructor
     */
    virtual ~WorkerTest();

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
     * reimplemented from DataHandler
     */
    virtual void data( const char* data);

  private:
    Worker* c;
};

#endif // FEEDERTEST_H__
