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

  void LogSink::log( LogLevel level, LogArea area, const std::string& message )
  {
    LogHandlerMap::const_iterator it = m_logHandlers.begin();
    for( ; it != m_logHandlers.end(); ++it )
    {
      if( ( (*it).second.level <= level ) && ( (*it).second.areas & area ) )
        (*it).first->handleLog( level, area, message );
    }

    if( ( m_level <= level ) && ( m_fileFilter & area ) && !m_file.empty() )
      m_ofile << level << "::" << area << ": " << message << std::endl;
  }

  void LogSink::setFile( LogLevel level, int areas, const std::string& file, bool append )
  {
    if( !m_file.empty() )
      m_ofile.close();

    m_ofile.open( file.c_str(), (append)?(std::ios::out|std::ios::app):(std::ios::out|std::ios::trunc) );
    m_file = file;
    m_fileFilter = areas;
    m_level = level;
  }

  void LogSink::registerLogHandler( LogLevel level, int areas, LogHandler *lh )
  {
    LogInfo info = { level, areas };
    m_logHandlers[lh] = info;
  }

  void LogSink::removeLogHandler( LogHandler *lh )
  {
    m_logHandlers.erase( lh );
  }

}
