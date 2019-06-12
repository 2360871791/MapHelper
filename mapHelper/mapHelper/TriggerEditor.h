#pragma once

#include "stdafx.h"
#include "mapHelper.h"
#include "EditorData.h"
#include "ActionNode.h"

class TriggerEditor
{
public:
	TriggerEditor();
	~TriggerEditor();

	static TriggerEditor* getInstance();

	void loadTriggers(TriggerData* data);
	void loadTriggerConfig(TriggerConfigData* data);

	void saveTriggers(const char* path); //����wtg
	void saveScriptTriggers(const char* path);//���� wct
	void saveSctipt(const char* path); //����j

	

	std::string convert_gui_to_jass(Trigger* trigger, std::vector<std::string>& initializtions);
	std::string convert_action_to_jass(ActionNodePtr node, std::string& pre_actions, bool nested);

	std::string resolve_parameter(Parameter* parameter, ActionNodePtr node, std::string& pre_actions, bool add_call = false) const;
	std::string testt(ActionNodePtr node, std::string& pre_actions, bool add_call) const;
	
	std::string get_base_type(const std::string& type) const;
	std::string generate_function_name(std::shared_ptr<std::string> trigger_name) const;

private:
	void writeCategoriy(BinaryWriter& writer);
	void writeVariable(BinaryWriter& writer);
	void writeTrigger(BinaryWriter& writer);
	void writeTrigger(BinaryWriter& writer,Trigger* trigger);
	void writeAction(BinaryWriter& writer, Action* action);
	void writeParameter(BinaryWriter& writer, Parameter* param);

protected:
	TriggerConfigData* m_configData;
	TriggerData* m_editorData;

	class YDTrigger* m_ydweTrigger;

	uint32_t m_version;

	const std::string seperator = "//===========================================================================\n";

	//��������Ĭ�ϵ�ֵ
	std::unordered_map<std::string, TriggerType*> m_typesTable;

	std::unordered_map<std::string, bool> m_initFuncTable;
public:
	std::string spaces[200];
	int space_stack;
};