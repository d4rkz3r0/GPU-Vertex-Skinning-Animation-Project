#pragma once

class Time
{
public:
	Time();

	double TotalTime() const;
	void SetTotalTime(double totalTime);
	double ElapsedTime() const;
	void SetElapsedTime(double elapsedTime);

private:
	double mTotalTime;
	double mElapsedTime;
};