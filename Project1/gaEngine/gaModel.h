#pragma once
#include <string>

namespace GameEngine {
	enum {
		PRELOAD = 0
	};

	class gaModel
	{
		uint32_t m_modelClass=0;

	protected:
		std::string m_name;
		uint32_t m_modelID = 0;

	public:
		static uint32_t m_modelClasses;
		gaModel(const std::string& name, uint32_t myclass);
		gaModel(uint32_t myclass);
		bool named(const std::string& name);
		const std::string& name(void) { return m_name; };
		uint32_t id(void) { return m_modelID; };
		uint32_t modelClass(void) { return m_modelClass; };
	};
}
