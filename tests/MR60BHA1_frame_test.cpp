#include <MR60BHA1_Driver.hpp>
#include <gtest/gtest.h>

TEST(AssembleFrameTest, AssemblesCorrectly) {
    MR60BHA1::MR60BHA1_Driver driver;

    MR60BHA1::ControlWord word_test = MR60BHA1::ControlWord::HEART_INFO;
    const uint8_t command = 0x02;
    const uint8_t data[] = {0x03, 0x04, 0x05};
    const uint16_t dataSize = sizeof(data) / sizeof(data[0]);

    uint8_t outFrame[20];   
    size_t outFrameMaxSize = sizeof(outFrame);

    size_t expectedSize = 9 + dataSize; // 12 
    size_t actualSize = driver.assembleFrame(word_test, command, data, dataSize, outFrame, outFrameMaxSize);

    EXPECT_EQ(actualSize, expectedSize);
    
    EXPECT_EQ(outFrame[0], static_cast<uint16_t>(MR60BHA1::FrameConstants::START) >> 8);
    EXPECT_EQ(outFrame[1], static_cast<uint16_t>(MR60BHA1::FrameConstants::START) & 0xFF);
    EXPECT_EQ(outFrame[2], static_cast<uint8_t>(MR60BHA1::ControlWord::HEART_INFO)); 
    EXPECT_EQ(outFrame[3], 0x02); // Command 
    EXPECT_EQ(outFrame[4], 0x0);  // Data Length
    EXPECT_EQ(outFrame[5], 0x3); 
    EXPECT_EQ(outFrame[6], data[0]); // Data
    EXPECT_EQ(outFrame[7], data[1]); 
    EXPECT_EQ(outFrame[8], data[2]);
    EXPECT_EQ(outFrame[9], 0x42); // Checksum
    EXPECT_EQ(outFrame[10], (static_cast<uint16_t>(MR60BHA1::FrameConstants::END) >> 8));
    EXPECT_EQ(outFrame[11], (static_cast<uint16_t>(MR60BHA1::FrameConstants::END) & 0xFF));
}

// Main function for the test program
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
