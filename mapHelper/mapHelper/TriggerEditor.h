#pragma once

#include "stdafx.h"
#include "mapHelper.h"

struct TriggerType
{
	uint32_t flag;//0x0
	const char type[0x8c];//0x4
	const char value[0x110];//0x90 ������TriggerData�ļ����TriggerTypeDefaults��Ĭ��ֵ
};//size 0x1a0

struct TriggerConfigData
{
	char unknow[0x1c];
	uint32_t type_count; //0x1c
	TriggerType* array;//0x20
};

struct Parameter
{
	enum Type {
		invalid = -1,
		preset,
		variable,
		function,
		string
	};
	uint32_t table; //0x0
	uint32_t unknow2; //0x4
	uint32_t type;		  //0x8
	char unknow3[0x40];//0xc
	const char value[0x12c]; //0x4c
	struct Action* funcParam;//0x178 ��0ʱ��ʾ �˲�����������
	Parameter* arrayParam;//0x17c ��0ʱ ��ʾ�ò������������ ����ӵ���Ӳ���
};

struct Action
{
	enum Type {
		event,
		condition,
		action
	};

	struct VritualTable
	{
		uint32_t unknow1;
		uint32_t unknow2;
		uint32_t (__thiscall* getType)(void* pThis);
	};
	VritualTable* table; //0x0
	char unknow1[0x8];	 //0x4
	uint32_t child_count;	//0xc
	Action** child_actions;//0x10
	char unknow2[0xc];	 //0x14
	const char name[0x100]; //0x20
	uint32_t unknow3;//0x120;
	uint32_t unknow32;//0x124;
	uint32_t param_count; // 0x128
	Parameter** parameters;//0x12c
	char unknow4[0xC];//0x130
	uint32_t enable;//0x13c 
	char unknow5[0x14];//0x140
	uint32_t group_id;//0x154 �������������Ӷ���ʱΪ0 ������-1
};

struct Trigger
{
	char unknow1[0xc];
	uint32_t line_count; //0xc
	Action** actions;	//0x10
	char unknow2[0x4];//0x14
	uint32_t is_comment; //0x18
	uint32_t unknow3; //0x1c
	uint32_t is_enable;  //0x20 
	uint32_t is_disable_init; //0x24
	uint32_t is_custom_srcipt;//0x28
	uint32_t is_initialize;//0x2c Ӧ��Ĭ�϶���1
	uint32_t unknow7;//0x30
	uint32_t custom_jass_size;//0x34
	const char* custom_jass_script;//0x38
	char unknow4[0x10]; //0x3c
	const char name[0x100];//0x4c
	uint32_t unknow5;//0x14c
	struct Categoriy* parent;//0x150 //�ô������ڵ��ļ���
	const char text[0x1000];//0x154 �����ı�ע�� ����δ֪���� ������˸�size

};

struct Categoriy
{
	uint32_t categoriy_id;
	const char categoriy_name[0x10C];
	uint32_t has_change; // 0x110
	uint32_t unknow2; // 0x114
	uint32_t is_comment; // 0x118
	char unknow[0x14];// 0x11c
	uint32_t trigger_count;//0x130 ��ǰ�����еĴ���������
	Trigger** triggers;		//0x134
};

struct VariableData
{
	uint32_t unknow1;	//0x0 δ֪ ����1
	uint32_t is_array;	//0x4
	uint32_t array_size;//0x8
	uint32_t is_init;	//0xc
	const char type[0x1E];//0x10
	const char name[0x64];//0x2e
	const char value[0x12e];//0x92
};

struct Variable 
{
	char unknow1[0x8];		//0x0
	uint32_t globals_count;//0x8 ����gg_ ������ ����Ԥ�����ݵ�ȫ�ֱ���
	VariableData* array; //0xc
};

struct TriggerData
{
	uint32_t unknow1;		//0x0
	uint32_t trigger_count; // 0x4	���д���������
	char unknow2[0xC];		// 0x8
	uint32_t categoriy_count; //0x14 
	Categoriy** categories;	  //0x18
	uint32_t unknow3;		 //0x1c
	Variable* variables;    //0x20
	char unknow4[0x10]; // 0x24
	const char global_jass_comment[0x800];//0x34
	uint32_t unknow5; //0x834
	uint32_t globals_jass_size; //0x838
	const char* globals_jass_script;//0x83c
};


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

	
private: 
	std::string convert_gui_to_jass(Trigger* trigger, std::vector<std::string>& initializtions);
	std::string resolve_parameter(Parameter* parameter, const std::string& trigger_name, std::string& pre_actions, const std::string& type, bool add_call = false) const;
	std::string testt(const std::string& trigger_name, const std::string& parent_name, Parameter** parameters, std::string& pre_actions, bool add_call) const;


	void writeCategoriy(BinaryWriter& writer);
	void writeVariable(BinaryWriter& writer);
	void writeTrigger(BinaryWriter& writer);
	void writeTrigger(BinaryWriter& writer,Trigger* trigger);
	void writeAction(BinaryWriter& writer, Action* action);
	void writeParameter(BinaryWriter& writer, Parameter* param);
protected:
	TriggerConfigData* m_configData;
	TriggerData* m_editorData;
	uint32_t m_version;
	bool is_ydwe;

	const std::string seperator = "//===========================================================================\n";

	//��������Ĭ�ϵ�ֵ
	std::map<std::string, std::string> m_typeDefaultValues;
};