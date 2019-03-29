#pragma once

#include <ClippedUtils/cString.h>
#include <vector>
#include <atomic>

namespace Clipped
{
    /**
     * @brief The IODevice class is an interface for an generic input output device.
     *   This can be a serial communicaton, network communication or something else.
     */
    class IODevice
    {
    public:
        /**
         * @brief The OpenMode specifying the access right we request.
         */
        enum OpenMode
        {
            Read,
            Write,
            ReadWrite
        };

        virtual ~IODevice() {}

        virtual bool open(const IODevice::OpenMode& mode) = 0;

        virtual bool readAll(std::vector<char>& data) = 0;

        virtual bool read(std::vector<char>& data, size_t count) = 0;

        virtual bool write(const String& data) = 0;

        virtual bool write(const std::vector<char>& data) = 0;

        virtual bool close() = 0;

        bool getIsOpen() const { return isOpen; }

    protected:
        std::atomic<bool> isOpen;
        OpenMode mode; //!< Stored open mode.
    };

}
