#pragma once
#include <windows.h>

class Time;

class Clock
{
public:
	Clock();
	const LARGE_INTEGER& StartTime() const;
	const LARGE_INTEGER& CurrentTime() const;
	const LARGE_INTEGER& LastTime() const;

	void Reset();
	double GetFrequency() const;
	void GetTime(LARGE_INTEGER& time) const;
	void UpdateTime(Time& Time);

private:
	Clock(const Clock& rhs);
	Clock& operator=(const Clock& rhs) = delete;

	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mCurrentTime;
	LARGE_INTEGER mLastTime;
	double mFrequency;
};