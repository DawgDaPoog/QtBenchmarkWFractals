// Fractal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cstdint>
#include <iostream>
#include <ctime>
#include <sstream>
#include "FractalCreator.h"
#include "Zoom.h"
#include "PrioritySetter.h"
#include "InfoGatherer.h"



int main()
{
	//initialising fractal creator
	int resolutionScale = 1;
	int const WIDTH = resolutionScale*800;
	int const HEIGHT = resolutionScale*600;

	FractalCreator fractalCreator(WIDTH,HEIGHT);
	
	int PossibleThreadAmountVariation[4] = { 1,2,4,8 };

	int ZoomWidth[4] = { WIDTH / 2,288,455,149 };
	int ZoomHeight[4] = { HEIGHT / 2,170,299,233 };
	int ZoomIntensity[4] = { 0,4,40,60 };

	//run
	InfoGatherer Gatherer = InfoGatherer();
	
	bool bTestInProgress = false;

	PrioritySetter PrSetter = PrioritySetter();

	Gatherer.GetInitialInformation();

	// Gatherer.WriteInitialnformation();

	clock_t begin;
	clock_t end;
	double elapsed_secs;

	//Starting the information gatherer ticker
	std::thread([&bTestInProgress, &Gatherer]() {
		while (true)
		{
			if (bTestInProgress)
			{
				// Wait 100 miliseconds
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				// Call our method
				Gatherer.TickInformationCheck();
			}
		}
	}).detach();

	for (int ZoomIndex = 0; ZoomIndex <= 3; ZoomIndex++)
	{
		//adding zoom
		fractalCreator.resetZoom();
		fractalCreator.addZoom(Zoom(ZoomWidth[0],ZoomHeight[0],4.0/WIDTH)); // Standart non-zoom version
		if (ZoomIndex != 0)
		{
			fractalCreator.addZoom(Zoom(ZoomWidth[ZoomIndex], ZoomHeight[ZoomIndex], 1.0 / ZoomIntensity[ZoomIndex]));
		}

		for (int PriorityCaseIndex = 0; PriorityCaseIndex <= 2; PriorityCaseIndex++)
		{
			PrSetter.SetPriority(PriorityCaseIndex);
			for (int AmountOfThreadsIndex = 0; AmountOfThreadsIndex <= 3; AmountOfThreadsIndex++)
			{
				if (PossibleThreadAmountVariation[AmountOfThreadsIndex] > Gatherer.GetNumberOfCores()) break;
				
				std::string ZoomUsed;
				std::stringstream ss;
				ss << ZoomWidth[ZoomIndex] << "x" << ZoomHeight[ZoomIndex] << " " << ZoomIntensity[ZoomIndex];
				ZoomUsed = ss.str();

				for (int TestRepeats = 0; TestRepeats < 5; TestRepeats++)
				{
					Gatherer.InitializeNewTest(PriorityCaseIndex, PossibleThreadAmountVariation[AmountOfThreadsIndex], ZoomUsed);
					//Calculating Iterations of Mandelbrot algorythm
					begin = clock();

					bTestInProgress = true;

					fractalCreator.run(PossibleThreadAmountVariation[AmountOfThreadsIndex]);

					end = clock();
					elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

					std::cout << "Executing Mandelbrot algorythm took " << elapsed_secs << " seconds." << std::endl;

					bTestInProgress = false;
					Gatherer.AddNewTestTimeTaken(elapsed_secs);
				}
			}
		}
	}

	
	Gatherer.WriteTestsToFile();
    return 0;
}

