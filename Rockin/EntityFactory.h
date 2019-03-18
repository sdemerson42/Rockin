#pragma once

#include "sde.h"
#include <vector>
#include <string>
#include <memory>
#include "angelscript.h"

namespace Core
{
	class EntityFactory
	{
	public:
		EntityFactory(std::vector<std::unique_ptr<sde::Entity>> *, asIScriptEngine *);
		void createEntity(const std::string &name, bool active = false);
		void loadBlueprintData(const std::string &fName);
	private:
		enum class BType { Int, Float, Bool, String };
		struct BPair
		{
			BType type;
			std::string value;
		};
		struct ComponentData
		{
			std::string component;
			std::vector<BPair> valuePair;
		};
		struct Blueprint
		{
			std::string name;
			std::vector<std::string> tag;
			std::vector<ComponentData> data;
		};
		std::vector<std::unique_ptr<sde::Entity>> *m_entity;
		asIScriptEngine *m_engine;
		std::vector<Blueprint> m_blueprint;

		bool readBlueprint(std::istream &ist, Blueprint &bp);
		bool readComponentData(std::istream &ist, ComponentData &cd);
		std::string nextToken(std::istream &ist);

	};
}
