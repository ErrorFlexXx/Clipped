/* Copyright 2019 Christian Löpke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
        ~Logger() { Flush(); }

        /**
         * @brief EnableLogfile start logging of all messages to a file.
         * @param filepath full path to logfile.
         */
        void EnableLogfile(const char* filepath) { Flush(MessageType::MessageTypeCount, filepath); }

        /**
         * @brief DisableLogfile stops the logging of all messages to a file.
         */
        void DisableLogfile()
        {
            Flush(MessageType::MessageTypeCount, reinterpret_cast<const char*>(-1));
        }

        /**
         * @brief ClearLogfile deletes the current log file and creates a new one.
         */
        void ClearLogfile()
        {
            Flush(MessageType::MessageTypeCount, reinterpret_cast<const char*>(-2));
        }

        /**
         * @brief SetCallbackFunction installs a callback func. that gets called
         *   every time a log message is ready.
         */
        template <typename T>
        void SetCallbackFunction(std::function<void(MessageType, const String&)> callback)
        {
            Flush(MessageType::MessageTypeCount, nullptr, &callback);
        }

        /**
         * @brief ResetCallbackFunction removes the currently installed callback function.
         */
        void ResetCallbackFunction()
        {
            Flush(MessageType::MessageTypeCount, nullptr,
                  reinterpret_cast<std::function<void(MessageType, const String&)>*>(-1));
        }

        /**
         * @brief operator<< Switch the current log level of the Logger.
         * @param const MessageType obj new log level.
         * @return reference of this logger object.
         */
        inline Logger& operator<<(const MessageType obj)
        {
            Flush(obj);
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
        String GetDecoratedOutput() const
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
         * @brief Flush is the function, that actually outputs the log message.
         * @param switchLogLevel used to change the current log level of the Logger.
         * @param setLogFilepath used to set the log filepath and used as control var (clear log,
         * disable, ...)
         */
        void Flush(MessageType switchLogLevel = MessageType::MessageTypeCount,
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
                String output = GetDecoratedOutput();
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
