#include "stdafx.h"
#include "WorldEditor.h"
#include "TriggerEditor.h"

WorldEditor::WorldEditor()
{
	m_tempPath = NULL;

	TriggerEditor* triggerEditor = TriggerEditor::getInstance();

	TriggerConfigData* configData = std_call<TriggerConfigData*>(getAddress(0x004D4DA0));
	triggerEditor->loadTriggerConfig(configData);
}

WorldEditor::~WorldEditor()
{

}

WorldEditor* WorldEditor::getInstance()
{
	static WorldEditor instance;

	return &instance;
}

uintptr_t WorldEditor::getAddress(uintptr_t addr)
{
	uintptr_t base = (uintptr_t)GetModuleHandle(NULL);
	return addr - 0x00400000 + base;
}


EditorData* WorldEditor::getEditorData()
{
	uintptr_t addr = *(uintptr_t*)getAddress(0x803cb0);

	uintptr_t count = *(uintptr_t*)(addr + 0x1b0);

	uintptr_t object = *(uintptr_t*)(*(uintptr_t*)(addr + 0x1a8) + count * 4);

	if (*(uintptr_t*)(object + 0x114))
	{
		uintptr_t uknow = *(uintptr_t*)(object + 0x118);

		return *(EditorData**)uknow;
	}
	MessageBoxA(0, "��ȡ�����༭������", "���󣡣���", MB_OK);
	return 0;
}

void WorldEditor::saveMap(const char* outPath)
{
	this_call<int>(getAddress(0x0055ccd0), getEditorData(), outPath);
}






const char* WorldEditor::getCurrentMapPath()
{
	uintptr_t addr = *(uintptr_t*)getAddress(0x803cb0);

	uintptr_t count = *(uintptr_t*)(addr + 0x1b0);

	uintptr_t object = *(uintptr_t*)(*(uintptr_t*)(addr + 0x1a8) + count * 4);


	return (const char*)object;
}

const char* WorldEditor::getTempSavePath()
{
	return m_tempPath;
}

int WorldEditor::getSoundDuration(const char* path)
{
	uint32_t param[10];
	ZeroMemory(&param, sizeof param);
	fast_call<int>(WorldEditor::getInstance()->getAddress(0x004DCFA0), path, &param);
	return param[1];
}

bool WorldEditor::getSkillObjectData(uint32_t id,uint32_t level,std::string text, std::string& value)
{
	uint32_t data = std_call<uint32_t>(getAddress(0x004D4EE0));
	char buffer[0x400];
	bool ret = this_call<bool>(getAddress(0x0050B7B0), data, id, text.c_str(), buffer, 0x400, level, 1);
	if (ret) value = buffer;
	return ret;
}

