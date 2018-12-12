#include "stdafx.h"
#include "FractalCreator.h"
#include "Mandelbrot.h"

#include <assert.h> 
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

FractalCreator::FractalCreator(const int  WIDTH, const int HEIGHT):WIDTH(WIDTH),HEIGHT(HEIGHT),_bitmap(WIDTH,HEIGHT), _zoomList(WIDTH, HEIGHT), _fractal(new int[WIDTH*HEIGHT]), _histogram(new int[Mandelbrot::MAX_ITERATIONS]{ 0 }),totalIterations(0)
{
}


 FractalCreator::~FractalCreator()
{
}

 
void FractalCreator::CalculateIterations(int AmountOfThreadsUsed)
{
	for (unsigned int a = 0; a < sizeof(_histogram); a++)
	{
		_histogram[a] = 0; // reseting the histogram;
	}
	auto IterThread = [this, AmountOfThreadsUsed](int thread)
	{
		for (int iHeight = (HEIGHT/ AmountOfThreadsUsed)*(thread - 1);iHeight < (HEIGHT/ AmountOfThreadsUsed)*(thread);iHeight++) {
			for (int iWidth = 0; iWidth < WIDTH;iWidth++)
			{
				std::pair<double, double> coords = _zoomList.ZoomIn(iWidth, iHeight);

				int iterations = Mandelbrot::getIterations(coords.first, coords.second);

				if (iterations != Mandelbrot::MAX_ITERATIONS) {
					_histogram[iterations]++;
				}
				_fractal[iHeight*WIDTH + iWidth] = iterations;
			}
		}
	};
	
	
	std::vector<std::thread> v;
	for (int i = 0; i < AmountOfThreadsUsed; i++)
	{
		v.push_back(std::thread(IterThread, i + 1));
	}

	join_all(v);


	totalIterations = 0;
	for (int i = 0; i < Mandelbrot::MAX_ITERATIONS;i++) {
		totalIterations += _histogram[i];
	}
}

void FractalCreator::addZoom(const Zoom & zoom)
{
	_zoomList.add(zoom); 
}

void FractalCreator::resetZoom()
{
	_zoomList.clear();
}

void FractalCreator::WriteBitmap(std::string name)
{
	_bitmap.write(name);
}

void FractalCreator::calculateRangeTotals()
{
	for (auto item : _rangeTotals)
	{
		item = 0;
	}
	int rangeIndex = 0;

	for (int i = 0;i < Mandelbrot::MAX_ITERATIONS;i++) {
		int pixels = _histogram[i];

		if (i >= _ranges[rangeIndex + 1]) {
			rangeIndex++;
		}
		_rangeTotals[rangeIndex] += pixels;
	}
}



int FractalCreator::getRange(int iterations) const
{
	int range = 0;

	for (int i = 1; i < (int)_ranges.size();i++) {
		range = i;

		if (_ranges[i] > iterations) break;
		
	}
	range--;

	assert(range > -1);
	assert(range < (int)_ranges.size());
	return range;
}


void FractalCreator::run(int AmountOfThreadsUsed)
{
	CalculateIterations(AmountOfThreadsUsed);
}


void FractalCreator::do_join(std::thread & t)
{
	t.join();
}

void FractalCreator::join_all(std::vector<std::thread> & v)
{
	for (int i = 0; i < v.size(); i++)
	{
		do_join(v.at(i));
	}
}