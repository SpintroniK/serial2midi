#include "include/Serial.hpp"

int main() 
{

    Serial s{"/dev/ttyACM0", 115'200};

    for(std::size_t i = 0; i < 200; ++i)
    {
        s.Read();
    }

    return 0; // success
};