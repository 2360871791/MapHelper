#include "stdafx.h"
#include "YDTrigger.h"
#include "TriggerEditor.h"
#include "WorldEditor.h"


YDTrigger::YDTrigger()
	:m_bEnable(true),
	 m_funcStack(0)
{
	
}

YDTrigger::~YDTrigger()
{

}

YDTrigger* YDTrigger::getInstance()
{
	static YDTrigger instance;

	return &instance;
}

bool YDTrigger::isEnable()
{
	return m_bEnable;
}

void YDTrigger::onGlobals(BinaryWriter& writer)
{
	writer.write_string("#include <YDTrigger/Import.h>\n");
	writer.write_string("#include <YDTrigger/YDTrigger.h>\n");
}

void YDTrigger::onEndGlobals(BinaryWriter& writer)
{
	writer.write_string("#include <YDTrigger/Globals.h>\n");
	writer.write_string("endglobals\n");
	writer.write_string("#include <YDTrigger/Function.h>\n");
}

bool YDTrigger::onRegisterEvent(std::string& events,Trigger* trigger,Action* action,std::string& name)
{
	if (name == "YDWEDisableRegister")
	{
		m_triggerHasDisable[trigger] = true;
		return false;
	}
		

	m_hasAnyPlayer = false;
	//�����¼��� �����Ӷ����� Ԥ����� �Ƿ��� ������� 
	std::function<bool(Parameter**,uint32_t)> seachAnyPlayer = [&](Parameter** params,uint32_t count)
	{
		for (int i = 0; i < count; i++)
		{
			Parameter* param = params[i];
			Parameter**  childs = NULL;
			uint32_t child_count = 0;
			switch (param->typeId)
			{
			case Parameter::Type::preset:
				if (!strcmp(param->value, "PlayerALL"))
					return true;
				break;
			case Parameter::Type::variable:
				if (param->arrayParam)
				{
					child_count = 1;
					childs = &param->arrayParam;
				}
				break;
			case Parameter::Type::function:
				child_count = param->funcParam->param_count;
				childs = param->funcParam->parameters;
				break;
			}
			if (child_count > 0 && childs)
			{
				return seachAnyPlayer(childs, child_count);
			}
		}
		return false;
	};

	if (seachAnyPlayer(action->parameters, action->param_count))
	{
		events += "#define YDTRIGGER_COMMON_LOOP(n) ";
		m_hasAnyPlayer = true;
	}
	
	return true;
}

void YDTrigger::onRegisterEvent2(std::string& events, Trigger* trigger, Action* action, std::string& name)
{
	if (m_hasAnyPlayer)
	{
		events += "#define YDTRIGGER_COMMON_LOOP_LIMITS (0, 15)\n";
		events += "#include <YDTrigger/Common/loop.h>\n";
		m_hasAnyPlayer = false;
	}
}

