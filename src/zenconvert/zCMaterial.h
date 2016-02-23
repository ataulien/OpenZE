#pragma once
#include "zTypes.h"
#include "parser.h"

namespace ZenConvert
{
	class zCMaterial
	{
	public:
		/**
		 * Reads a material from an internal zen
		 */
		void readObjectData(Parser& parser);

		/**
		* @brief returns sorted information about this material
		*/
		const MaterialInfo& getMaterialInfo(){return m_MaterialInfo;}

	private:
		/**
		 * @brief sorted information about this material
		 */
		MaterialInfo m_MaterialInfo;
	};
}