#ifndef MR60BHA1_DRIVER_HPP
#define MR60BHA1_DRIVER_HPP

#include <MR60BHA1_Defines.hpp>

namespace MR60BHA1 {
class MR60BHA1_Driver {
 public:
    MR60BHA1_Driver() {}
    ~MR60BHA1_Driver() {}

    const static inline uint8_t calculateChecksum(const std::vector<uint8_t>& frame) {
        uint32_t sum = 0;
        for (size_t i = 0; i < frame.size() - OFFSET_OF_FRAME_END; i++) {
            sum += frame[i];
        }
        return (sum) % (FULL_BYTE+1); 
    }

    const static inline uint8_t calculateFrameChecksum(const std::vector<uint8_t>& frame) {
        auto tFrame = frame;
        uint32_t sum = 0;
        for (size_t i = 0; i < frame.size(); i++) {
            sum += frame[i];
        }
        return sum % (FULL_BYTE)-1; 
    }

    const static inline Packet getFrameValue(const std::vector<uint8_t>& frame) {
        FrameErrorCodes validFrame = frameValidation(frame);
        Packet newPacket;
        if (validFrame == FrameErrorCodes::OK) {
            newPacket.controlWord = frame[2];
            newPacket.commandWord = frame[3];
            size_t pSize = newPacket.dataLength = MASK(frame[4], frame[5]);
            for (auto i = 0; i < pSize; i++) {
                newPacket.data[i] = frame[6+i];
            }
            newPacket.checkSumByte = frame[6+pSize];
        }
        return newPacket;
    }

    // Frame assembly function
    const static inline std::vector<uint8_t> assembleFrame(ControlWord controlWord, uint8_t command, const std::vector<uint8_t>& data) {
        std::vector<uint8_t> frame;
        // Start marker
        frame.push_back(static_cast<uint16_t>(FrameConstants::START) >> BYTE_SHIFT);
        frame.push_back(static_cast<uint16_t>(FrameConstants::START) & FULL_BYTE);
        // Control word and command
        frame.push_back(static_cast<uint8_t>(controlWord));
        frame.push_back(command);
        // Data length
        uint16_t dataLength = data.size();
        frame.push_back(dataLength >> BYTE_SHIFT);
        frame.push_back(dataLength & 0xFF);
        // Data
        frame.insert(frame.end(), data.begin(), data.end());
        // Checksum
        uint8_t checksum = calculateFrameChecksum(frame); // Calculate based on the content before checksum
        frame.push_back(checksum);

        frame.push_back(static_cast<uint16_t>(FrameConstants::END) >> BYTE_SHIFT);
        frame.push_back(static_cast<uint16_t>(FrameConstants::END) & FULL_BYTE);
        return frame;
    }

    const static inline std::vector<uint8_t> assembleFrame(ControlWord controlWord, uint8_t command, uint8_t data) {
        std::vector<uint8_t> frame;
        // Start marker
        frame.push_back(static_cast<uint16_t>(FrameConstants::START) >> BYTE_SHIFT);
        frame.push_back(static_cast<uint16_t>(FrameConstants::START) & FULL_BYTE);
        
        // Control word and command
        frame.push_back(static_cast<uint8_t>(controlWord));
        frame.push_back(command);

        // Data length
        frame.push_back(0x00); // First length Byte.
        frame.push_back(0x01); // Second lenght Byte.
        // Data
        frame.push_back(data);
        // Checksum
        uint8_t checksum = calculateFrameChecksum(frame); // Calculate based on the content before checksum
        frame.push_back(checksum);
        frame.push_back(static_cast<uint16_t>(FrameConstants::END) >> BYTE_SHIFT);
        frame.push_back(static_cast<uint16_t>(FrameConstants::END) & FULL_BYTE);
        return frame;
    }

    const static inline std::vector<uint8_t> hexStringToBytes(std::stringstream& hexStream) {
        std::vector<uint8_t> bytes;
        char c;
        while (hexStream.get(c)) { // Directly check stream state
            if (!std::isxdigit(c)) {
                throw std::invalid_argument("Invalid hex character");
            }
            uint8_t byte = (c >= '0' && c <= '9') ? (c - '0') : ((c & ~32) - 'A' + 10);
            if (hexStream.get(c)) { // Attempt to read the second hex digit
                if (!std::isxdigit(c)) {
                    throw std::invalid_argument("Invalid hex character");
                }
                byte <<= 4;
                byte |= (c >= '0' && c <= '9') ? (c - '0') : ((c & ~32) - 'A' + 10);
            } else {
                throw std::invalid_argument("Incomplete hex pair");
            }
            bytes.push_back(byte);
        }
        return bytes;
    }
private:
    /* Helper Variables, to prevent magic numbers */
    static inline constexpr uint8_t FULL_BYTE           = 0xFF;
    static inline constexpr uint8_t OFFSET_OF_FRAME_END = 3;
    static inline constexpr uint8_t BYTE_SHIFT          = 8;

    /* @Brief Validation check of the correct frame. */
    const static inline FrameErrorCodes frameValidation(const std::vector<uint8_t>& frame) {
        if (frame.size() < 8) { // Minimal frame length check
            return FrameErrorCodes::BELOW_MIN_SIZE;
        }

        const uint16_t startMarker = (frame[0] << BYTE_SHIFT) | frame[1];
        const uint16_t endMarker = (frame[frame.size() - 2] << BYTE_SHIFT) | frame[frame.size() - 1];

        if (startMarker != static_cast<uint16_t>(FrameConstants::START) || 
            endMarker != static_cast<uint16_t>(FrameConstants::END)) {
            return FrameErrorCodes::INVALID_MARKERS;
        }

        const uint8_t controlWord = frame[2];
        const uint8_t command = frame[3];
        const uint16_t dataLength = MASK(frame[4],frame[5]);

        // Correcting the frame length check to account for all components
        if (frame.size() != (6 + dataLength + 3)) { // 6 bytes (start marker, control, command, data length) + dataLength + 3 bytes (checksum, end marker)
            return FrameErrorCodes::INVALID_LENGTH;
        }

        const uint8_t checksum = frame[frame.size() - 3];
        const uint8_t calculatedChecksum = calculateChecksum(frame);
        if (calculatedChecksum != checksum) {
            return FrameErrorCodes::CHECKSUM_FAIL;
        }
        return FrameErrorCodes::OK;
    }
}; // End of class MR60BHA1_Driver.
} // End of namespace MR60BHA1.
#endif // MR60BHA1_DRIVER_HPP