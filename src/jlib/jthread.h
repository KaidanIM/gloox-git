/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */


#ifndef JTHREAD_H__
#define JTHREAD_H__

#include "thread.h"

#include <iksemelmm.hh>

class JClient;


/**
 * This is the JClient-specific implementation of Thread.
 * @author Jakob Schroeter <js@camaya.net>
 */
class JThread : public Thread
{
  public:
    /**
     * Constructor
     * @param parent An initialised JClient instance.
     */
    JThread( JClient* parent );

    /**
     * Destructor
     */
    virtual ~JThread();

    /**
     * reimplemented from Thread
     * contains the actual thread implementation
     */
    virtual void run();

    /**
     * call this function to end the thread
     */
    void cancel();

  private:
    bool m_cancel;
    JClient* m_parent;
    iksparser* m_parser;
};

#endif // JTHREAD_H__
