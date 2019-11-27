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

#include <ClippedUtils/cString.h>
#include <typeinfo>
#include <stdint.h>
#include <vector>

namespace Clipped
{
    template <class T>
    class CRC
    {
    public:
        CRC(const T generator, const T init = 0, const T finalXOR = 0, const bool reflectInput = false, const bool reflectResult = false, const String& name = "Custom CRC");

        /**
         * @brief begin initilizes the object.
         */
        void begin();

        /**
         * @brief update updates the crc with new data.
         *   This method is intended to be called repeatedly, till all data is fed in.
         * @param data to generate the CRC for.
         * @param length length of data field.
         */
        void update(const uint8_t* data, size_t length);

        /**
         * @brief update updates the crc with new data.
             This method is intended to be called repeatedly, till all data is fed in.
            @param data to generate crc for.
         */
        void update(const std::vector<uint8_t>& data);

        /**
         * @brief finish creates the final CRC value.
         * @return the calculated CRC.
         */
        T finish();

        /**
         * @brief getName return the name of this CRC instance.
         * @return the name as String.
         */
        const String& getName() const { return name; }

    private:
        String name;                //!< Optional name of the configuration, if any known.
        T init;                     //!< Init value for calculation.
        T crc;                      //!< Current stored CRC for incremental calculation.
        bool reflectInput;          //!< Flag stating, if the input bits shall be "bitmirrored"
        bool reflectResult;         //!< Flag stating, if the result shall be "bitmirrored"
        T finalXOR;                 //!< Final XOR operation.
        T generator;                //!< Generator polynom used for calculation.
        std::vector<T> lookupTable; //!< Lookup table with precalculations.

        /**
         * @brief calculateTable fills the lookup table with precalculated data as performance optimization.
         */
        void calculateTable();

    }; //class CRC

    using CRC8 = CRC<uint8_t>;
    using CRC16 = CRC<uint16_t>;
    using CRC32 = CRC<uint32_t>;
    using CRC64 = CRC<uint64_t>;

    /* Catalogue of parametrised CRC algorithms: */

    class CRC8CDMA2000      : public CRC8 { public: CRC8CDMA2000()      : CRC8(0x9Bu, 0xffu, 0, false, false, "CRC8-CDMA2000") {} };
    class CRC8DARC          : public CRC8 { public: CRC8DARC()          : CRC8(0x39u, 0, 0, true, true, "CRC8-DARC") {} };
    class CRC8DVBS2         : public CRC8 { public: CRC8DVBS2()         : CRC8(0xD5u, 0, 0, false, false, "CRC8-DVB-S2") {} };
    class CRC8EBU           : public CRC8 { public: CRC8EBU()           : CRC8(0x1Du, 0xffu, 0, true, true, "CRC8-EBU") {} };
    class CRC8ICODE         : public CRC8 { public: CRC8ICODE()         : CRC8(0x1Du, 0xfdu, 0, false, false, "CRC8-I-CODE") {} };
    class CRC8ITU           : public CRC8 { public: CRC8ITU()           : CRC8(0x07u, 0, 0x55u, false, false, "CRC8-ITU") {} };
    class CRC8MAXIM         : public CRC8 { public: CRC8MAXIM()         : CRC8(0x31, 0, 0, true, true, "CRC8-MAXIM") {} };
    class CRC8ROHC          : public CRC8 { public: CRC8ROHC()          : CRC8(0x07, 0xffu, 0, true, true, "CRC8-ROHC") {} };
    class CRC8WCDMA         : public CRC8 { public: CRC8WCDMA()         : CRC8(0x9B, 0, 0, true, true, "CRC8-WCDMA") {} };
    class CRC16CCITTFALSE   : public CRC16 { public: CRC16CCITTFALSE()  : CRC16(0x1021u, 0xffffu, 0, false, false, "CRC16-CCITT-FALSE") {} };
    class CRC32BZip2        : public CRC32 { public: CRC32BZip2()       : CRC32(0x04C11DB7, 0xFFFFFFFFu, 0xFFFFFFFFu, false, false, "CRC32BZip2") {} };
    class CRC32AIXM         : public CRC32 { public: CRC32AIXM()        : CRC32(0x814141abu, 0, 0, false, false, "CRC32AIXM") {} };
    class CRC32AUTOSAR      : public CRC32 { public: CRC32AUTOSAR()     : CRC32(0xf4acfb13u, 0xffffffffu, 0xffffffffu, true, true, "CRC32AUTOSAR") {} };
    class CRC32BASE91D      : public CRC32 { public: CRC32BASE91D()     : CRC32(0xa833982bu, 0xffffffffu, 0xffffffffu, true, true, "CRC32BASE91D") {} };
    class CRC32CDROMEDC     : public CRC32 { public: CRC32CDROMEDC()    : CRC32(0x8001801bu, 0, 0, true, true, "CRC32CDROMEDC") {} };
    class CRC32CKSUM        : public CRC32 { public: CRC32CKSUM()       : CRC32(0x04c11db7u, 0, 0xffffffffu, false, false, "CRC32CKSUM") {} };
    class CRC32ISCSI        : public CRC32 { public: CRC32ISCSI()       : CRC32(0x1edc6f41u, 0xffffffffu, 0xffffffffu, true, true, "CRC32ISCSI") {} };
    class CRC32ISOHDLC      : public CRC32 { public: CRC32ISOHDLC()     : CRC32(0x04c11db7u, 0xffffffffu, 0xffffffffu, true, true, "CRC32ISOHDLC") {} };
    class CRC32JAMCRC       : public CRC32 { public: CRC32JAMCRC()      : CRC32(0x04c11db7u, 0xffffffffu, 0, true, true, "CRC32JAMCRC") {} };
    class CRC32MPEG2        : public CRC32 { public: CRC32MPEG2()       : CRC32(0x04c11db7u, 0xffffffffu, 0, false, false, "CRC32MPEG2") {} };
    class CRC32XFER         : public CRC32 { public: CRC32XFER()        : CRC32(0x000000afu, 0, 0, false, false, "CRC32XFER") {} };
    class CRC64ECMA182      : public CRC64 { public: CRC64ECMA182()     : CRC64(0x42F0E1EBA9EA3693u, 0x0, 0x0u, false, false, "CRC64ECMA182") {} };
    class CRC64XZ           : public CRC64 { public: CRC64XZ()          : CRC64(0x42f0e1eba9ea3693u, 0xffffffffffffffffu, 0xffffffffffffffffu, true, true, "CRC64XZ") {} };

} //namespace Clipped
