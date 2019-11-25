#include "cCRC.h"
#include "cFormat.h"
#include <ClippedUtils/cLogger.h>

using namespace std;
using namespace Clipped;

template <class T>
CRC<T>::CRC(const T generator, const T init, const T finalXOR, const bool reflectInput, const bool reflectResult)
    : init(init)
    , reflectInput(reflectInput)
    , reflectResult(reflectResult)
    , finalXOR(finalXOR)
    , generator(generator)
{
    calculateTable();
}

template <class T>
void CRC<T>::calculateTable()
{
    for(int i = 0; i < 256; i++)
    {
        T currentByte = static_cast<T>(i << ((sizeof(T) - 1) * 8));

        for(unsigned char bit = 0; bit < 8; bit++)
        {
            if((currentByte & (1 << (sizeof(T) * 8) - 1)) != 0)
            {
                currentByte <<= 1;
                currentByte ^= generator;
            }
            else
            {
                currentByte <<= 1;
            }
        }
        lookupTable.push_back(currentByte);
    }
}

template <class T>
T CRC<T>::encrypt(const uint8_t* data, size_t length) const
{
    T crc = init;

    for(size_t i = 0; i < length; i++)
    {
        uint8_t dataByte = data[i];
        if(reflectInput)
        {
            dataByte = reflect(dataByte);
        }
        uint8_t tmp = (crc >> ((sizeof(T) - 1) * 8)) ^ dataByte;
        crc = (crc << 8) ^ lookupTable[static_cast<uint8_t>(tmp)];
    }
    if(reflectResult)
    {
        crc = reflect(crc);
    }

    return crc ^ finalXOR;
}

template <class T>
T CRC<T>::encrypt(const std::vector<uint8_t>& data) const
{
    return encrypt(reinterpret_cast<const uint8_t*>(data.data()), data.size());
}

// Please compile template class for the following types:
template class CRC<uint8_t>;
template class CRC<uint16_t>;
template class CRC<uint32_t>;
