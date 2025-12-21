#include <iostream>
#include "portaudio.h"

int main() {
    std::cout << "=== PortAudio Test ===" << std::endl;

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init failed: " << Pa_GetErrorText(err) << std::endl;
        return -1;
    }

    std::cout << "PortAudio initialized successfully!" << std::endl;

    Pa_Terminate();
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}
