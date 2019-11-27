#include <ClippedMaths/cCRC.h>
#include <ClippedUtils/cString.h>
#include <ClippedUtils/cLogger.h>

using namespace Clipped;

template<typename T>
bool test(const String &data, T expResult, CRC<T>& crc)
{
    crc.begin();
    crc.update(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    T result = crc.finish();
    if(result == expResult)
    {
        LogInfo() << crc.getName() << " success.\tResult = 0x" << std::hex << (int)result;
        return true;
    }
    LogError() << crc.getName() << " failed!\n\tExpected: 0x" << std::hex << (int)expResult << " but got: 0x" << std::hex << (int)result;
    return false;
}

bool testCRC32BZip2PartialUpdate();

int main(void)
{
    Logger() << Logger::MessageType::Debug;

    bool result = true;

    { CRC8 crc(0x07);   result &= test("123456789", (uint8_t)0xF4u, crc); }
    { CRC8CDMA2000 crc; result &= test("123456789", (uint8_t)0xDAu, crc); }
    { CRC8DARC crc;     result &= test("123456789", (uint8_t)0x15u, crc); }
    { CRC8DVBS2 crc;    result &= test("123456789", (uint8_t)0xBCu, crc); }
    { CRC8EBU crc;      result &= test("123456789", (uint8_t)0x97u, crc); }
    { CRC8ICODE crc;    result &= test("123456789", (uint8_t)0x7Eu, crc); }
    { CRC8ITU crc;      result &= test("123456789", (uint8_t)0xA1u, crc); }
    { CRC8MAXIM crc;    result &= test("123456789", (uint8_t)0xA1u, crc); }
    { CRC8ROHC crc;     result &= test("123456789", (uint8_t)0xD0u, crc); }
    { CRC8WCDMA crc;    result &= test("123456789", (uint8_t)0x25u, crc); }
    { CRC16CCITTFALSE crc;  result &= test("123456789", (uint16_t)0x29B1, crc); }
    { CRC32 crc(0x04C11DB7, 0xFFFFFFFFu, 0xFFFFFFFFu, true, true); result &= test("123456789", (uint32_t)0xCBF43926u, crc); }
    { CRC32BZip2 crc;   result &= test("123456789", (uint32_t)0xFC891918u, crc); }
    { CRC64ECMA182 crc;   result &= test("123456789", (uint64_t)0x6c40df5f0b497347u, crc); }
    { CRC64XZ crc;   result &= test("123456789", (uint64_t)0x995dc9bbdf1939fa, crc); }

    result &= testCRC32BZip2PartialUpdate();

    return !result;
}

bool testCRC32BZip2PartialUpdate()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    CRC32BZip2 crc; //Setup crypt..
    String testDataFirstPart("1234");
    String testDataSecondPart("56789");
    uint32_t expectedResult = 0xFC891918;
    crc.begin();
    crc.update(reinterpret_cast<const uint8_t*>(testDataFirstPart.data()), testDataFirstPart.size());
    crc.update(reinterpret_cast<const uint8_t*>(testDataSecondPart.data()), testDataSecondPart.size());
    uint32_t result = crc.finish();

    if(result == expectedResult)
    {
        LogInfo() << "ASCII data: " << testDataFirstPart << testDataSecondPart << " CRC32BZip2 Partial Update: 0x" << std::hex << (int)result << " ok.";
        return true;
    }
    else
    {
        LogError() << "ASCII data: " << testDataFirstPart << testDataSecondPart << " CRC32BZip2 Partial Update: 0x" << std::hex << (int)result << " != expected: " << (int)expectedResult << "! NOK!";
        return false;
    }
}
