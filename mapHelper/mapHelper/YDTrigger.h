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

	//�жϴ����Ƿ����� true Ϊ����״̬
	bool hasDisableRegister(Trigger* trigger);

	bool isEnable();
private:


protected: 
	bool m_bEnable;
	bool m_hasAnyPlayer;
	
	std::map<Trigger*, bool> m_triggerHasDisable;
};