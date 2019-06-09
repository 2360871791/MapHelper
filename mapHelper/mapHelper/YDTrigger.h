#pragma once

#include "stdafx.h"
#include "TriggerEditor.h"

//ר�Ÿ�������ydwe�Ĺ���

class YDTrigger
{
public:
	YDTrigger();
	~YDTrigger();
	static YDTrigger* getInstance();

	//����ͷ�ļ�
	void onGlobals(BinaryWriter& writer);
	void onEndGlobals(BinaryWriter& writer);

	//ʵ��������� ����������
	bool onRegisterEvent(std::string& events, Trigger* trigger, Action* action, std::string& name);
	void onRegisterEvent2(std::string& events, Trigger* trigger, Action* action, std::string& name);

	//ÿ����������ʱ
	bool onActionToJass(std::string& actions, Action* action, ActionNode* node, std::string& pre_actions, const std::string& trigger_name, bool nested);

	//ÿ����������ʱ
	bool onParamterToJass(std::string& actions, Parameter* parameter, ActionNode* node, std::string& pre_actions, const std::string& trigger_name, bool nested);

	//���������ɺ�����ʼʱ д��ֲ�����
	void onActionsToFuncBegin(std::string& funcCode, Trigger* trigger, ActionNode* node = NULL);
	
	//���������ɺ�������ʱ ����ֲ�����
	void onActionsToFuncEnd(std::string& funcCode, Trigger* trigger, ActionNode* node = NULL);


	//�жϴ����Ƿ����� true Ϊ����״̬
	bool hasDisableRegister(Trigger* trigger);

	bool isEnable();
private:

	void addLocalVar(std::string name,std::string type, std::string value = std::string());

	ActionNode getRootNode(ActionNode* node);

	std::string setLocal(ActionNode* node, const std::string& name, const std::string& type, const std::string& value, bool add = false);
	std::string getLocal(ActionNode* node, const std::string& name, const std::string& type);

	std::string setLocalArray(ActionNode* node, const  std::string& name, const std::string& type, const std::string& index, const std::string& value);
	std::string getLocalArray(ActionNode* node, const std::string& name, const std::string& type, const std::string& index);

	bool seachHashLocal(Parameter** parameters, uint32_t count, std::map<std::string, std::string>* mapPtr = NULL);
protected: 
	bool m_bEnable;
	bool m_isInYdweEnumUnit;
	bool m_hasAnyPlayer;
	bool m_isInMainProc;

	bool m_isFuncBegin;

	bool m_hasYdweLocal;

	int m_funcStack;
	std::map<Trigger*, bool> m_triggerHasDisable;

	struct LocalVar
	{
		std::string name;
		std::string type;
		std::string value;
	};
	//�ֲ������� ����,����,Ĭ��ֵ
	std::vector<LocalVar> m_localTable;
	std::map<std::string, bool> m_localMap;
	


	//��ϣ�ֲ������� ����,����
	std::map<std::string, std::string> m_HashLocalTable;



};