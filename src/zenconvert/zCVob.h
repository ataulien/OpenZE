#pragma once
#include "zTypes.h"
#include "zenParser.h"
#include "zenParserPropRead.h"

namespace ZenConvert
{
	class zCVob
	{
	public:
		/**
		* Reads this object from an internal zen
		*/
		static zCVobData readObjectData(ZenParser& parser)
		{
			zCVobData info;

			info.objectClass = "zCVob";

			ReadObjectProperties(parser,		info.properties, 
				Prop("Packing",					info.pack),
				Prop("PresetName",				info.presetName),
				Prop("BBoxMin",					info.bboxMin),
				Prop("BBoxMax",					info.bboxMax),
				Prop("RotationMatrixRow0",		info.rotationMatrixRows[0]),
				Prop("RotationMatrixRow1",		info.rotationMatrixRows[1]),
				Prop("RotationMatrixRow2",		info.rotationMatrixRows[2]),
				Prop("Position",				info.position),
				Prop("VobName",					info.vobName),
				Prop("VisualName",				info.visual),
				Prop("ShowVisual",				info.showVisual),
				Prop("VisualCamAlign",			info.visualCamAlign),
				Prop("VisualAniMode",			info.visualAniMode),
				Prop("VisualAniModeStrength",	info.visualAniModeStrength),
				Prop("VobFarClipScale",			info.vobFarClipScale),
				Prop("CollisionDetectionStatic",info.cdStatic),
				Prop("CollisionDetectionDyn",	info.cdDyn),
				Prop("StaticVob",				info.staticVob),
				Prop("DynamicShadow",			info.dynamicShadow),
				Prop("zBias",					info.zBias),
				Prop("IsAmbient",				info.isAmbient)); // TODO: References!

			return info;
		}

	private:
	};
}