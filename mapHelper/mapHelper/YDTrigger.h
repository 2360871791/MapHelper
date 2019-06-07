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
	bool onActionToJass(std::string& actions, Action* action,Action* parent, std::string& pre_actions, const std::string& trigger_name, bool nested);

	//ÿ����������ʱ
	bool onParamterToJass(std::string& actions, Parameter* action, std::string& pre_actions, const std::string& trigger_name, bool nested);

	//���������ɺ�����ʼʱ д��ֲ�����
	void onActionsToFuncBegin(std::string& funcCode, Trigger* trigger, Action* parent = NULL);
	
	//���������ɺ�������ʱ ����ֲ�����
	void onActionsToFuncEnd(std::string& funcCode, Trigger* trigger, Action* parent = NULL);


	//�жϴ����Ƿ����� true Ϊ����״̬
	bool hasDisableRegister(Trigger* trigger);

	bool isEnable();
private:
	void addLocalVar(std::string name,std::string type, std::string value = std::string());

	bool setHashLocal(std::string name, std::string type);

protected: 
	bool m_bEnable;
	bool m_isInYdweEnumUnit;
	bool m_hasAnyPlayer;
	bool m_isInMainProc;
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
	std::map<std::string,std::string> m_HashLocalTable;
};