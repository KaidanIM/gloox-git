/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 * published under the GPL
 */


#ifndef WORKERTEST_H__
#define WORKERTEST_H__

#include "../worker/infohandler.h"

#include <string>
using namespace std;

class Worker;

/**
 * A test program using the JClient class.
 * @author Jakob Schroeter <js@camaya.net>
 */

class WorkerTest : public InfoHandlerWorker
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

  private:
    Worker* c;
};

#endif // FEEDERTEST_H__