bool YDTrigger::onActionToJass(std::string& output, Action* action,ActionNode* node, std::string& pre_actions, const std::string& trigger_name, bool nested)
{
	if (!action->enable)
		return false;
	TriggerEditor* editor = TriggerEditor::getInstance();
	int& stack = editor->space_stack;

	std::vector<ActionNodePtr> list;

	Parameter** parameters = action->parameters;
    switch (node->name_id)
	{
	case "YDWEForLoopLocVarMultiple"s_hash:
	{
		std::string variable = std::string("ydul_") + action->parameters[0]->value;
		output += editor->spaces[stack];
		output += "set " + variable + " = ";
		output += editor->resolve_parameter(parameters[1], node, trigger_name, pre_actions) + "\n";
		output += editor->spaces[stack];
		output += "loop\n";
		output += editor->spaces[++stack];
		output += "exitwhen " + variable + " > " + editor->resolve_parameter(parameters[2], node, trigger_name, pre_actions) + "\n";
	
		node->getChildNodeList(list);
		for (auto& child : list)
		{
			output += editor->spaces[stack];
			//ѭ������Ӷ��� ����������ͬ�ĸ��ڵ�
			output += editor->convert_action_to_jass(child, pre_actions, trigger_name, false) + "\n";
		}

		output += editor->spaces[stack];
		output += "set " + variable + " = " + variable + " + 1\n";
		output += editor->spaces[--stack];
		output += "endloop\n";
		return true;
	}
	case "YDWEEnumUnitsInRangeMultiple"s_hash:
	{
		if (m_isInYdweEnumUnit) break;
		m_isInYdweEnumUnit = true;

		output += "set ydl_group = CreateGroup()\n";
		output += editor->spaces[stack];
		output += "call GroupEnumUnitsInRange(ydl_group,";
		output += parameters[0]->value;
		output += ",";
		output += parameters[1]->value;
		output += ",";
		output += parameters[2]->value;
		output += ",null)\n";
		output += editor->spaces[stack];
		output += "loop\n";

		output += editor->spaces[++stack];
		output += "set ydl_unit = FirstOfGroup(ydl_group)\n";

		output += editor->spaces[stack];
		output += "exitwhen ydl_unit == null\n";

		output += editor->spaces[stack];
		output += "call GroupRemoveUnit(ydl_group, ydl_unit)\n";

		node->getChildNodeList(list);
		for (auto& child : list)
		{
			output += editor->spaces[stack];
			//ѭ������Ӷ��� ����������ͬ�ĸ��ڵ�
			output += editor->convert_action_to_jass(child, pre_actions, trigger_name, false) + "\n";
		}
		output += editor->spaces[--stack];
		output += "endloop\n";
		output += editor->spaces[stack];
		output += "call DestroyGroup(ydl_group)\n";

		m_isInYdweEnumUnit = false;
		return true;
	}
	case "YDWESaveAnyTypeDataByUserData"s_hash:
	{
		output += "call YDUserDataSet(";
		output += parameters[0]->value + 11; //typename_01_integer  + 11 = integer
		output += ",";
		output += editor->resolve_parameter(parameters[1], node, trigger_name, pre_actions);
		output += ",\"";
		output += parameters[2]->value;
		output += "\",";
		output += parameters[3]->value + 11;
		output += ",";
		output += editor->resolve_parameter(parameters[4], node, trigger_name, pre_actions);
		output += ")\n";
		return true;
	}
	case "YDWEFlushAnyTypeDataByUserData"s_hash:
	{
		output += "call YDUserDataClear(";
		output += parameters[0]->value + 11; //typename_01_integer  + 11 = integer
		output += ",";
		output += editor->resolve_parameter(parameters[1], node, trigger_name, pre_actions);
		output += ",\"";
		output += parameters[3]->value;
		output += "\",";
		output += parameters[2]->value + 11;
		output += ")\n";

		return true;
	}
	case "YDWEFlushAllByUserData"s_hash:
	{
		output += "call YDUserDataClearTable(";
		output += parameters[0]->value + 11; //typename_01_integer  + 11 = integer
		output += ",";
		output += editor->resolve_parameter(parameters[1], node, trigger_name, pre_actions);
		output += ")\n";
		return true;
	}

	case "YDWEExecuteTriggerMultiple"s_hash:
	{
		output += "set ydl_trigger = ";
		output += editor->resolve_parameter(parameters[0], node, trigger_name, pre_actions);
		output += "\n" + editor->spaces[stack];
		output += "YDLocalExecuteTrigger(ydl_trigger)\n";
	
		node->getChildNodeList(list);
		for (auto& child : list)
		{
			output += editor->spaces[stack];
			output += editor->convert_action_to_jass(child, pre_actions, trigger_name, false) + "\n";
		}
		output += editor->spaces[stack];
		output += "call YDTriggerExecuteTrigger(ydl_trigger,";
		output += editor->resolve_parameter(parameters[1], node, trigger_name, pre_actions);
		output += ")\n";
		return true;
		
	}
	case "YDWERegisterTriggerFlush"s_hash:
	{
		ActionNodePtr parent = node->getParentNode();

		if (parent.get() && parent->getNameId() == "YDWEExecuteTriggerMultiple"s_hash)
		{
			output += "call YDLocal4Release()\n";
			output += editor->spaces[stack];
			output += "call call DestroyTrigger(GetTriggeringTrigger())\n";
			return true;
		}
		else
		{
			output += "��Ҫ�����촥�����Ķ�����ʹ��<������촥����>";
			return true;
		}
	}
	case "YDWETimerStartMultiple"s_hash:
	{

		std::string param_text;
		std::string action_text;

		param_text += "set ydl_timer = ";
		param_text += editor->resolve_parameter(parameters[0], node, trigger_name, pre_actions) + "\n";


		std::map<std::string, std::string> hashVarTable;

		//��ǰ��һ����Ҫ���εı�����
		std::map<std::string, std::string> thisVarTable;

		//�������һ����Ҫ���εı����� 
		std::map<std::string, std::string>* mapPtr = NULL;
		
		//�ҵ���һ�㺯��������ֲ�������
		ActionNode* ptr = node;
		while (ptr)
		{
			if (ptr->mapPtr)
			{
				mapPtr = ptr->mapPtr;
				break;
			}
			ptr = ptr->parent;
		}
	
		if (mapPtr == NULL)
		{
			mapPtr = new std::map<std::string, std::string>;
			node->mapPtr = mapPtr;
		}

		node->getChildNodeList(list);
		for (auto& child : list)
		{
			Action* childAction = child->getAction();
			if (child->getActionId() == 0)//����ǲ�����
			{
				switch (child->getNameId())
				{
					//�������ʱ��������ʹ������ֲ�����
				case "YDWESetAnyTypeLocalArray"s_hash:
				case "YDWESetAnyTypeLocalVariable"s_hash:
				{
					std::string var_name = childAction->parameters[1]->value;
					std::string var_type = childAction->parameters[0]->value + 11;
					hashVarTable.emplace(var_name, var_type);
					break;
				}
				}
				param_text += editor->spaces[stack];
				param_text += editor->convert_action_to_jass(child, pre_actions, trigger_name, false) + "\n";
			}
		}


		int s = stack;
		stack = 1;
		
		for (auto& child : list)
		{
			if (child->getNameId() != 0)//����Ƕ�����
			{
				seachHashLocal(childAction->parameters, childAction->param_count, &thisVarTable);
				action_text += editor->spaces[stack];
				action_text += editor->convert_action_to_jass(child, pre_actions, trigger_name, false) + "\n";
			}
		}

		stack = s;

		for (auto&[n, t] : hashVarTable)
		{	
			thisVarTable.erase(n);
			if (node->mapPtr)
			{
				node->mapPtr->erase(n);
			}

		}

		output += param_text;

		ActionNode temp(action, node);

		//�����ǰ�������Ҫ����ı���
		if (node->mapPtr)
		{
			for (auto&[n, t] : *node->mapPtr)
			{
				output += editor->spaces[stack];
				output += setLocal(&temp, n, t, getLocal(node->parent, n, t), true) + "\n";
			}
		}
		
		//����һ����Ҫ���εı��� ���ݸ���һ��
		for (auto&[n, t] : thisVarTable)
		{
			output += editor->spaces[stack];
			output += setLocal(&temp, n, t, getLocal(node->parent, n, t),true) + "\n";

			mapPtr->emplace(n, t);
		}
		if (node->mapPtr)
		{
			delete node->mapPtr;
			node->mapPtr = NULL;
		}


		std::string func_name = editor->generate_function_name(trigger_name);
		pre_actions += "function " + func_name + " takes nothing returns nothing\n";

		std::vector<LocalVar> localTable;

		onActionsToFuncBegin(pre_actions, NULL,NULL, &localTable);
		pre_actions += action_text;
		onActionsToFuncEnd(pre_actions, NULL, node, &localTable);
		pre_actions += "endfunction\n";

		output += editor->spaces[stack];
		output += "call TimerStart(ydl_timer,";
		output += editor->resolve_parameter(parameters[1], node, trigger_name, pre_actions);
		output += ",";
		output += editor->resolve_parameter(parameters[2], node, trigger_name, pre_actions);
		output += ",function ";
		output += func_name;
		output += ")";

		return true;
		
	}


	case "YDWERegisterTriggerMultiple"s_hash:
	{
		std::string param_text;
		std::string action_text;

		param_text += "set ydl_trigger = ";
		param_text += editor->resolve_parameter(parameters[0], node, trigger_name, pre_actions) + "\n";


		std::map<std::string, std::string> hashVarTable;

		//��ǰ��һ����Ҫ���εı�����
		std::map<std::string, std::string> thisVarTable;

		//�������һ����Ҫ���εı����� 
		std::map<std::string, std::string>* mapPtr = NULL;

		//�ҵ���һ�㺯��������ֲ�������
		ActionNode* ptr = node;
		while (ptr)
		{
			if (ptr->mapPtr)
			{
				mapPtr = ptr->mapPtr;
				break;
			}
			ptr = ptr->parent;
		}

		if (mapPtr == NULL)
		{
			mapPtr = new std::map<std::string, std::string>;
			node->mapPtr = mapPtr;
		}

		node->getChildNodeList(list);

		for (auto& child : list)
		{
			Action* childAction = action->child_actions[i];

			if (childAction->child_flag < 2)//������¼��� or ������
			{
				switch (hash_(childAction->name))
				{
					//�������ʱ��������ʹ������ֲ�����
				case "YDWESetAnyTypeLocalArray"s_hash:
				case "YDWESetAnyTypeLocalVariable"s_hash:
				{
					std::string var_name = childAction->parameters[1]->value;
					std::string var_type = childAction->parameters[0]->value + 11;
					hashVarTable.emplace(var_name, var_type);
					break;
				}
				}
				param_text += editor->spaces[stack];
				param_text += editor->convert_action_to_jass(child, pre_actions, trigger_name, false) + "\n";
			}
		}

		int s = stack;
		stack = 1;

		for (auto& child : list)
		{
			Action* childAction = action->child_actions[i];

			if (childAction->child_flag == 2)//����Ƕ�����
			{
				seachHashLocal(childAction->parameters, childAction->param_count, &thisVarTable);
				action_text += editor->spaces[stack];
				action_text += editor->convert_action_to_jass(child, pre_actions, trigger_name, false) + "\n";
			}
		}

		stack = s;

		for (auto&[n, t] : hashVarTable)
		{
			thisVarTable.erase(n);
			if (node->mapPtr)
			{
				node->mapPtr->erase(n);
			}

		}

		output += param_text;

		ActionNode temp(action, node);

		//�����ǰ�������Ҫ����ı���
		if (node->mapPtr)
		{
			for (auto&[n, t] : *node->mapPtr)
			{
				output += editor->spaces[stack];
				output += setLocal(&temp, n, t, getLocal(node->parent, n, t), true) + "\n";
			}
		}

		//����һ����Ҫ���εı��� ���ݸ���һ��
		for (auto&[n, t] : thisVarTable)
		{
			output += editor->spaces[stack];
			output += setLocal(&temp, n, t, getLocal(node->parent, n, t), true) + "\n";

			mapPtr->emplace(n, t);
		}
		if (node->mapPtr)
		{
			delete node->mapPtr;
			node->mapPtr = NULL;
		}


		std::string func_name = editor->generate_function_name(trigger_name);
		pre_actions += "function " + func_name + " takes nothing returns nothing\n";
		std::vector<LocalVar> localTable;
		onActionsToFuncBegin(pre_actions, NULL, node,&localTable);
		pre_actions += action_text;
		onActionsToFuncEnd(pre_actions, NULL, node,&localTable);
		pre_actions += "endfunction\n";

		output += editor->spaces[stack];
		output += "call TriggerAddCondition(ydl_trigger,Condition(function ";
		output += func_name;
		output += "))";

		return true;
	}

	case "YDWESetAnyTypeLocalVariable"s_hash:
	{
	
		std::string var_name = parameters[1]->value;
		std::string var_type= parameters[0]->value + 11;

		std::string var_value = editor->resolve_parameter(parameters[2], node, trigger_name, pre_actions);
	
		output +=setLocal(node,var_name, var_type, var_value);
		return true;
	}
	case "YDWESetAnyTypeLocalArray"s_hash:
	{

		std::string var_name = parameters[1]->value;
		std::string var_type = parameters[0]->value + 11;

		std::string index = editor->resolve_parameter(parameters[2], node, trigger_name, pre_actions);
		std::string var_value = editor->resolve_parameter(parameters[3], node, trigger_name, pre_actions);

		output += setLocalArray(node, var_name, var_type,index, var_value);
		return true;
	}



	case "YDWETimerStartFlush"s_hash:
	{
		ActionNode root = getRootNode(node);
		if (root.parent && strcmp(root.parent->action->name, "YDWETimerStartMultiple") == 0)
		{
			output += "call YDLocal3Release()\n";
			output += editor->spaces[stack];
			output += "call DestroyTimer(GetExpiredTimer())\n";
			return true;
		}
		else
		{
			output += "��Ҫ�������ʱ���Ķ�����ʹ��<��������ʱ��>";
			return true;
		}
	}

	
	case "TriggerSleepAction"s_hash:
	case "PolledWait"s_hash:
	{
		ActionNode root = getRootNode(node);
		output += editor->testt(trigger_name, action->name, parameters, action->param_count, node, pre_actions, !nested);
		if (root.parent && (
			strcmp(root.parent->action->name, "YDWETimerStartMultiple") == 0 ||
			strcmp(root.parent->action->name, "YDWERegisterTriggerMultiple") == 0
			))
		{
			output += "��Ҫ�������ʱ��/���촥������ʹ�õȴ�";
		}
		else
		{
			output += editor->spaces[stack];
			output += "call YDLocalReset()\n";
		}
			
		return true;
	}
	case "ReturnAction"s_hash:
	{
		output += "call YDLocal1Release()\n";
		output += editor->spaces[stack];
		onActionsToFuncEnd(output, NULL, node);
		output += "return\n";
		return true;
	}

	case "YDWECustomScriptCode"s_hash:
	{
		output += parameters[0]->value;
		return true;
	}
	case "YDWEActivateTrigger"s_hash:
	{
		output += "";
		Parameter* param = parameters[0];
		if (param->typeId == Parameter::Type::variable && param->arrayParam == NULL)
		{
			const char* ptr = param->value;
			if (ptr && strncmp(ptr, "gg_trg_", 7) == 0)
				ptr = ptr + 7;
			
			std::string func_name = std::string("InitTrig_") + ptr;
			replace_string(func_name.begin(), func_name.end());

			std::string ret = editor->resolve_parameter(parameters[1], node, trigger_name, pre_actions);
			if (ret.compare("true") == 0)
			{
				output += "call ExecuteFunc(\"" + func_name + "\")\n";
			}
			else
			{
				output += "call " + func_name + "()\n";
			}
		}
		return true;
	}
	}

	return false;
}

