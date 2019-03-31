#pragma once

#include <vector>
#include <map>
#include <string>
#include <algorithm>

namespace Core
{
	template<typename T>
	class AutoListScene
	{
	public:
		AutoListScene(const std::string &scene = "main")
		{
			m_refMap[scene].push_back(static_cast<T *>(this));
		}
		virtual ~AutoListScene()
		{
			auto p = static_cast<T *>(this);
			for (auto &pr : m_refMap)
			{
				auto iter = std::find(std::begin(pr.second), std::end(pr.second), p);
				if (iter != std::end(pr.second))
				{
					pr.second.erase(iter);
					return;
				}
			}
		}
		void alsMoveRef(const std::string &scene)
		{
			auto p = static_cast<T *>(this);
			for (auto &pr : m_refMap)
			{
				auto iter = std::find(std::begin(pr.second), std::end(pr.second), p);
				if (iter != std::end(pr.second))
				{
					pr.second.erase(iter);
					m_refMap[scene].push_back(p);
					return;
				}
			}

		}
		static size_t alsSize(const std::string &scene = "main")
		{
			return m_refMap[scene].size();
		}
		static T *alsGet(int index, const std::string &scene = "main")
		{
			return m_refMap[scene][index];
		}
		static size_t alsCurrentSize()
		{
			return m_refMap[m_currentScene].size();
		}
		static T *alsCurrentGet(int index)
		{
			return m_refMap[m_currentScene][index];
		}
		static void alsSetScene(const std::string &scene)
		{
			m_currentScene = scene;
		}
		static const std::string &alsScene()
		{
			return m_currentScene;
		}
	private:
		static std::map<std::string, std::vector<T *>> m_refMap;
		static std::string m_currentScene;
	};

	template<typename T>
	std::map<std::string, std::vector<T *>> AutoListScene<T>::m_refMap;

	template<typename T>
	std::string AutoListScene<T>::m_currentScene = "main";
}
