/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef LOGSINK_H__
#define LOGSINK_H__

#include "gloox.h"
#include "loghandler.h"

#include <string>
#include <fstream>

namespace gloox
{

  /**
   * @brief An implementation of log sink and source.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_EXPORT LogSink
  {
    public:
      /**
       *
       */
      virtual ~LogSink();

      /**
       * Use this method to get a reference to the Log object.
       * @return A reference to the Log object.
       */
      static LogSink& instance();

      /**
       *
       */
      void log( LogLevel level, LogIdentifier identifier, const std::string& message );

      /**
       * Registers @c lh as object that receives all debug messages of the specified type.
       * Suitable for logging to a file, etc.
       * @param identifiers ORed identifiers the LogHandler wants to be informed about.
       * @param lh The object to receive exchanged data.
       */
      void registerLogHandler( int identifiers, LogHandler *lh );

      /**
       * Removes the given object from the list of log handlers.
       * @param lh The object to remove from the list.
       */
      void removeLogHandler( LogHandler *lh );

      /**
       * You can log to a file by setting its file name here. Subsequent calls will close the old
       * log file and open the new one.
       * @param level The LogLevel. A given level includes all levels of higer importance.
       * @param identifiers Bitwise ORed LogIdentifiers.
       * @param file The log file's name.
       * @param append @b New log messages will be appended if @b true, the file will be truncated otherwise.
       * @deprecated
       */
      GLOOX_DEPRECATED void setFile( LogLevel level, int identifiers,
                                     const std::string& file, bool append = true );

    private:
      LogSink();
      LogSink( const LogSink& copy ) {};

      typedef std::map<LogHandler*, int> LogHandlerMap;
      LogHandlerMap m_logHandlers;

      std::string m_file;
      std::ofstream m_ofile;
      LogLevel m_level;
      int m_fileFilter;

  };

}
#endif // LOGSINK_H__
