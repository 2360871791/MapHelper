#pragma once
#include "stdafx.h"

#include "json.hpp"
#include "word_handler.hpp"
#include "word_parser.hpp"


namespace word {

	using json::Json;

	struct ActionInfo
	{
		int type_id;

		std::string name;

		bool is_child = false;


		std::map<std::string, std::string> attrs;

		bool equal(const std::string& key, const std::string& value) {
			auto it = attrs.find(key);
			if (it == attrs.end()) {
				return false;
			}
			return it->second.compare(value) == 0;
		}

		std::string get_value(const std::string& key) {
			auto it = attrs.find(key);
			if (it == attrs.end()) {
				return std::string();
			}
			return it->second;
		}

		bool get_value(const std::string& key, std::string& result) {
			auto it = attrs.find(key);
			if (it == attrs.end()) {
				return false;
			}
			result = it->second;
			return true;
		}
	};


	typedef std::shared_ptr<Handler> HandlerPtr;
	typedef std::vector<ActionInfo> ActionInoList;
	typedef std::shared_ptr<ActionInoList> ActionInoListPtr;

	class ActionDef {

	public:

		enum TYPE {
			NONE,
			ACTION,
			VALUE,
			GROUP
		};

		ActionDef()
			:m_init(0),
			m_auto_param(0),
			m_is_group(0)
		{ }

		bool load(const Json& json) {
			std::string type_name;
			Json jtype = json["Type"];
			if (jtype.is_string()) {
				type_name = jtype.string_value();
			}
			else {
				std::cout << "ȱ������ ��ȡ��Ч\n";
				return false;
			}
			TYPE type = NONE;
			if (type_name == "Action") {
				m_type = ACTION;
				if (!load_action(json)) {
					std::cout << "��ȡ����ʧ��\n";
					return false;
				}
			} else if (type_name == "Value") {
				m_type = VALUE;
				if (!load_value(json)) {
					std::cout << "��ȡֵʧ��\n";
					return false;
				}
			} else if (type_name == "Group") {
				m_type = GROUP;
				m_is_group = true;
				if (!load_group(json)) {
					std::cout << "��ȡ������ʧ��\n";
					return false;
				}
			}
			else {
				std::cout << "δ֪���� ��ȡ����\n";
				return false;
			}
			m_init = true;

			return true;
		}

		bool load_group(const Json& json) {
			Json jgroup = json["Group"];
			Json jscript = json["Script"];

			if (!jgroup.is_array()) {
				std::cout << "Group����\n";
				return false;
			}
			if (!jscript.is_array()) {
				std::cout << "Script����\n";
				return false;
			}

			Json jbool = json["AutoParam"];
			if (jbool.is_bool()) {
				m_auto_param = jbool.bool_value();
			}

			m_group = std::make_shared<ActionInoList>();

			for (const auto& action : jgroup.array_items()) {
				Json jtype = action["Action"];
				int type_id = -1;

				if (jtype.is_string()) {
					type_id = 2;
				}
				else if (jtype = action["Condition"], jtype.is_string()) {
					type_id = 1;
				}
				else if (jtype = action["Event"], jtype.is_string()) {
					type_id = 0;
				}
				if (type_id < 0) {
					std::cout << "ȱ�ٶ���������\n";
					return false;
				}

				ActionInfo info;
				info.type_id = type_id;
				info.name = jtype.string_value();

				Json jchild = action["IsChild"];
				if (jchild.is_bool()) {
					info.is_child = jchild.bool_value();
				}

				for (const auto&[key, value] : action.object_items()) {
					if (value.is_string()) {
						info.attrs[key] = value.string_value();
					}
				}

				m_group->push_back(info);
			}

			std::string script;
			auto& array = jscript.array_items();
			if (array.empty()) {
				std::cout << "Script�ǿ����� ����ʧ��\n";
				return false;
			}
			for (const auto& line : array) {
				if (line.is_string()) {
					script += line.string_value() + "\n";
				}
			}
			if (script.empty()) {
				std::cout << "Script�ǲ����ַ������� ����ʧ��\n";
				return false;
			}
			script.pop_back();

			m_script = std::make_shared<Handler>();

			word::Parser parser(script.c_str());
			parser.parse(*m_script);

			//std::cout << "����" << m_script->lines.size() << "\n";
			//for (const auto& line : m_script->lines) {
			//	for (const auto& value : line.values) {
			//		if (value.type == ValueInfo::CODE) {
			//			std::cout << *value.code;
			//		} else if (value.call){
			//			std::cout << value.call->name;
			//			for (const auto& param : value.call->params) {
			//				std::cout << param.type << "  :  ";
			//				if (param.type == Param::TYPE::STRING) {
			//					std::cout << param.string;
			//				} else if (param.type == Param::UINT) {
			//					std::cout << param.uint;
			//				}
			//				std::cout << "  \n";
			//			}
			//		}
			//	}
			//	std::cout << "\n";
			//}

			return true;
		}

