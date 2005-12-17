/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
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
       * Virtual destructor.
       */
      virtual ~LogSink();

      /**
       * Use this method to get a reference to the Log object.
       * @return A reference to the Log object.
       */
      static LogSink& instance();

      /**
       * Use this function to log a message with given LogLevel and LogIdentifier.
       * @param level The severity of the logged event.
       * @param area The part of the program/library the message comes from.
       * @param message The actual log message.
       */
      void log( LogLevel level, LogArea area, const std::string& message );

      /**
       * Registers @c lh as object that receives all debug messages of the specified type.
       * Suitable for logging to a file, etc.
       * @param level The LogLevel for this handler.
       * @param areas Bit-wise ORed LogAreas the LogHandler wants to be informed about.
       * @param lh The object to receive exchanged data.
       */
      void registerLogHandler( LogLevel level, int areas, LogHandler *lh );

      /**
       * Removes the given object from the list of log handlers.
       * @param lh The object to remove from the list.
       */
      void removeLogHandler( LogHandler *lh );

      /**
       * You can log to a file by setting its file name here. Subsequent calls will close the old
       * log file and open the new one.
       * @param level The LogLevel. A given level includes all levels of higer importance.
       * @param areas Bitwise ORed LogAreas.
       * @param file The log file's name.
       * @param append New log messages will be appended if @b true, the file will be truncated otherwise.
       * @deprecated You're encouraged to use the LogHandler to write log messages to a file.
       */
      GLOOX_DEPRECATED void setFile( LogLevel level, int areas, const std::string& file, bool append = true );

    private:
      struct LogInfo
      {
        LogLevel level;
        int areas;
      };

      LogSink();
      LogSink( const LogSink& copy ) {};

      typedef std::map<LogHandler*, LogInfo> LogHandlerMap;
      LogHandlerMap m_logHandlers;

      std::string m_file;
      std::ofstream m_ofile;
      LogLevel m_level;
      int m_fileFilter;

  };

}

#endif // LOGSINK_H__
