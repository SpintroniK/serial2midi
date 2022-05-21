#pragma once

#include <array>
#include <optional>
#include <queue>

#include <cstdint>

namespace Midi
{

    struct Message
    {
        uint8_t command{};
        uint8_t channel{};
        uint8_t param1{};
        uint8_t param2{};
    };

    
    class MessageParser
    {
        
    public:
        MessageParser() = default;
        ~MessageParser() = default;

        void ReadByte(uint8_t byte)
        {

            const auto isStatusByte =  byte >> 7 != 0;

            if(!isStatusByte && bufferIndex == 0)
            {
                // Not a midi command, ignore next bytes.
                return;
            }

            switch(bufferIndex)
            {
                case 0:
                {
                    // Status byte
                    tempMessage.command = byte & 0xF0;
                    tempMessage.channel = byte & 0x0F;
                break;  
                }

                case 1:
                {
                    tempMessage.param1 = byte;
                    break;
                }

                case 2:
                {
                    tempMessage.param2 = byte;
                    messages.push(tempMessage);
                    break;
                }
            }

            bufferIndex = (bufferIndex + 1) % nbBytesPerMessage;

        }

        std::optional<Message> PopMessage()
        {
            if(!messages.empty())
            {
                const auto message = messages.back();
                messages.pop();
                return message;
            }

            return {};
        }

    private:

        static constexpr std::size_t nbBytesPerMessage = 3U;

        std::size_t bufferIndex{};
        std::array<uint8_t, nbBytesPerMessage> buffer{};

        Message tempMessage{};
        std::queue<Message> messages{};

    };

    class VirtualDevice
    {
        
    public:
        VirtualDevice() = default;
        ~VirtualDevice() = default;

    private:
    };
    
} // namespace Midi
