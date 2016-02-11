#pragma once
#include <chrono>
#include <array>

namespace Utils
{
	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::time_point<Clock> TimePoint;

	/**
	 *@brief Simple timer, using T as type of std::chrono::duration<T>
	 */
	template<typename T, int C=1>
	class Timer
	{
	public:
		Timer()
		{
			// Initialize the timer
			m_LastUpdate = Clock::now();
			m_AvgDeltaCtr = 0;
			m_LastDelta.fill(std::chrono::duration<T>::zero());
			update();
		}

		/**
	     * @brief updates the timer and returns the average delta between this update and the last one as seconds
		 */
		std::chrono::duration<T> update()
		{
			TimePoint now = Clock::now();
			std::chrono::duration<T> delta = std::chrono::duration<T>(now - m_LastUpdate);
			m_LastUpdate = now;

			m_LastDelta[m_AvgDeltaCtr] = delta;

			// Go to next avg-frame
			m_AvgDeltaCtr = (m_AvgDeltaCtr + 1) % C;
			
			// Calculate average
			m_AvgDelta = std::chrono::duration<T>::zero();
			for(unsigned int i = 0; i < C; i++)
				m_AvgDelta += m_LastDelta[i];

			m_AvgDelta /= C;

			return m_AvgDelta;
		}

		/**
		 * @brief Returns the delta of the last update-call
		 */
		std::chrono::duration<T> getAvgDelta()
		{
			return m_AvgDelta;
		}

	private:

		/**
		 * @brief The time of the last update
		 */
		TimePoint m_LastUpdate;

		/**
		 * @brief The delta of the last update-call
		 */
		std::array<std::chrono::duration<T>,C> m_LastDelta;

		/**
		 * @brief Current delta-index
		 */
		unsigned int m_AvgDeltaCtr;

		/**
		 * @brief Averaged deltatimes from the last C update-calls 
		 */
		std::chrono::duration<T> m_AvgDelta;
	};
}