#include "include/Midi.hpp"
#include "include/Serial.hpp"

#include <array>
#include <iostream>
#include <ranges>

int main() 
{

    Serial serial{"/dev/ttyACM0", 115'200};

    const auto nbBytes = int{10 * 3};

    Midi::MessageParser parser;

    for(auto _ : std::views::iota(0, nbBytes))
    {
        const auto byte = serial.ReadByte();
        parser.ReadByte(byte);

        const auto message = parser.PopMessage();

        if(message)
        {
            std::cout << "Command: " << std::hex << +message->command << ", "
                      << "Channel: " << std::hex << +message->channel << ", "
                      << "Param 1: " << std::hex << +message->param1 << ", "
                      << "Param 2: " << std::hex << +message->param2 << std::endl;
        }
    }
    return 0; // success
};