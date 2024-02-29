#include <gtest/gtest.h>
#include "MR60BHA1_Driver.hpp"

constexpr uint8_t FULL_BYTE = 0xFF;
constexpr uint8_t OFFSET_OF_FRAME_END = 3;

// Test the calculateChecksum function
TEST(CalculateChecksumTest, CorrectChecksum) {
    // Example frame with known checksum
    uint16_t tets = (static_cast<uint16_t>(MR60BHA1::FrameConstants::START) >> 8);
    const uint8_t frame1[] = {
        (static_cast<uint16_t>(MR60BHA1::FrameConstants::START) >> 8),
        (static_cast<uint16_t>(MR60BHA1::FrameConstants::START) & FULL_BYTE),
        0x02, 0x03, 0x04, 0xA, 0xAB, 0x6A,
        (static_cast<uint16_t>(MR60BHA1::FrameConstants::END) >> 8),
        (static_cast<uint16_t>(MR60BHA1::FrameConstants::END) & FULL_BYTE),
    };

    size_t frameSize1 = sizeof(frame1) / sizeof(frame1[0]);
    uint8_t expectedChecksum1 = frame1[frameSize1-3]; // Should be 0x6A
    EXPECT_EQ(MR60BHA1::MR60BHA1_Driver::calculateFrameChecksum(frame1, frameSize1), expectedChecksum1);

    // Add more tests with different frames and expected checksums to thoroughly test your function
    const uint8_t frame2[] = {0xFF, 0x00, 0x01, 0x02, 
        (static_cast<uint16_t>(MR60BHA1::FrameConstants::END) >> 8),
        (static_cast<uint16_t>(MR60BHA1::FrameConstants::END) & FULL_BYTE)};
    size_t frameSize2 = sizeof(frame2) / sizeof(frame2[0]);
    uint8_t expectedChecksum2 = (0xFF + 0x00 + 0x01) % (FULL_BYTE + 1); // Adjust based on your logic
    EXPECT_EQ(MR60BHA1::MR60BHA1_Driver::calculateFrameChecksum(frame2, frameSize2), expectedChecksum2);
}

/* Boundary Check */
TEST(CalculateChecksumTest, HandlesEmptyFrame) {
    const uint8_t emptyFrame[] = {}; // Empty arr
    size_t emptyFrameSize = sizeof(emptyFrame) / sizeof(emptyFrame[0]);
    uint8_t expectedChecksum = 0;
    EXPECT_EQ(MR60BHA1::MR60BHA1_Driver::calculateFrameChecksum(emptyFrame, emptyFrameSize), expectedChecksum);
}

TEST(CalculateChecksumTest, HandlesMinimal) {
    const uint8_t emptyFrame[] = {
        0x5, 0x1,
        (static_cast<uint16_t>(MR60BHA1::FrameConstants::END) >> 8),
        (static_cast<uint16_t>(MR60BHA1::FrameConstants::END) & FULL_BYTE)
    }; // Empty arr
    size_t emptyFrameSize = sizeof(emptyFrame) / sizeof(emptyFrame[0]);
    uint8_t expectedChecksum = 0;
    EXPECT_EQ(MR60BHA1::MR60BHA1_Driver::calculateFrameChecksum(emptyFrame, emptyFrameSize), expectedChecksum);
}

// TEST(CalculateChecksumTest, CorrectChecksumPart) {
//     // Test with a simple frame
//     const uint8_t frame1[] = {0x01, 0x02, 0x03, 0x04};
//     size_t frameSize1 = sizeof(frame1) / sizeof(frame1[0]);
//     // Calculate expected checksum according to the updated logic
//     uint8_t expectedChecksum1 = ((0x01 + 0x02 + 0x03 + 0x04) % FULL_BYTE) - 1;
//     EXPECT_EQ(MR60BHA1::MR60BHA1_Driver::calculateChecksum(frame1, frameSize1), expectedChecksum1);

//     // Test with a frame that includes higher values
//     const uint8_t frame2[] = {0xFF, 0xFF, 0xFF, 0xFF};
//     size_t frameSize2 = sizeof(frame2) / sizeof(frame2[0]);
//     uint8_t expectedChecksum2 = ((0xFF + 0xFF + 0xFF + 0xFF) % FULL_BYTE) - 1;
//     EXPECT_EQ(MR60BHA1::MR60BHA1_Driver::calculateChecksum(frame2, frameSize2), expectedChecksum2);

//     // Ensure the checksum calculation handles the wrap-around correctly
//     const uint8_t frame3[] = {0x00, 0x01, 0xFF};
//     size_t frameSize3 = sizeof(frame3) / sizeof(frame3[0]);
//     uint8_t expectedChecksum3 = ((0x00 + 0x01 + 0xFF) % FULL_BYTE) - 1;
//     EXPECT_EQ(MR60BHA1::MR60BHA1_Driver::calculateChecksum(frame3, frameSize3), expectedChecksum3);
// }

// TEST(CalculateChecksumTest, HandlesEmptyPart) {
//     const uint8_t emptyFrame[] = {};
//     size_t emptyFrameSize = sizeof(emptyFrame) / sizeof(emptyFrame[0]);
//     // Depending on your definition of FULL_BYTE, this might need adjustment.
//     // The expected checksum for an empty frame based on the given function logic
//     uint8_t expectedChecksum = static_cast<uint8_t>(-1); // This effectively becomes 0xFF for uint8_t
//     EXPECT_EQ(MR60BHA1::MR60BHA1_Driver::calculateChecksum(emptyFrame, emptyFrameSize), expectedChecksum);
// }

// Main function for the test program
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}