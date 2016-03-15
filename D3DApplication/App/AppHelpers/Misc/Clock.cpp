#include "Clock.h"
#include "Time.h"
#include "Exception.h"


Clock::Clock() : mStartTime(), mCurrentTime(), mLastTime(), mFrequency()
{
	mFrequency = GetFrequency();
	Reset();
}

const LARGE_INTEGER& Clock::StartTime() const
{
	return mStartTime;
}

const LARGE_INTEGER& Clock::CurrentTime() const
{
	return mCurrentTime;
}

const LARGE_INTEGER& Clock::LastTime() const
{
	return mLastTime;
}

void Clock::Reset()
{
	GetTime(mStartTime);
	mCurrentTime = mStartTime;
	mLastTime = mCurrentTime;
}

double Clock::GetFrequency() const
{
	LARGE_INTEGER frequency;

	if (QueryPerformanceFrequency(&frequency) == false)
	{
		throw Exception("QueryPerformanceFrequency() failed.");
	}

	return static_cast<double>(frequency.QuadPart);
}

void Clock::GetTime(LARGE_INTEGER& time) const
{
	QueryPerformanceCounter(&time);
}

void Clock::UpdateTime(Time& Time)
{
	GetTime(mCurrentTime);
	Time.SetTotalTime((mCurrentTime.QuadPart - mStartTime.QuadPart) / mFrequency);
	Time.SetElapsedTime((mCurrentTime.QuadPart - mLastTime.QuadPart) / mFrequency);

	mLastTime = mCurrentTime;
}