void WorldEditor::onSaveMap(const char* tempPath)
{
	m_tempPath = tempPath;

	printf("��ǰ��ͼ·��%s\n", getCurrentMapPath());
	printf("�����ͼ·�� %s\n", getTempSavePath());

	TriggerEditor* triggerEditor = TriggerEditor::getInstance();

	TriggerData* triggerData = getEditorData()->triggers;

	triggerEditor->loadTriggers(triggerData);

	int ret = MessageBoxA(0, "�Ƿ����µı���ģʽ����?", "����", MB_YESNO);

	if (ret == 6)
		printf("�Զ��屣��ģʽ\n");
	else 
		printf("ԭʼ����ģʽ\n");
	
	

	clock_t start = clock();
		
	saveWts();
	saveW3i();
	saveImp();
	saveW3e();
	saveShd();
	saveWpm();
	saveMiniMap();//���ɶ��߳�
	saveMmp();
	saveObject();//���ɶ��߳�
	saveDoodas();//���ɶ��߳�
	saveUnits(); //���ɶ��߳�
	saveRect();
	saveCamara();
	saveSound();

	if (ret == 6)
	{
		triggerEditor->saveTriggers(getTempSavePath());
		triggerEditor->saveScriptTriggers(getTempSavePath());
		triggerEditor->saveSctipt(getTempSavePath());
	}
	else
	{
		saveTrigger();
	}

	saveScript();
	saveArchive();

		
	printf("��ͼ�������ݱ������ �ܺ�ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	m_tempPath = NULL;
}

int WorldEditor::saveWts()
{

	printf("����wts�ı�����\n");

	clock_t start = clock() ;

	int ret = this_call<int>(getAddress(0x0055DAF0), getEditorData(), getTempSavePath());

	printf("wts������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}

int WorldEditor::saveW3i()
{
	printf("����w3i��ͼ��Ϣ����\n");

	clock_t start = clock();

	int ret = this_call<int>(getAddress(0x0055D280), getEditorData(), getTempSavePath(), 1);

	printf("w3i������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}
int WorldEditor::saveImp()
{
	printf("����imp�ļ��б�����\n");

	clock_t start = clock();

	uintptr_t object = *(uintptr_t*)((uintptr_t)getEditorData() + 0x3904);
	this_call<int>(getAddress(0x0051CEB0), object, getTempSavePath());
	int ret = this_call<int>(getAddress(0x0055DFD0), getEditorData(), getTempSavePath());

	printf("imp ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}

int WorldEditor::saveW3e()
{
	printf("����w3e������������\n");

	clock_t start = clock();

	int ret = this_call<int>(getAddress(0x005B0C50), getEditorData()->terrain, getTempSavePath());

	printf("w3e ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;

}

int WorldEditor::saveShd()
{
	printf("����shd������Ӱ����\n");

	clock_t start = clock();

	int ret = this_call<int>(getAddress(0x0055d1f0), getEditorData(), getTempSavePath());

	printf("shd ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}


int WorldEditor::saveWpm()
{
	printf("����wpm����·������\n");

	clock_t start = clock();


	std::string path = std::string(getTempSavePath()) + ".wpm";
	int ret = this_call<int>(getAddress(0x005E91C0), getEditorData()->terrain, path.c_str());

	printf("wpm ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}

int WorldEditor::saveMiniMap()
{
	printf("����minimapС��ͼ����\n");

	clock_t start = clock();

	std::string path = std::string(getTempSavePath()) + "Map.tga";

	int ret = this_call<int>(getAddress(0x00583200), getEditorData(), path.c_str(), 0);

	printf("minimap ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}


int WorldEditor::saveMmp()
{
	printf("����mmpԤ��С�ļ�������\n");

	clock_t start = clock();
	int ret = this_call<int>(getAddress(0x00583D00), getEditorData(), getTempSavePath());

	printf("mmp ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}

int WorldEditor::saveObject()
{
	printf("�����������\n");

	clock_t start = clock();

	std::string path = std::string(getTempSavePath()) + "Map.tga";


	uintptr_t object = *(uintptr_t*)((uintptr_t)getEditorData() + 0x3908);
	this_call<int>(getAddress(0x00518CA0), object, getTempSavePath());

	object = *(uintptr_t*)((uintptr_t)getEditorData() + 0x390c);
	this_call<int>(getAddress(0x00518CA0), object, getTempSavePath());

	int ret = this_call<int>(getAddress(0x0051B5B0), getEditorData()->objects, getTempSavePath(), 1);

	printf("��� ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;;
}

int WorldEditor::saveDoodas()
{
	printf("����war3map.doo����װ����\n");

	clock_t start = clock();

	int ret = this_call<int>(getAddress(0x0062BAE0), getEditorData()->doodas, getTempSavePath(), 1);

	printf("war3map.doo ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}


int WorldEditor::saveUnits()
{
	printf("����war3mapUnit.doo���ε�λԤ������\n");

	clock_t start = clock();

	int ret = this_call<int>(getAddress(0x0062BAE0), getEditorData()->units, getTempSavePath(), 1);

	printf("war3mapUnit.doo ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}

int WorldEditor::saveRect()
{
	printf("����w3r������������\n");

	clock_t start = clock();

	int ret = this_call<int>(getAddress(0x0062ACF0), getEditorData()->rects, getTempSavePath());

	printf("war3map.doo ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}


int WorldEditor::saveCamara()
{
	printf("����w3cԤ�辵ͷ����\n");

	clock_t start = clock();

	int ret = this_call<int>(getAddress(0x005AEBB0), getEditorData()->cameras, getTempSavePath());

	printf("w3c ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}
int WorldEditor::saveSound()
{
	printf("����w3s��������\n");

	clock_t start = clock();

	int ret = this_call<int>(getAddress(0x005EACE0), getEditorData()->sounds, getTempSavePath());

	printf("w3s ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}

int WorldEditor::saveTrigger()
{
	printf("����wtg + wct ��������\n");

	clock_t start = clock();

	int ret = this_call<int>(getAddress(0x00520ED0), getEditorData()->triggers, getTempSavePath(), 1);

	printf("wtg + wct  ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}

int WorldEditor::saveScript()
{
	printf("����war3map.j�ű��ļ�\n");

	clock_t start = clock();

	int ret = this_call<int>(getAddress(0x0055DA80), getEditorData(), getTempSavePath());

	printf("war3map.j ������� ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return ret;
}



int WorldEditor::saveArchive()
{


	fs::path path = fs::path(getTempSavePath());
	path.remove_filename();

	std::string name = path.filename().string();
	if (name.length() < 4)
		return 0;

	name = name.substr(0,name.length() - 4);

	fs::path pathTemp = path / name;

	printf("������ļ��д����mpq�ṹ\n");

	printf("·�� %s\n", path.string().c_str());

	clock_t start = clock();


	int ret = this_call<int>(getAddress(0x0055D720), getEditorData(), pathTemp.string().c_str(), 1);

	if (ret)
	{
		path.remove_filename();

		fs::path path2 = path / name;

		//�ƶ��ļ�Ŀ¼
		ret = fast_call<int>(getAddress(0x004D0F60), pathTemp.string().c_str(), path2.string().c_str(), 1, 0);

		printf("��ͼ������ ��ʱ : %f ��\n", (double)(clock() - start) / CLOCKS_PER_SEC);

		return ret;

	}

	
	return 0;
}