bool YDTrigger::onParamterToJass(std::string& output, Parameter* paramter, ActionNode* node, std::string& pre_actions, const std::string& trigger_name, bool nested)
{
	TriggerEditor* editor = TriggerEditor::getInstance();

	switch (paramter->typeId)
	{
	case Parameter::Type::function:
	{
		Action* action = paramter->funcParam;
		if (!action) break;
		Parameter** parameters = action->parameters;
		switch (hash_(action->name))
		{
		case "YDWELoadAnyTypeDataByUserData"s_hash:
		{
			output += "YDUserDataGet(";
			output += parameters[0]->value + 11; //typename_01_integer  + 11 = integer
			output += ",";
			output += editor->resolve_parameter(parameters[1], node, trigger_name, pre_actions);
			output += ",\"";
			output += parameters[2]->value;
			output += "\",";
			output += paramter->type_name;
			output += ")";
			return true;
		}
		case "YDWEHaveSavedAnyTypeDataByUserData"s_hash:
		{
			output += "YDUserDataHas(";
			output += parameters[0]->value + 11; //typename_01_integer  + 11 = integer
			output += ",";
			output += editor->resolve_parameter(parameters[1], node, trigger_name, pre_actions);
			output += ",\"";
			output += parameters[3]->value;
			output += "\",";
			output += parameters[2]->value + 11;
			output += ")";
		}
		case "GetEnumUnit"s_hash:
		{
			if (m_isInYdweEnumUnit)
			{
				output += "ydl_unit";
			}
			else
			{
				output += "GetEnumUnit()";
			}
			return true;
		}
		case "GetFilterUnit"s_hash:
		{
			if (m_isInYdweEnumUnit)
			{
				output += "ydl_unit";
			}
			else
			{
				output += "GetFilterUnit()";
			}
			return true;
		}
		case "YDWEForLoopLocVarIndex"s_hash:
		{
			std::string varname = action->parameters[0]->value;
			output = "ydul_" + varname;
			return true;
		}
		case "YDWEGetAnyTypeLocalVariable"s_hash:
		{
			std::string var_name = parameters[0]->value;
			std::string var_type = paramter->type_name;
	
			output += getLocal(node, var_name, var_type);
			return true;
		}
		case "YDWEGetAnyTypeLocalArray"s_hash:
		{
			std::string var_name = parameters[0]->value;
			std::string var_type = paramter->type_name;
			std::string index = editor->resolve_parameter(parameters[1], node, trigger_name, pre_actions);
	
			output += getLocalArray(node, var_name, var_type,index);
			return true;
		}
		case "YDWECustomScriptCode"s_hash:
		{
			return parameters[0]->value;
		}
		}
	}

	}
	return false;
}

