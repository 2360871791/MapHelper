#include "stdafx.h"
#include "EditorData.h"
#include "ActionNode.h"

#include "script_handler.hpp"
#include "script_parser.hpp"
#include "script_group.hpp"

namespace script {
	typedef std::shared_ptr<std::map<std::string, std::string>> FuncTablePtr;

	struct ScriptInfo {
		//Script�ű����ڴ���Ϣ
		HandlerPtr script;

		//��������ģ�� ����
		ActionDefPtr action_def;

		//�����ǰ���� �� ������ �� group �Ƕ������е���Ϣ ����� ��������ֵ�Ļ� group�����ڴ��εĶ�����
		ActionInoListPtr group;

		//�����ǰ���� �� ������ ����0 ������ ��������ֵ ʱ group_id�� ���ڶ������е���id 
		//���綯�������촥�������¼����� 0 ������ 1 ������2
		uint32_t group_id;

		//���������� �Ƕ��� ���� ֵʱ ���ڵ��Զ����ζ����������
		std::string parent_name;

		//��ǰ����ģ��ĺ������ı� ʵ��func_name(1) ʱ��¼����
		FuncTablePtr func_name_table;

		//��ǰ�������ڵĽڵ�
		ActionNodePtr node;

		//�����ֵ ��ô�ͻ��и�������ָ��
		Parameter* parameter = nullptr;

		//����������ű���ַ
		std::string* pre_actions;
		
	};
	
	class Converter {
	public:
		Converter();

		bool init();
		
		bool find_script(ActionNodePtr node, const std::string& name, ScriptInfo& script_info);
		
		bool execute(ScriptInfo& info, Value& result);

		bool call_func(CallPtr call, ScriptInfo& info, Value& result);

		bool call_bind(BindPtr bind, ScriptInfo& info, Value& result);
		
	private:
		
		bool m_init;

		int m_func_stack;

	public:
		ActionGroup group;

		int& space_stack;
		std::string(&spaces)[200];

		std::map<std::string, std::string> localTable;
	};

	Converter& get_converter();
}

