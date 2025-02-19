#include "Engine.h"

Engine::Engine()
{
	init();
}

Engine::~Engine()
{
	std::cout << "!!! *********************** !!!" << std::endl;
	std::cout << "!!! Engine -> Shutting down !!!" << std::endl;
	std::cout << "!!! *********************** !!!" << std::endl;
}

int Engine::init()
{
	//std::cout << "Engine -> init() called()" << std::endl;

	playbackDevice.init();

	// start playback device which will run callback function and wait for any audio data to be passed through
	if (ma_device_start(&playbackDevice.device) != MA_SUCCESS)
	{
		std::cerr << "Engine -> Failed to start playback device." << std::endl;
		ma_device_uninit(&playbackDevice.device);
		// this is already creating a red flag. all decoders should be uninitialised at once
		// making use of a resource manager or something. look into this later.
		//ma_decoder_uninit(&bigWave.decoder); 
		return -4;
	}
	else
		std::cout << "Engine -> Playback device started" << std::endl;

	eventManager.init();
}

void Engine::run()
{
	//std::cout << "Engine -> run() called()" << std::endl;
	while(true)
	{
		// calculating delta time
		auto now = std::chrono::steady_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
		lastUpdate = now;

		eventManager.update(deltaTime);
	}
}
