#pragma once
#ifndef GAMETIMER_H
#define GAMETIMER_H
#include <windows.h>
class GameTimer
{
public:
	GameTimer();
	float GameTime() const; // Returns the total time since the game started
	float DeltaTime() const; // Returns the time elapsed since the last frame

	void Reset(); // Resets the timer
	void Start(); // Starts the timer
	void Stop(); // Stops the timer
	void Tick(); // Updates the timer for the current frame

private:
	double m_secondsPerCount; // Time per count
	double m_deltaTime; // Time elapsed since the last frame
	__int64 m_baseTime; // Time when the timer was started
	__int64 m_pausedTime; // Time when the timer was paused
	__int64 m_stopTime; // Time when the timer was stopped
	__int64 m_prevTime; // Time of the previous frame
	__int64 m_currTime; // Time of the current frame
	bool m_stopped; // Indicates if the timer is stopped
};

#endif // GAMETIMER_H