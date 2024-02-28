#ifndef MR60BHA1_DEFINES_HPP
#define MR60BHA1_DEFINES_HPP

#include <stdint.h>

#define MASK(value, value2) (static_cast<uint16_t>(value) << 8 | (value2))
#define MAX_DATA_SIZE 6

namespace MR60BHA1 {

enum class FrameConstants : uint16_t {
    START           = MASK(0x53, 0x59),             /* FH - Frame header fixed sequence, ASCII: "S Y" */
    END             = MASK(0x54, 0x43),             /* FT - End of frame fixed sequence, ASCII: "T C" */    
};

/* Specification of which part you select to control. */
enum class ControlWord : uint8_t {
    PACKET_HEARTBEAT    = 0x01,
    PRODUCT_ID          = 0x02,
    OTA_UPGRADE         = 0x03,
    OPERATION_STAUS     = 0x05,
    RANGE_INFO          = 0x07,
    HUMAN_PRESENCE      = 0x80,
    RESPIRATORY_INFO    = 0x81,
    SLEEP_INFO          = 0x84,
    HEART_INFO          = 0x85,
};

/* Concatenation of Control+Command Word. Must be hard-coded ugly due to poor protocol design */
enum class ReferenceWord : uint16_t {
    HEART_RATE_SWITCH           = MASK(ControlWord::HEART_INFO,0x01),
    HEART_RATE_INTENSITY        = MASK(ControlWord::HEART_INFO,0x03),
    HEART_RATE_CONFIDENCE       = MASK(ControlWord::HEART_INFO,0x04),
    HEART_RATE_WAVEFORM         = MASK(ControlWord::HEART_INFO,0x05),
    BREATH_RATE_INTENSITY       = MASK(ControlWord::HEART_INFO,0x03),
    BREATH_RATE_CONFIDENCE      = MASK(ControlWord::HEART_INFO,0x04),
    BREATH_RATE_WAVEFORM        = MASK(ControlWord::HEART_INFO,0x05),
    OUT_OF_BOUNDS_STATUS        = MASK(ControlWord::RANGE_INFO,0x07),
    MODULE_RESET                = MASK(ControlWord::PACKET_HEARTBEAT, 0x02), 
};

struct Packet {
    static const FrameConstants startHeader = FrameConstants::START;
    uint8_t controlWord;
    uint8_t commandWord;
    uint16_t dataLength;
    uint8_t data[MAX_DATA_SIZE];
    uint8_t checkSumByte;
    static const FrameConstants endHeader = FrameConstants::END;
};

enum class FrameErrorCodes : uint8_t {
    BELOW_MIN_SIZE,
    INVALID_MARKERS,
    INVALID_LENGTH,
    CHECKSUM_FAIL,
    OK,
};

} // End of namespace MR60BHA1;
#endif // MR60BHA1_DEFINES_HPP