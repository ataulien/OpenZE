#pragma once
#include "utils/logger.h"
#include "visuals/visual.h"

namespace Renderer
{
	class Visual;

	/**
	 * @brief Storage for a visual and it's hash.
	 *		  Note: This class will delete every visual registered here, if it gets deleted!
	 */
	class VisualStorage
	{
	public:
		VisualStorage(){}
		~VisualStorage()
		{
			// Free everything that has not been freed yet
			for(auto& v : m_VisualMap)
			{
				delete v.second;
			}
		}

		/** 
		 * @brief Adds a visuals having the given hash to the storage
		 */
		void addVisual(size_t hash, Visual* pVisual)
		{
			auto it = m_VisualMap.find(hash);

			// Report, if there was a hashcollision of we already have this object in the map
			if(it != m_VisualMap.end())
			{
				LogWarn() << "Found hash-collision/double add of visuals to the VisualStorage! Hash: " << hash;
				return;
			}

			m_VisualMap[hash] = pVisual;
		}

		/**
		 * @brief Removes a visual from the storage
		 */
		void removeVisual(size_t hash)
		{
			m_VisualMap.erase(hash);
		}

		/**
		 * @brief Returns the visual matching the given hash. Nullptr if none was found
		 */
		Visual* getVisual(size_t hash)
		{
			auto it = m_VisualMap.find(hash);

			// Report, if there was a hashcollision of we already have this object in the map
			if(it != m_VisualMap.end())
				return (*it).second;

			return nullptr;
		}
	private:
		/**
		 * @brief Visuals by their hashes
		 */
		std::unordered_map<size_t, Visual*> m_VisualMap;
	};
}