bool YDTrigger::seachHashLocal(Parameter** parameters, uint32_t count, std::map<std::string, std::string>* mapPtr)
{

	for (int i = 0; i < count; i++)
	{
		Parameter* param = parameters[i];

		Parameter**  childs = NULL;
		uint32_t child_count = 0;
		switch (param->typeId)
		{
		case Parameter::Type::variable:
			if (param->arrayParam)
			{
				child_count = 1;
				childs = &param->arrayParam;
			}
			break;
		case Parameter::Type::function:
		{
			Action* action = param->funcParam;
			switch (hash_(action->name))
			{
			case "YDWEGetAnyTypeLocalVariable"s_hash:
			case "YDWEGetAnyTypeLocalArray"s_hash:
			{
				if (mapPtr)
				{
					std::string var_name = action->parameters[0]->value;
					std::string var_type = param->type_name;
					if (mapPtr->find(var_type) == mapPtr->end())
						mapPtr->emplace(var_name, var_type);
				}
				else
				{
					return true; //�������˾��˳��ݹ�
				}
			}
			}
			child_count = action->param_count;
			childs = param->funcParam->parameters;
			break;
		}
		}
		if (child_count > 0 && childs)
		{
			return seachHashLocal(childs, child_count,mapPtr);
		}
	}
	if (mapPtr && mapPtr->size() > 0)
	{
		return true;
	}
	return false;
}
void YDTrigger::onActionsToFuncBegin(std::string& funcCode,Trigger* trigger, ActionNode* parent)
{
	m_funcStack++;

	if (!trigger && !parent)
		return;
	bool isSeachHashLocal = parent == NULL;

	


	//������Ҫע��ľֲ�����
	std::function<void(Action**, uint32_t,Action*,bool,bool)> seachLocal = [&](Action** actions, uint32_t count,Action* parent_action,bool isSeachChild,bool isTimer)
	{
		for (int i = 0; i < count; i++)
		{
			Action* action = actions[i];
			//����������Ӷ��� and action���Ӷ���������
		if (!isSeachChild && action->child_flag != -1 )
				continue;
			
			uint32_t hash = hash_(action->name);
			if (isSeachHashLocal)
			{
				if (!isTimer)
				{
					//������ڼ�ʱ���� ����������
					if (hash == "YDWETimerStartMultiple"s_hash)
						isTimer = true;
				}
		
				//�����������Ƿ������õ�����ֲ�����
				m_isInMainProc = seachHashLocal(action->parameters, action->param_count);
			}

#define next(b) seachLocal(action->child_actions, action->child_count,action,b,isTimer);
			switch (hash)
			{
			case "IfThenElseMultiple"s_hash:
			case "ForLoopAMultiple"s_hash:
			case "ForLoopBMultiple"s_hash:
			case "ForLoopVarMultiple"s_hash:
			case "YDWERegionMultiple"s_hash:
			{
				next(true);
				break;
			}
			case "YDWEForLoopLocVarMultiple"s_hash:
			{
				std::string varname = action->parameters[0]->value;
				addLocalVar("ydul_" + varname, "integer");
				next(true);
				break;
			}
			case "EnumDestructablesInRectAllMultiple"s_hash:
			case "EnumDestructablesInCircleBJMultiple"s_hash:
			case "EnumItemsInRectBJMultiple"s_hash:
			{
				next(true);
				break;
			}
			case "ForGroupMultiple"s_hash:
			case "YDWEEnumUnitsInRangeMultiple"s_hash:
			{
				addLocalVar("ydl_group", "group");
				addLocalVar("ydl_unit", "unit");
				next(false);
				break;
			}

			case "YDWETimerStartMultiple"s_hash:
			{
				addLocalVar("ydl_timer", "timer");
				next(false);
				break;
			}
			case "YDWEExecuteTriggerMultiple"s_hash:
			{
				addLocalVar("ydl_triggerstep", "integer");
				addLocalVar("ydl_trigger", "trigger");
				next(false);
				break;
			}
			case "YDWESetAnyTypeLocalVariable"s_hash:
			case "YDWESetAnyTypeLocalArray"s_hash:
			{
				//ֻ������ʱ���еĲ����������������ֲ�����
				if (isSeachHashLocal && (!isTimer || (isTimer && action->child_flag == 0)))
				{
					m_isInMainProc = true;
				}
				break;
			}

			}
#undef next(b)
		}

	};

	if (trigger)
	{
		seachLocal(trigger->actions, trigger->line_count,NULL,true,false);
	}
	else
	{
		seachLocal(parent->action->child_actions, parent->action->child_count,parent->action,true,false);
	}

	for (auto& i : m_localTable)
	{
		funcCode += "\tlocal " + i.type + " " + i.name;
		if (i.value.empty())
		{
			funcCode += "\n";
		}
		else
		{
			funcCode += " = " + i.value + "\n";
		}
	}

	if (trigger && m_isInMainProc)
	{
		funcCode += "\tYDLocalInitialize()\n";
	}
	
}

