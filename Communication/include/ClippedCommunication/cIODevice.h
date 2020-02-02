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
