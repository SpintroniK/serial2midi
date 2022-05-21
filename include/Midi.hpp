#pragma once

#include <alsa/asoundlib.h>

#include <array>
#include <optional>
#include <queue>
#include <string_view>

#include <cstdint>

namespace Midi
{

    static constexpr std::size_t nbBytesPerMessage = 3U;
    using MidiBytes_t = std::array<uint8_t, nbBytesPerMessage>;

    struct Message
    {
        uint8_t command{};
        uint8_t channel{};
        uint8_t param1{};
        uint8_t param2{};

        MidiBytes_t ToBytes() const
        {
            return {static_cast<uint8_t>(command & 0xF0 | channel & 0x0F), param1, param2};
        }

        static Message FromBytes(const MidiBytes_t& bytes)
        {
            Message message{};   
            message.command = bytes[0] & 0xF0;
            message.channel = bytes[0] & 0x0F;
            message.param1 = bytes[1];
            message.param2 = bytes[2];

            return message;
        }
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
                    buffer[bufferIndex] = byte;
                break;  
                }

                case 1:
                {
                    buffer[bufferIndex] = byte;
                    break;
                }

                case 2:
                {
                    buffer[bufferIndex] = byte;
                    messages.push(Message::FromBytes(buffer));
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

        std::size_t bufferIndex{};
        MidiBytes_t buffer{};

        Message tempMessage{};
        std::queue<Message> messages{};

    };

    class VirtualDevice
    {
        
    public:
        VirtualDevice(std::string_view deviceName) : name{deviceName}
        {
            if(snd_seq_open(&seq, "default", SND_SEQ_OPEN_OUTPUT, 0))
            {
                // error
                return;
            }

            port = snd_seq_create_simple_port(seq, name.data(), 
                                              SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ, 
                                              SND_SEQ_PORT_TYPE_MIDI_GENERIC);

        }

        ~VirtualDevice()
        {
            snd_seq_close(seq);
        }

        void EmitMessage(const Message& message) const
        {
            snd_midi_event_t* midi;

            const auto messageBytes = message.ToBytes();

            if (snd_midi_event_new(messageBytes.size(), &midi))
            {
                return;
            }

            snd_seq_event_t ev;
            snd_seq_ev_clear(&ev);
            snd_seq_ev_set_source(&ev, port);
            snd_seq_ev_set_subs(&ev);
            snd_seq_ev_set_direct(&ev);
            snd_midi_event_encode(midi, messageBytes.data(), messageBytes.size(), &ev);
            snd_seq_event_output(seq, &ev);
            snd_seq_drain_output(seq);
            snd_midi_event_free(midi);
    
        }

    private:

        std::string_view name;
        snd_seq_t* seq;
        int port;
    };
    
} // namespace Midi