void YDTrigger::onActionsToFuncEnd(std::string& funcCode, Trigger* trigger, ActionNode* parent)
{
	m_funcStack--;

	if (trigger && m_isInMainProc)
	{
		funcCode += "\tcall YDLocal1Release()\n";
	}
	for (auto& i : m_localTable)
	{
		switch (hash_(i.type.c_str()))
		{
		case "unit"s_hash:
		case "group"s_hash:
		case "timer"s_hash:
		case "trigger"s_hash:
		case "force"s_hash:
		{
			funcCode += "\tset " + i.name + " = null\n";
			break;
		}
		}
	}

	if (trigger)
	{
		m_localTable.clear();
		m_localMap.clear();
		m_isInMainProc = false;
	}


}

bool YDTrigger::hasDisableRegister(Trigger* trigger)
{
	auto it = m_triggerHasDisable.find(trigger);
	return it != m_triggerHasDisable.end();
}

void YDTrigger::addLocalVar(std::string name, std::string type, std::string value)
{
	if (m_localMap.find(name) != m_localMap.end())
		return;

	//m_localTable.push_back({ name,type,value });
	m_localMap[name] = true;
}


ActionNode YDTrigger::getRootNode(ActionNode* node)
{

	ActionNode* root = node->parent;

	Action* prev = node->action;

	//�������ڵ�
	while (root)
	{

		bool isBreak = false;

		if (root->action)
		{
			switch (root->name_id)
			{
				//�����ɺ����ļ����ڵ�
			case "ForForceMultiple"s_hash:
			case "ForGroupMultiple"s_hash:
			case "EnumDestructablesInRectAllMultiple"s_hash:
			case "EnumDestructablesInCircleBJMultiple"s_hash:
			case "EnumItemsInRectBJMultiple"s_hash:
			case "YDWETimerStartMultiple"s_hash:
			case "YDWERegisterTriggerMultiple"s_hash:
				isBreak = true; break;
			default:
				break;
			}
		}
		if (isBreak) break;

		prev = root->action;
		root = root->parent;
	}
	return ActionNode(prev,root);
}

