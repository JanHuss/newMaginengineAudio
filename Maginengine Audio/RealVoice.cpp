#include "RealVoice.h"

void RealVoice::assignDataToBuffer(std::vector<float>& audioData, bool loop)
{
    //std::clog << "Voice -> audio data size in voice: " << audioData.size() << std::endl;
    buffer = audioData;
    isLooping = loop;
    playHead = 0;
    setIsActive(true);
    //if (!buffer.empty())
    //    std::clog << "RealVoice -> Buffer not empty. Audio data assigned to Buffer" << std::endl;
    std::clog << "RealVoice -> Buffer size: " << buffer.size() << std::endl;
}

void RealVoice::clearBuffer()
{
    buffer.clear();
    playHead = 0;
    setIsActive(false);
    std::clog << "RealVoice -> Buffer cleared" << std::endl;
    std::clog << "RealVoice -> buffer size: " << buffer.size() << std::endl;
}

void RealVoice::processAudio(float* outputBuffer, ma_uint32 frameCount)
{
    //std::clog << "Voice -> \"processAudio()\" being called" << std::endl;
    if (!getIsActive() || buffer.empty())
    {
        memset(outputBuffer, 0, frameCount * channels * sizeof(float));
        return;
    }

    // number of frames to hit the expected max/min fade
    const ma_uint32 fadeDuration = 256; 

    ma_uint32 i = 0;
    for (; i < frameCount; ++i)
    {
        float sampleLeft = 0.0f;
        float sampleRight = 0.0f;
        
        if (playHead < buffer.size())
        {
            if (channels == 1)
            {
                float sample = buffer[playHead++];
                sampleLeft = sample * (1.0f - pan);
                sampleRight = sample * pan;
                //std::clog << "RealVoice -> 1 Channel" << std::endl;
            }
            else if (channels == 2)
            {
                sampleLeft = buffer[playHead++];
                sampleRight = buffer[playHead++];
                //std::clog << "RealVoice -> 2 Channels" << std::endl;
            }
        }
        else
        {
            // Loop
            if (isLooping)
                playHead = 0;
            // Stop
            else
            {
                setIsActive(false);
                hasFadedIn = false;
                std::cout << "set Is active is false" << std::endl;
                break;
            }
        }

        // doesn't need to fade in again if is looping
        if (playHead > fadeDuration)
            hasFadedIn = true;

        // Calculate the fade in/out
        float fadeFactor = 1.0f;
        if (!hasFadedIn && playHead < fadeDuration)
            fadeFactor = (1.0f - cosf(3.14159265359 * playHead / fadeDuration)) * 0.5f;
        else if (!isLooping && buffer.size() - playHead < fadeDuration)
            fadeFactor = (1.0f - cosf(3.14159265359 * (buffer.size() - playHead) / fadeDuration)) * 0.5f;

        // pass fade, left and right samples to output buffer
        outputBuffer[i * 2] += sampleLeft * fadeFactor;
        outputBuffer[i * 2 + 1] += sampleRight * fadeFactor;
    }
}

void RealVoice::setIsActive(bool iActive)
{
    isActive = iActive;
}

bool RealVoice::getIsActive()
{
    return isActive;
}

void RealVoice::setPlayHead(size_t plHead)
{
    playHead = plHead;
}

size_t RealVoice::getPlayHead()
{
    return playHead;
}

std::vector<float> RealVoice::getBuffer()
{
    return buffer;
}
