/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */


#ifndef POLLHANDLER_H__
#define POLLHANDLER_H__


/**
 * A virtual interface.
 * Derived classes can be registered as PollHandlers.
 * poll() will be called by the Feeder if it has workers available.
 * @author Jakob Schroeter <js@camaya.net>
 */
class PollHandler
{
  public:
    /**
     * reimplment this to be able to provide data upon request.
     * @return char* data
     * @return NULL if no data is available
     */
    virtual char* poll();

};

#endif // POLLHANDLER_H__

