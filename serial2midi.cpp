#include "include/Serial.hpp"

#include <array>
#include <iostream>

struct MidiMessage
{
    uint8_t command{};
    uint8_t channel{};
    uint8_t param1{};
    uint8_t param2{};
};

int main() 
{

    Serial s{"/dev/ttyACM0", 115'200};

    const auto nbMessages = std::size_t{20U};

    for(std::size_t i = 0; i < nbMessages; ++i)
    {

        MidiMessage m{};

        for(std::size_t index = 0U; index < 3U; ++index)
        {
            const auto byte = s.ReadByte();
            const auto isStatusByte =  byte >> 7 != 0;

            if(!isStatusByte && index == 0)
            {
                // Not a midi command, ignore next bytes.
                break;
            }

            switch(index)
            {
                case 0:
                {
                    // Status byte
                    m.command = byte & 0xF0;
                    m.channel = byte & 0x0F;
                break;  
                }

                case 1:
                {
                    m.param1 = byte;
                    break;
                }

                case 2:
                {
                    m.param2 = byte;
                    break;
                }
            }

            if(index == 2)
            {
                std::cout << "Command: " << std::hex << +m.command << ", "
                      << "Channel: " << std::hex << +m.channel << ", "
                      << "Param 1: " << std::hex << +m.param1 << ", "
                      << "Param 2: " << std::hex << +m.param2 << std::endl;
            }
        }

    }
    return 0; // success
};