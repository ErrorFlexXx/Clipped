#include <ClippedMaths/cCRC.h>
#include <ClippedUtils/cString.h>
#include <ClippedUtils/cLogger.h>

using namespace Clipped;

bool testCRC8();
bool testCRC16();
bool testCRC32();
bool testCRC32BZip2();

int main(void)
{
    Logger() << Logger::MessageType::Debug;

    bool result = true;
    result &= testCRC8();
    result &= testCRC16();
    result &= testCRC32();
    result &= testCRC32BZip2();

    return !result;
}

bool testCRC8()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    CRC8 crc8Crypt(0x07); //Setup crypt with Poly and default init value.
    String testData("123456789");
    uint8_t expectedResult = 0xF4u;
    uint8_t result = crc8Crypt.encrypt(reinterpret_cast<const uint8_t*>(testData.data()), testData.size());

    if(result == expectedResult)
    {
        LogInfo() << "ASCII data: " << testData << " CRC8: 0x" << std::hex << (int)result << " ok.";
        return true;
    }
    else
    {
        LogError() << "ASCII data: " << testData << " CRC8: 0x" << std::hex << (int)result << " != expected: " << (int)expectedResult << "! NOK!";
        return false;
    }
}

bool testCRC16()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    CRC16 crc16Crypt(0x1021, 0xFFFFu); //Setup crypt..
    String testData("123456789");
    uint16_t expectedResult = 0x29B1u;
    uint16_t result = crc16Crypt.encrypt(reinterpret_cast<const uint8_t*>(testData.data()), testData.size());

    if(result == expectedResult)
    {
        LogInfo() << "ASCII data: " << testData << " CRC16: 0x" << std::hex << (int)result << " ok.";
        return true;
    }
    else
    {
        LogError() << "ASCII data: " << testData << " CRC16: 0x" << std::hex << (int)result << " != expected: " << (int)expectedResult << "! NOK!";
        return false;
    }
}

bool testCRC32()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    CRC32 crc32(0x04C11DB7, 0xFFFFFFFFu, 0xFFFFFFFFu, true, true); //Setup crypt..
    String testData("123456789");
    uint32_t expectedResult = 0xCBF43926;
    uint32_t result = crc32.encrypt(reinterpret_cast<const uint8_t*>(testData.data()), testData.size());

    if(result == expectedResult)
    {
        LogInfo() << "ASCII data: " << testData << " crc32: 0x" << std::hex << (int)result << " ok.";
        return true;
    }
    else
    {
        LogError() << "ASCII data: " << testData << " crc32: 0x" << std::hex << (int)result << " != expected: " << (int)expectedResult << "! NOK!";
        return false;
    }
}

bool testCRC32BZip2()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    CRC32 crc32BZip2(0x04C11DB7, 0xFFFFFFFFu, 0xFFFFFFFFu); //Setup crypt..
    String testData("123456789");
    uint32_t expectedResult = 0xFC891918;
    uint32_t result = crc32BZip2.encrypt(reinterpret_cast<const uint8_t*>(testData.data()), testData.size());

    if(result == expectedResult)
    {
        LogInfo() << "ASCII data: " << testData << " crc32BZip2: 0x" << std::hex << (int)result << " ok.";
        return true;
    }
    else
    {
        LogError() << "ASCII data: " << testData << " crc32BZip2: 0x" << std::hex << (int)result << " != expected: " << (int)expectedResult << "! NOK!";
        return false;
    }
}