std::string YDTrigger::setLocal(ActionNode* node, const std::string& name, const std::string& type, const std::string& value,bool add)
{

	ActionNode root = getRootNode(node);

	//���ݵ�ǰ��������ֲ�������λ�� ���������ɵĴ���

	std::string callname;
	std::string handle;

	if (root.parent == NULL)//������ڴ�����
	{
		callname = "YDLocal1Set";

		m_isInMainProc = true;
	}
	else
	{
		switch (root.parent->name_id)
		{
			//������������ʱ����
		case "YDWETimerStartMultiple"s_hash:
		{
			if (root.action->child_flag == 0 || add)
			{
				handle = "ydl_timer";
			}
			else //�����Ƕ�����
			{
				handle = "GetExpiredTimer()";
			}
			callname = "YDLocalSet";
			break;
		}
		//����������촥������
		case "YDWERegisterTriggerMultiple"s_hash:
		{
			if (root.action->child_flag == 0)
			{
				handle = "ydl_trigger";
			}
			else //�����Ƕ�����
			{	
				handle = "GetTriggeringTrigger()";
			}
			callname = "YDLocalSet";
			break;
		}
		//������� �������ж���������
		case "YDWEExecuteTriggerMultiple"s_hash:
		{
			callname = "YDLocal5Set";
			break;
		}
		//���� ������δ����Ķ�������
		default:
		{
			callname = "YDLocal2Set";
		}
		}
	}

	std::string output;

	output += "call " + callname + "(";
	if (!handle.empty()) //��handle������
	{
		output += handle + ",";
	}
	output += type + ",\"" + name + "\",";
	output += value;
	output += ")";

	

	return output;
}

