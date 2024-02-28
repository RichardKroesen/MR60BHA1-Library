// #ifndef MR60BHA1_FUNCTIONS_HPP
// #define MR60BHA1_FUNCTIONS_HPP

// #include <vector>
// #include <stdint.h>
// #include "MR60BHA1_Defines.hpp"

// const uint8_t calculateChecksum(const std::vector<uint8_t>& frame) {
//     auto tFrame = frame;
//     uint32_t sum = 0;
//     for (size_t i = 0; i < frame.size() - 3; i++) {
//         sum += frame[i];
//     }
//     return (sum) % (0xFF+1); 
// }

// const uint8_t calculateFrameChecksum(const std::vector<uint8_t>& frame) {
//     auto tFrame = frame;
//     uint32_t sum = 0;
//     for (size_t i = 0; i < frame.size(); i++) {
//         sum += frame[i];
//     }
//     return sum % (0xFF)-1; 
// }

// // Frame assembly function
// std::vector<uint8_t> assembleFrame(ControlWord controlWord, uint8_t command, const std::vector<uint8_t>& data) {
//     std::vector<uint8_t> frame;
//     // Start marker
//     frame.push_back(static_cast<uint16_t>(FrameConstants::START) >> 8);
//     frame.push_back(static_cast<uint16_t>(FrameConstants::START) & 0xFF);
//     // Control word and command
//     frame.push_back(static_cast<uint8_t>(controlWord));
//     frame.push_back(command);
//     // Data length
//     uint16_t dataLength = data.size();
//     frame.push_back(dataLength >> 8);
//     frame.push_back(dataLength & 0xFF);
//     // Data
//     frame.insert(frame.end(), data.begin(), data.end());
//     // Checksum
//     uint8_t checksum = calculateFrameChecksum(frame); // Calculate based on the content before checksum
//     frame.push_back(checksum);

//     frame.push_back(static_cast<uint16_t>(FrameConstants::END) >> 8);
//     frame.push_back(static_cast<uint16_t>(FrameConstants::END) & 0xFF);
//     return frame;
// }

// // Frame parser function
// bool parseFrame(const std::vector<uint8_t>& frame) {
//     if (frame.size() < 8) { // Minimal frame length check
//         std::cout << "Frame too short." << std::endl;
//         return false;
//     }

//     uint16_t startMarker = (frame[0] << 8) | frame[1];
//     uint16_t endMarker = (frame[frame.size() - 2] << 8) | frame[frame.size() - 1];

//     if (startMarker != static_cast<uint16_t>(FrameConstants::START) || 
//         endMarker != static_cast<uint16_t>(FrameConstants::END)) {
//         std::cout << "Invalid start or end marker." << std::endl;
//         return false;
//     }

//     uint8_t controlWord = frame[2];
//     uint8_t command = frame[3];
//     uint16_t dataLength = (frame[4] << 8) | frame[5];

//     // Correcting the frame length check to account for all components
//     if (frame.size() != (6 + dataLength + 3)) { // 6 bytes (start marker, control, command, data length) + dataLength + 3 bytes (checksum, end marker)
//         std::cout << "Invalid frame length." << std::endl;
//         return false;
//     }

//     const uint8_t checksum = frame[frame.size() - 3];
//     auto temp = calculateChecksum(frame);
//     if (temp != checksum) {
//         std::cout << "Checksum mismatch." << std::endl;
//         return false;
//     }

//     // Successfully parsed frame
//     std::cout << "Frame parsed successfully. Control Word: " << static_cast<int>(controlWord) 
//               << ", Command: " << static_cast<int>(command) 
//               << ", Data Length: " << dataLength << std::endl;

//     return true;
// }

// std::vector<uint8_t> assembleFrame(ControlWord controlWord, uint8_t command, uint8_t data) {
//     std::vector<uint8_t> frame;
//     // Start marker
//     frame.push_back(static_cast<uint16_t>(FrameConstants::START) >> 8);
//     frame.push_back(static_cast<uint16_t>(FrameConstants::START) & 0xFF);
    
//     // Control word and command
//     frame.push_back(static_cast<uint8_t>(controlWord));
//     frame.push_back(command);

//     // Data length
//     frame.push_back(0x00); // First length Byte.
//     frame.push_back(0x01); // Second lenght Byte.
//     // Data
//     frame.push_back(data);
//     // Checksum
//     uint8_t checksum = calculateFrameChecksum(frame); // Calculate based on the content before checksum
//     frame.push_back(checksum);

//     frame.push_back(static_cast<uint16_t>(FrameConstants::END) >> 8);
//     frame.push_back(static_cast<uint16_t>(FrameConstants::END) & 0xFF);
//     return frame;
// }

// std::vector<uint8_t> hexStringToBytes(std::stringstream& hexStream) {
//     std::vector<uint8_t> bytes;
//     char c;
//     while (hexStream.get(c)) { // Directly check stream state
//         if (!std::isxdigit(c)) {
//             throw std::invalid_argument("Invalid hex character");
//         }
//         uint8_t byte = (c >= '0' && c <= '9') ? (c - '0') : ((c & ~32) - 'A' + 10);
//         if (hexStream.get(c)) { // Attempt to read the second hex digit
//             if (!std::isxdigit(c)) {
//                 throw std::invalid_argument("Invalid hex character");
//             }
//             byte <<= 4;
//             byte |= (c >= '0' && c <= '9') ? (c - '0') : ((c & ~32) - 'A' + 10);
//         } else {
//             throw std::invalid_argument("Incomplete hex pair");
//         }
//         bytes.push_back(byte);
//     }
//     return bytes;
// }

// #endif // MR60BHA1_FUNCTIONS_HPP
