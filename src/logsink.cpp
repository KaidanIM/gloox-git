/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#include "logsink.h"

namespace gloox
{

  LogSink::LogSink()
    : m_level( LOG_ERROR ), m_fileFilter( 0 )
  {
  }

  LogSink::~LogSink()
  {
  }

  LogSink& LogSink::instance()
  {
    static LogSink sink;
    return sink;
  }

  void LogSink::log( LogLevel level, LogArea identifier, const std::string& message )
  {
    LogHandlerMap::const_iterator it = m_logHandlers.begin();
    for( ; it != m_logHandlers.end(); ++it )
    {
      if( (*it).second & identifier )
        (*it).first->handleLog( level, identifier, message );
    }

    if( ( level >= m_level ) && ( m_fileFilter & identifier ) && !m_file.empty() )
      m_ofile << level << "::" << identifier << ": " << message << std::endl;
  }

  void LogSink::setFile( LogLevel level, int identifiers, const std::string& file, bool append )
  {
    if( !m_file.empty() )
      m_ofile.close();

    m_ofile.open( file.c_str(), (append)?(std::ios::out|std::ios::app):(std::ios::out|std::ios::trunc) );
    m_file = file;
    m_fileFilter = identifiers;
    m_level = level;
  }

  void LogSink::registerLogHandler( int identifiers, LogHandler *lh )
  {
    m_logHandlers[lh] = identifiers;
  }

  void LogSink::removeLogHandler( LogHandler *lh )
  {
    m_logHandlers.erase( lh );
  }

}