std::string YDTrigger::getLocal(ActionNode* node, const std::string& name,const std::string& type)
{
	ActionNode root = getRootNode(node);

	//���ݵ�ǰ��������ֲ�������λ�� ���������ɵĴ���
	std::string callname;
	std::string handle;
	if (root.action == NULL || root.parent == NULL)//������ڴ�����
	{
		if (m_isInMainProc)
		{
			callname = "YDLocal1Get";
		}
		else
		{
			callname = "YDLocal2Get";
		}
	}
	else
	{
		switch (root.parent->name_id)
		{
			//������������ʱ����
		case "YDWETimerStartMultiple"s_hash:
		{
			if (root.action->child_flag == 0) //0�ǲ�����
			{
				return getLocal(root.parent->parent, name, type);
			}
			else //�����Ƕ�����
			{
				handle = "GetExpiredTimer()";
			}
			callname = "YDLocalGet";
			break;
		}
		//����������촥������
		case "YDWERegisterTriggerMultiple"s_hash:
		{
			if (root.action->child_flag < 2) //0���¼��� 1�ǲ�����
			{
				return getLocal(root.parent->parent, name, type);
			}
			else
			{//�����Ƕ�����
				handle = "GetTriggeringTrigger()";
			}
			callname = "YDLocalGet";
			break;
		}
		//���� ������δ����Ķ�������
		default:
		{
			callname = "YDLocal2Get";
			break;
		}
		}
	}

	std::string output;

	output += callname + "(";
	if (!handle.empty()) //��handle������
	{
		output += handle + ",";
	}
	output += type + ",\"" + name + "\")";

	return output;
}


