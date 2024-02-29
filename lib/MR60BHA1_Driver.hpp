#ifndef MR60BHA1_DRIVER_HPP
#define MR60BHA1_DRIVER_HPP

#include <MR60BHA1_Defines.hpp>

namespace MR60BHA1 {
class MR60BHA1_Driver {
 public:
    MR60BHA1_Driver() {}
    ~MR60BHA1_Driver() {}

    /* @Brief Calculates the Checksum of a complete given frame. */
    const static inline uint8_t calculateFrameChecksum(const uint8_t* frame, size_t frameSize) {
        uint32_t sum = 0;
        for (size_t i = 0; i < frameSize - OFFSET_OF_FRAME_END; i++) {
            sum += frame[i];
        }
        return (sum) % (FULL_BYTE + 1); 
    }

    /* @Brief Calculates the Checksum of a given part. Function for parsing */
    const static inline uint8_t calculateChecksum(const uint8_t* frame, size_t frameSize) {
        uint32_t sum = 0;
        for (size_t i = 0; i < frameSize; i++) {
            sum += frame[i];
        }
        return static_cast<uint8_t>(sum % FULL_BYTE - 1);
    }

    // @Brief Frame assembly 
    static uint16_t assembleFrame(ControlWord controlWord, uint8_t command, 
        const uint8_t* data, uint16_t dataSize, uint8_t* outFrame, size_t outFrameMaxSize) {
        // Start marker
        outFrame[0] = (static_cast<uint16_t>(FrameConstants::START) >> BYTE_SHIFT);
        outFrame[1] = (static_cast<uint16_t>(FrameConstants::START) & FULL_BYTE);

        // Control word and command
        outFrame[2] = static_cast<uint8_t>(controlWord); 
        outFrame[3] = command; 

        // Data length
        outFrame[4] = (dataSize >> BYTE_SHIFT); 
        outFrame[5] = (dataSize & FULL_BYTE);

        // Data
        for (auto i = 0; i < dataSize; i++) {
            outFrame[6 + i] = data[i];
        }

        // Checksum of the current frame part.
        const uint8_t checksum = calculateFrameChecksum(outFrame, FRAME_MINIMAL_SIZE + dataSize);
        outFrame[6 + dataSize] = checksum;
        outFrame[7 + dataSize] = (static_cast<uint16_t>(FrameConstants::END) >> BYTE_SHIFT);
        outFrame[8 + dataSize] = (static_cast<uint16_t>(FrameConstants::END) & FULL_BYTE);
        return (FRAME_MINIMAL_SIZE + dataSize);
    }

    const static inline Packet getFrameValue(const uint8_t* frame, size_t frameSize) {
        FrameErrorCodes validFrame = frameValidation(frame, frameSize);
        Packet newPacket;
        if (validFrame == FrameErrorCodes::OK) {
            newPacket.controlWord = frame[2];
            newPacket.commandWord = frame[3];
            size_t pSize = newPacket.dataLength = MASK(frame[4], frame[5]);
            for (auto i = 0; i < pSize; i++) {
                newPacket.data[i] = frame[FRAME_START_WITHOUT_DATA_SIZE + i];
            }
            newPacket.checkSumByte = frame[FRAME_START_WITHOUT_DATA_SIZE + pSize];
        }
        return newPacket;
    }

private:
    /* Helper Variables, to prevent magic numbers */
    const static inline uint8_t FULL_BYTE                     = 0xFF;
    const static inline uint8_t OFFSET_OF_FRAME_END           = 0x03;
    const static inline uint8_t BYTE_SHIFT                    = 0x08;
    const static inline uint16_t FRAME_MINIMAL_SIZE           = 0x09; // Without data.
    const static inline uint8_t FRAME_START_WITHOUT_DATA_SIZE = 0x06; // Header: start+command+control

    /* @Brief Validation check of the correct frame. */
    const static inline FrameErrorCodes frameValidation(const uint8_t* frame, uint16_t dataSize) {
        if (dataSize < FRAME_MINIMAL_SIZE) { // Minimal frame length check
            return FrameErrorCodes::BELOW_MIN_SIZE;
        }

        const uint16_t startMarker = (frame[0] << BYTE_SHIFT) | frame[1];
        const uint16_t endMarker = (frame[dataSize - 2] << BYTE_SHIFT) | frame[dataSize - 1];

        if (startMarker != static_cast<uint16_t>(FrameConstants::START) || 
            endMarker != static_cast<uint16_t>(FrameConstants::END)) {
            return FrameErrorCodes::INVALID_MARKERS;
        }

        const uint8_t controlWord = frame[2];
        const uint8_t command = frame[3];
        const uint16_t dataLength = MASK(frame[4], frame[5]);

        // Correcting the frame length check to account for all components
        if (dataSize != (dataLength + FRAME_MINIMAL_SIZE)) {
            return FrameErrorCodes::INVALID_LENGTH;
        }

        const uint8_t checksum = frame[dataSize - OFFSET_OF_FRAME_END];
        const uint8_t calculatedChecksum = calculateFrameChecksum(frame, dataSize);
        if (calculatedChecksum != checksum) {
            return FrameErrorCodes::CHECKSUM_FAIL;
        }
        return FrameErrorCodes::OK;
    }
}; // End of class MR60BHA1_Driver.
} // End of namespace MR60BHA1.
#endif // MR60BHA1_DRIVER_HPP