/**
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 * published under the GPL
 */


#ifndef CONNECT_H__
#define CONNECT_H__

#include <string>
using namespace std;

class JClient;

/**
 * A test program using the JClient class.
 * @author Jakob Schroeter <js@camaya.net>
 */
class Connect
{

  public:
    /**
     * Constructor
     */
    Connect();

    /**
     * Destructor
     */
    virtual ~Connect();

    /**
     * Start the programm with this function.
     */
    void start();

  private:
    JClient *c;
};

#endif // CONNECT_H__