std::string YDTrigger::setLocalArray(ActionNode* node, const  std::string& name, const std::string& type, const  std::string& index, const  std::string& value)
{

	ActionNode root = getRootNode(node);

	//���ݵ�ǰ��������ֲ�������λ�� ���������ɵĴ���
	std::string callname;
	std::string handle;

	if (root.parent == NULL)//������ڴ�����
	{
		callname = "YDLocal1ArraySet";
	}
	else
	{
		switch (root.parent->name_id)
		{
			//������������ʱ����
		case "YDWETimerStartMultiple"s_hash:
		{
			if (root.action->child_flag == 0)
			{
				handle = "ydl_timer";
			}
			else //�����Ƕ�����
			{
				handle = "GetExpiredTimer()";
			}
			callname = "YDLocalArraySet";
			break;
		}
		//����������촥������
		case "YDWERegisterTriggerMultiple"s_hash:
		{
			if (root.action->child_flag == 0)
			{
				handle = "ydl_trigger";
			}
			else //�����Ƕ�����
			{
				handle = "GetTriggeringTrigger()";
			}

			callname = "YDLocalArraySet";
			break;
		}
		//������� �������ж���������
		case "YDWEExecuteTriggerMultiple"s_hash:
		{
			callname = "YDLocal5ArraySet";
			break;
		}
		//���� ������δ����Ķ�������
		default:
		{
			callname = "YDLocal2ArraySet";
		}

		}
	}

	std::string output;

	output += "call " + callname + "(";
	if (!handle.empty()) //��handle������
	{
		output += handle + ",";
	}
	output += type;
	output += ",\"";
	output += name;
	output += "\",";
	output += index;
	output += ",";
	output += value;
	output += ")";


	return output;
}
std::string YDTrigger::getLocalArray(ActionNode* node, const std::string& name, const std::string& type, const std::string& index)
{
	//���ݵ�ǰ��������ֲ�������λ�� ���������ɵĴ���

	ActionNode root = getRootNode(node);

	std::string callname;
	std::string handle;
	if (root.action == NULL || root.parent == NULL)//������ڴ�����
	{
		if (m_isInMainProc)
		{
			callname = "YDLocal1ArrayGet";
		}
		else
		{
			callname = "YDLocal2ArrayGet";
		}
	}
	else
	{
		switch (root.parent->name_id)
		{
			//������������ʱ����
		case "YDWETimerStartMultiple"s_hash:
		{
			if (root.action->child_flag == 0) //0�ǲ�����
			{
				return getLocal(root.parent->parent, name, type);
			}
			else //�����Ƕ�����
			{
				handle = "GetExpiredTimer()";
			}
			callname = "YDLocalArrayGet";
			break;
		}
		//����������촥������
		case "YDWERegisterTriggerMultiple"s_hash:
		{

			if (root.action->child_flag < 2) //0���¼��� 1�ǲ�����
			{
				return getLocal(root.parent->parent, name, type);
			}
			else
			{//�����Ƕ�����
				handle = "GetTriggeringTrigger()";
			}
			callname = "YDLocalArrayGet";
			break;
		}
		//���� ������δ����Ķ�������
		default:
		{

			callname = "YDLocal2ArrayGet";
			break;
		}
		}
	}
	std::string output;

	output += callname + "(";
	if (!handle.empty()) //��handle������
	{
		output += handle + ",";
	}
	output += type;
	output += ",\"";
	output += name;
	output += "\",";
	output += index;
	output += ")";
	return output;
}