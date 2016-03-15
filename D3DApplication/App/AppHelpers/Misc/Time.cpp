#include "Time.h"

Time::Time() : mTotalTime(0.0), mElapsedTime(0.0) {	}

double Time::TotalTime() const
{
	return mTotalTime;
}

void Time::SetTotalTime(double totalTime)
{
	mTotalTime = totalTime;
}

double Time::ElapsedTime() const
{
	return mElapsedTime;
}

void Time::SetElapsedTime(double elapsedTime)
{
	mElapsedTime = elapsedTime;
}