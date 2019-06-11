#pragma once
#include "stdafx.h"
#include "EditorData.h"

//�����ڵ�

//ÿ��������Ϊһ�� root ���ڵ� ���ڵ�� action �� parent ΪNULL 
//ÿ������ �� �ڵ�Ϊ ����,���ڵ�  ���ڵ�Ϊ��һ��֧�� ��if then else  ��Ϊroot�ڵ�
//ÿ�������Ľڵ�Ϊ action ��ǰ�����ĺ������ö���,���ڵ�Ϊ���ڵĶ���

typedef std::shared_ptr<class ActionNode> ActionNodePtr;

typedef std::shared_ptr<class ParamNode> ParamNodePtr;

class ActionNode
{
public:

	//�ڵ�����
	enum class Type
	{
		trigger,
		action,
		parameter,
	};

	ActionNode();
	ActionNode(Trigger* root);
	ActionNode(Action* action, ActionNodePtr parent);

	//��ȡ�ڵ�Ķ���
	Action* getAction();

	//��ȡ�ڵ�����������
	Trigger* getOwner();

	//��ȡ������id �ַ�����ϣֵ
	uint32_t getNameId();

	//��ȡ���������Ӷ���id
	uint32_t getActionId();

	//�жϸýڵ��Ƿ��Ǹ��ڵ�
	bool isRootNode();

	//��ȡ���ڵ�
	ActionNodePtr getParentNode();

	//��ȡ���ڵ�
	ActionNodePtr getRootNode();

	//��ȡ֧��
	ActionNodePtr getBranchNode();

	size_t getChildCount();
	//��ȡ�Ӷ����ڵ�
	void getChildNodeList(std::vector<ActionNodePtr>& list);
	
	size_t getParamCount();
	void getParamNodeList(std::vector<ParamNodePtr>& list);


	Action::Type getActionType();

private:

protected:
	Action* m_action;//��ǰ����
	ActionNodePtr m_parent;//���ڵ�

	uint32_t m_nameId;//�������Ĺ�ϣֵid

	Trigger* m_trigger;//����������

	Parameter* m_parameter;//�������� ֻ�нڵ�����Ϊ����ʱ����Ч

	//������¼���������ʱ���ľֲ����� �Ա�����һ�㺯��������
	std::map<std::string, std::string>* mapPtr;

	Type m_type;

};



class ParamNode
{
public:
	ParamNode(Parameter* param, ActionNodePtr parent);

	ActionNodePtr getOwner();

private: 

protected: 
	Parameter* m_parameter;
	ActionNodePtr m_parent;
};

ParamNode::ParamNode(Parameter* param,ActionNodePtr parent)
{
	m_parameter = param;
	m_parent = parent;
}

ActionNodePtr ParamNode::getOwner()
{
	return m_parent;
}

