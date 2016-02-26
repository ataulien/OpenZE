#pragma once
#include "zTypes.h"
#include "zenParser.h"
#include "zCVob.h"

namespace ZenConvert
{
	class oCWorld
	{
	public:

		static void readVobTree(ZenParser& parser, zCVobData& target)
		{

		}

		/**
		* Reads this object from an internal zen
		*/
		static oCWorldData readObjectData(ZenParser& parser)
		{
			oCWorldData info;
			info.objectClass = "oCWorld";

			return info;
		}

	private:
	};

}