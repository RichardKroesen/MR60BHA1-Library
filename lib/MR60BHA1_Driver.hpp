#ifndef MR60BHA1_DRIVER_HPP
#define MR60BHA1_DRIVER_HPP

#include <cstring>
#include <MR60BHA1_Defines.hpp>

namespace MR60BHA1 {
class MR60BHA1_Driver {
 public:
    MR60BHA1_Driver() {}
    ~MR60BHA1_Driver() {}

    /* @Brief Calculates the Checksum of a complete frame. */
    const static inline uint8_t calculateFrameChecksum(const uint8_t* frame, const size_t frameSize, 
    size_t offset = OFFSET_OF_FRAME_END) {
        uint32_t sum = 0;
        for (size_t i = 0; i < frameSize - offset; i++) {
            sum += frame[i];
        }
        return (sum) & (FULL_BYTE); 
    }

    // @Brief Frame assembly 
    const static inline uint16_t assembleFrame(const ControlWord controlWord, const uint8_t command, 
        const uint8_t* data, const uint16_t dataSize, uint8_t* outFrame, const uint16_t outFrameMaxSize) {

        const uint16_t totalFrameSize = FRAME_MINIMAL_SIZE + dataSize;
        if (totalFrameSize > outFrameMaxSize) return 0;

        // Start markers
        outFrame[0] = START_BYTE_1;
        outFrame[1] = START_BYTE_2;

        // Control word and command
        outFrame[2] = static_cast<uint8_t>(controlWord); 
        outFrame[3] = command; 

        // Data length
        outFrame[4] = (dataSize >> BYTE_SHIFT); 
        outFrame[5] = (dataSize & FULL_BYTE);

        // Data Copy
        memcpy(&outFrame[6], data, dataSize);  

        // Checksum of the current frame part.
        outFrame[6 + dataSize] = calculateFrameChecksum(outFrame, totalFrameSize);

        // End Markers
        outFrame[7 + dataSize] = END_BYTE_1;
        outFrame[8 + dataSize] = END_BYTE_2;
        return (totalFrameSize);
    }

    static Packet getFrameValue(const uint8_t* frame, const uint16_t frameSize) {
        const FrameErrorCodes validFrame = frameValidation(frame, frameSize);
        Packet newPacket;
        if (validFrame == FrameErrorCodes::OK) {
            newPacket.controlWord = frame[2];
            newPacket.commandWord = frame[3];
            size_t pSize = newPacket.dataLength = MASK(frame[4], frame[5]);
            memcpy(&newPacket.data, (frame+FRAME_START_WITHOUT_DATA_SIZE), pSize);
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
    const static inline FrameErrorCodes frameValidation(const uint8_t* frame, const uint16_t dataSize) {
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