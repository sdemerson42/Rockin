#pragma once

#include "CoreEntity.h"
#include <vector>
#include <string>
#include <memory>
#include "angelscript.h"

namespace Core
{
	class EntityFactory
	{
	public:
		EntityFactory(std::vector<std::unique_ptr<CoreEntity>> *, asIScriptEngine *);
		void createEntity(const std::string &name, bool active = false, const std::string &layer = "default",
			float x = 0.0f, float y = 0.0f, const std::string &subsceneName = "main");
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
		std::vector<std::unique_ptr<CoreEntity>> *m_entity;
		asIScriptEngine *m_engine;
		std::vector<Blueprint> m_blueprint;

		bool readBlueprint(std::istream &ist, Blueprint &bp);
		bool readComponentData(std::istream &ist, ComponentData &cd);
		std::string nextToken(std::istream &ist);
		std::vector<std::string> m_compiledScripts;

	};
}