		bool load_action(const Json& json) {
			Json jscripts = json["Scripts"];

			if (!load_scripts(jscripts)) {
				std::cout << "��ȡScriptsʧ��\n";
				return false;
			}
			return true;
		}

		bool load_value(const Json& json) {
			Json jscripts = json["Scripts"];

			if (!load_scripts(jscripts)) {
				std::cout << "��ȡScriptsʧ��\n";
				return false;
			}

			Json jbool = json["AutoParam"];
			if (jbool.is_bool()) {
				m_auto_param = jbool.bool_value();
			}

			return true;
		}

		bool load_scripts(const Json& json) {
			if (!json.is_object()) {
				std::cout << "Scripts������Ч�Ķ���\n";
				return false;
			}

			for (const auto&[name, child] : json.object_items()) {
				if (!child.is_array()) {
					continue;
				}
				auto& array = child.array_items();
				if (array.empty()) {
					continue;
				}
				std::string code;
				for (const auto& line : array) {
					if (line.is_string()) {
						code += line.string_value() + "\n";
					}
				}
				if (code.empty()) {
					continue;
				}

				code.pop_back();
				
				HandlerPtr handler = std::make_shared<Handler>();
				Parser parser(code.c_str());
				
				if (!parser.parse(*handler)) {
					return false;
				}

				m_scripts[name] = handler;
			}
			return true;
		}

		bool is_auto_param() {
			return m_auto_param;
		}
		bool is_group() {
			return m_is_group;
		}

		TYPE get_type() {
			return m_type;
		}

		ActionInoListPtr get_group() {
			if (m_type != GROUP) {
				return nullptr;
			}
			return m_group;
		}
		size_t get_group_size() {
			if (m_type != GROUP) {
				return 0;
			}
			return m_group->size();
		}

		bool get_group_value(uint32_t child_id, const std::string& key, std::string& result) {
			auto group_ptr = get_group();
			if (!group_ptr) {
				return false;
			}
			if (child_id >= group_ptr->size()) {
				return false;
			}
			auto& info = (*group_ptr)[child_id];
			return info.get_value(key,result);
		}


		HandlerPtr get_group_script() {
			return m_script;
		}

		HandlerPtr get_script(const std::string& parent_name) {
			auto it = m_scripts.find(parent_name);
			if (it == m_scripts.end())
			{
				return nullptr;
			}
			return it->second;
		}

		
	private:
		//������
		ActionInoListPtr m_group;
		//�������ģ��
		HandlerPtr m_script;


		//�������Լ�ֵ ���ݲ�ͬ�� ���ڵ� ���ɲ�ͬ�Ľű�
		std::map<std::string, HandlerPtr> m_scripts;

		TYPE m_type;
		bool m_init;
		bool m_auto_param;
		bool m_is_group;

	};
	typedef std::shared_ptr<ActionDef> ActionDefPtr;
	typedef std::map<std::string, ActionDefPtr> GroupDef;

	class ActionGroup {

	public:
		
		ActionGroup()
		{ }

		bool load(const fs::path& path)
		{
			std::ifstream file(path, std::ios::binary);
			if (!file.is_open()) {
				std::cout << path << " �ļ�������\n";
				return false;
			}
			std::stringstream stream;
			std::string text;
			std::string error;

			stream << file.rdbuf();

			text = stream.str();

			file.close();

			return load(text);
		}

		bool load(const std::string& text) {
			std::string error;
			Json json = Json::parse(text, error);

			if (!error.empty()) {
				std::cout << "json ��ȡ����\n" << error;
				return false;
			}
			return load(json);
		}

		bool load(const Json& json) {
			auto defs = json.object_items();
			if (defs.empty()) {
				std::cout << "json�ǿյ�\n";
				return 0;
			}

			for (const auto&[def_name, child] : defs) {
				if (!child.is_object()) {
					continue;
				}

				ActionDefPtr action_def = std::make_shared<ActionDef>();
				if (!action_def->load(child)) {
					std::cout << "��ȡ" << def_name << "ʧ��\n";
					continue;
				}
				m_group_def.emplace(def_name, action_def);
			}
			std::cout << "��ȡ�ɹ�\n";
			return true;
		}

		GroupDef& get_group_def() {
			return m_group_def;
		}


		ActionDefPtr get_action_def(const std::string& name) {
			auto it = m_group_def.find(name);
			if (it == m_group_def.end()) {
				return nullptr;
			}
			return it->second;
		}

		//��ȡָ�������� ָ������id�� ָ��key��ֵ 
		//�� ("YDWETimerStartMultiple",0,"handle") ���� "ydl_timer", child_idΪ 1�򷵻�"GetExpiredTimer()"
		std::string get_group_value(const std::string& name, uint32_t child_id, const std::string& key) {
			std::string result;

			ActionDefPtr action_def = get_action_def(name);
			if (!action_def) {
				return result;
			}
			action_def->get_group_value(child_id, key, result);
			return result;
		}

	private:
		GroupDef m_group_def;
	};
}