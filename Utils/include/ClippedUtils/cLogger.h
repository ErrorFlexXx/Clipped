/*
** Clipped -- a Multipurpose C++ Library.
**
** Copyright (C) 2019-2020 Christian Löpke. All rights reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#pragma once

#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include "cString.h"
#include "cTime.h"

#define Log() Clipped::Logger()  // Used for Control commands e.g. Log().EnableLogfile("path.log");
// Actual logging functions:
#define LogDebug() \
    Clipped::Logger(__FILE__, __FUNCTION__, __LINE__, Clipped::Logger::MessageType::Debug)
#define LogInfo() Clipped::Logger(__FILE__, __FUNCTION__, __LINE__, Clipped::Logger::MessageType::Info)
#define LogWarn() \
    Clipped::Logger(__FILE__, __FUNCTION__, __LINE__, Clipped::Logger::MessageType::Warning)
#define LogError() \
    Clipped::Logger(__FILE__, __FUNCTION__, __LINE__, Clipped::Logger::MessageType::Error)

namespace Clipped
{
    /**
     * @brief Logger - header only library class to Log messages of different kinds.
     */
    class Logger
    {
    public:
        /**
         * @brief MessageType classifier used to differenciate the different log message types.
         */
        enum class MessageType : unsigned int
        {
            Debug,    //!< used for messages that shall only be displayed in debug mode.
            Info,     //!< used for unimportant informations.
            Warning,  //!< used for runtime errors, which don't prevent the app from executing.
            Error,    //!< used for runtime errors, that quits the program execution.
            MessageTypeCount  //!< used to check, if a message type is valid.
        };

        Logger() : Logger("", "", 0, MessageType::Info) {}

        Logger(const char* file, const char* function, int line,
               MessageType type = MessageType::Info)
            : type(type), file(file), function(function), line(line)
        {
        }

        /**
         * @brief ~Logger destroys this Logger (Message) instance and triggers message output.
         */
        ~Logger() { flush(); }

        /**
         * @brief enableLogfile start logging of all messages to a file.
         * @param filepath full path to logfile.
         */
        void enableLogfile(const char* filepath) { flush(MessageType::MessageTypeCount, filepath); }

        /**
         * @brief disableLogfile stops the logging of all messages to a file.
         */
        void disableLogfile()
        {
            flush(MessageType::MessageTypeCount, reinterpret_cast<const char*>(-1));
        }

        /**
         * @brief clearLogfile deletes the current log file and creates a new one.
         */
        void clearLogfile()
        {
            flush(MessageType::MessageTypeCount, reinterpret_cast<const char*>(-2));
        }

        /**
         * @brief setCallbackFunction installs a callback func. that gets called
         *   every time a log message is ready.
         */
        void setCallbackFunction(std::function<void(MessageType, const String&)> callback)
        {
            flush(MessageType::MessageTypeCount, nullptr, &callback);
        }

        /**
         * @brief resetCallbackFunction removes the currently installed callback function.
         */
        void resetCallbackFunction()
        {
            flush(MessageType::MessageTypeCount, nullptr,
                  reinterpret_cast<std::function<void(MessageType, const String&)>*>(-1));
        }

        /**
         * @brief operator<< Switch the current log level of the Logger.
         * @param const MessageType obj new log level.
         * @return reference of this logger object.
         */
        inline Logger& operator<<(const MessageType obj)
        {
            flush(obj);
            return *this;
        }

        /**
         * @brief operator<< string stream value processing.
         * @param T template argument.
         * @return reference of this Logger object.
         */
        template <typename T>
        inline Logger& operator<<(const T& obj)
        {
            message << obj;
            return *this;
        }

    private:
        MessageType type;      //!< Type of this log message
        StringStream message;  //!< Text of this log message.
        String file;           //!< Name of file, in which this log message has been created.
        String function;       //!< Function name in which this log message has been created.
        std::fstream logFile;  //!< Filestream used for log file creation.
        int line;              //!< Line number inside file, in which this message has been created.

        /**
         * @brief GetDecoratedOutput - Builds the final output from type and message.
         * @return the log text.
         */
        String getDecoratedOutput() const
        {
            String output, strMessage;
            strMessage.append(message.str());
            if (0 < strMessage.length())  // Do not prepare empty messages.
            {
                Time now;
                switch (type)
                {
                    case MessageType::Debug:
                    {
                        output += String("[Debug]   ") + now.toString("[%H:%M:%S] ") + strMessage;
                        break;
                    }
                    case MessageType::Info:
                    {
                        output += String("[Info]    ") + now.toString("[%H:%M:%S] ") + strMessage;
                        break;
                    }
                    case MessageType::Warning:
                    {
                        output += String("[Warning] ") + now.toString("[%H:%M:%S] ") + file +
                                  String(":") + String(line) + String(": ") + strMessage;
                        break;
                    }
                    case MessageType::Error:
                    {
                        output += String("[Error]   ") + now.toString("[%H:%M:%S] ") + file +
                                  String(":") + function + String(":") + String(line) +
                                  String(": ") + strMessage;
                        break;
                    }
                    case MessageType::MessageTypeCount:
                    {
                        break;
                    }
                }
            }
            return output;
        }

        /**
         * @brief flush is the function, that actually outputs the log message.
         * @param switchLogLevel used to change the current log level of the Logger.
         * @param setLogFilepath used to set the log filepath and used as control var (clear log,
         * disable, ...)
         */
        void flush(MessageType switchLogLevel = MessageType::MessageTypeCount,
                   const char* setLogFilepath = nullptr,
                   std::function<void(MessageType, const String&)>* setCallback = nullptr)
        {
            // Static class arguments (in header only library style)
            static MessageType logLevel = MessageType::Warning;  // Default: Only output bad news.
            static bool logToFile = false;                       // Default: File log disabled
            static String logFilepath;                           // Default: No file set.
            static std::function<void(MessageType, const String&)> callback;

            if (switchLogLevel <
                MessageType::MessageTypeCount)  // Switch requested and valid level ?
            {
                logLevel = switchLogLevel;
            }
            else if (setLogFilepath)  // Switch logfile ?
            {
                if (setLogFilepath == reinterpret_cast<const char*>(-1))
                    logToFile = false;
                else if (setLogFilepath == reinterpret_cast<const char*>(-2))
                {
                    if (logToFile)
                    {
                        logFile.open(logFilepath.c_str(), std::fstream::out | std::fstream::trunc);
                        if (logFile.good())
                            logFile.close();
                        else
                            LogWarn() << "Cannot clear log file: " << logFilepath;
                    }
                    else
                        LogWarn() << "ClearLogfile called, but EnableLogfile was never called!";
                }
                else
                {
                    logToFile = true;
                    logFilepath = setLogFilepath;
                }
            }
            else if (setCallback)  // Set callback function ?
            {
                if (setCallback ==
                    reinterpret_cast<std::function<void(MessageType, const String&)>*>(
                        -1))  // Reset
                    // ?
                    callback = [](MessageType, const String&) {};
                else
                    callback = *setCallback;
            }
            else if (logLevel <= type)
            {
                String output = getDecoratedOutput();
                if (0 < output.length())  // Do not output empty messages
                {
                    // Log callback task:
                    if (callback) callback(type, output);
                    // Log to std output:
                    if (type >= MessageType::Warning)
                        std::cerr << output << std::endl;
                    else
                        std::cout << output << std::endl;
                    // Log to file:
                    if (logToFile)
                    {
                        logFile.open(logFilepath.c_str(), std::fstream::out | std::fstream::app);
                        if (logFile.good())  // Successfully opened ?
                        {
                            logFile << output << std::endl;
                            logFile.close();
                        }
                        else
                        {
                            LogWarn() << "Cannot write to logfile: " << logFilepath;
                        }
                    }
                }
            }
        }
    };  // class
}  // namespace Clipped
