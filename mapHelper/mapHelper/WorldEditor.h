#pragma once

#include "stdafx.h"
#include "TriggerEditor.h"


struct RandomGroup
{
	uint32_t rate;//0x0 �ٷֱ�
	const char names[10][4]; //һ���������10������
};

struct RandomGroupData
{
	uint32_t unknow1;//0x0
	const char name[0x64];//0x4
	uint32_t param_count;//0x68//��ʾ ����������ֵ�����
	char unknow3[0x2c];//0x6c
	uint32_t group_count;//0x98
	RandomGroup* groups;//0x9c
	uint32_t unknow2;//0xA0
};//size 0xA4

struct ItemTableInfo
{
	const char name[0x4];//��Ʒid
	uint32_t rate;//0x4 ����
};
struct ItemTableSetting
{
	uint32_t info_count;//0x0
	uint32_t info_count2;//0x4
	ItemTableInfo* item_infos;//0x8
	uint32_t unknow;//0xc
};//0x10

struct ItemTable
{
	uint32_t unknow1;//0x0
	const char name[0x64];//0x4;
	uint32_t setting_count;//0x68
	uint32_t setting_count2;//0x6c
	ItemTableSetting* item_setting;//0x70
	uint32_t unknow2;//0x74
};//size 0x78

struct UnitItem
{
	uint32_t slot_id;//0x0
	const char name[0x4];//0x4 ��Ʒid
};//size 0x8

struct UnitSkill
{
	const char name[0x4];//0x0 ����id
	uint32_t unknow1;//0x4
	uint32_t level;//0x8
};//size 0xc

struct Unit
{
	uint32_t unknow1;//0x0
	const char name[0x4];//0x4 ��λ���id
	uint32_t variation;//0x8 ��ʽ
	float x;//0xc	//�ڵ����е�����
	float y;//0x10
	float z;//0x14
	float facing;//0x18 ������ Ҫת�ؽǶ� * 180 / pi = �Ƕ���
	float sacle_x;//0x1c 
	float sacle_y;//0x20
	float scale_z;//0x24
	float sacle;//0x28
	char unknow2[0x54];//0x2c
	uint32_t color;//0x80 ��ɫ �����䷶Χ-1ʱ��0xFFFFFFFF ��-2ʱ��0xFF1010FF
	char unknow24[0x3]; //0x84
	uint8_t type;//0x87 ���� ��Ʒ��1
	char unknow22[0x34];//0x88
	uint32_t player_id;//0xbc
	uint32_t unknow13;//0xc0
	uint32_t life;//0xc4	�����ٷֱ� ��С1 ���ڻ����100��Ϊ��1
	uint32_t mana;//0xc8	ħ���ٷֱ� ��С1 ���ڻ����100��Ϊ��1
	uint32_t level;//0xcc	�ȼ�
	uint32_t state_str;//0xd0 ���� ����
	uint32_t state_agi;//0xd4 ���� ����
	uint32_t state_int;//0xd8 ���� ����
	uint32_t item_table_index;//0xdc ��Ʒ�������
	uint32_t item_setting_count;//0xe0
	uint32_t item_setting_count2;//0xe4
	ItemTableSetting* item_setting;//0xe8
	char unknow4[0x8];//0xec
	float warning_range;//0xf4 ���䷶Χ -1 ����ͨ -2 ��Ӫ��
	uint32_t item_count;//0xf8
	uint32_t item_count2;//0xfc
	UnitItem* items;//0x100
	uint32_t unknow14;//0x104
	uint32_t skill_count;//0x108
	uint32_t skill_count2;//0x10c
	UnitSkill* skills;//0x110
	char unknow3[0x4];//0x114

	//0x118 �����Ʒģʽ 0 Ϊ�κ���Ʒ ָ�� �ȼ�������  1 ��������� 2 �������Զ����б�
	uint32_t random_item_mode;//0x118

	uint8_t random_item_level;//0x11c
	char unknow23[0x2];//0x11d
	uint8_t random_item_type;//0x11f
	
	uint32_t random_group_index;//0x120 ������id �������ƷģʽΪ1ʱȡ��
	uint32_t random_group_child_index; // 0x124����� ����λ�� �����������Ϊ-1

	uint32_t random_item_count;//0x128
	uint32_t random_item_count2;//0x12c
	ItemTableInfo* random_items;//0x130

	char unknow25[0x18];//0x134
	uint32_t doodas_life;// 0x14c ���ƻ��������
	char unknow21[0x8];//0x150
	uint32_t index;//0x158 ȫ��Ԥ�������id
	char unknow28[0x2c];//0x15c
};//size 0x188

struct UnitData
{
	char unknow[0x5c];//0x0
	uint32_t unit_count;//0x5c
	Unit* array;//0x60
};


struct Sound
{
	const char name[0x64];//0x0 ��gg_snd_ ǰ׺��ȫ�ֱ�����
	const char file[0x104];//0x64
	const char effect[0x34];//0x168
	uint32_t flag;//0x19c  1 | 2 | 4   1���Ƿ�ѭ�� 2 �Ƿ�3D��Ч 4 ������Χֹͣ
	uint32_t fade_out_rate;//0x1a0 ������
	uint32_t fade_in_rate;//0x1a4 ������
	uint32_t volume;//0x1a8 ����
	float pitch;//0x1ac ���� = ����
	char unknow1[0x8];//0x1b0
	uint32_t channel;//0x1b8 ͨ��
	float min_range;//0x1bc ��С˥����Χ
	float max_range;//0x1c0 ���˥����Χ
	float distance_cutoff;//0x1c4 �ضϾ���
	float inside;//0x1c8
	float outside;//0x1cc
	uint32_t outsideVolume;//0x1d0
	float x;//0x1d4
	float y;//0x1d8
	float z;//0x1dc
};//size 0x1E0

struct SoundData
{
	uint32_t unknow1;//0x0
	uint32_t sound_count;//0x4
	Sound* array;//0x8
};


struct EditorData
{
	char unknow1[0x38c4];// 0x0
	uint32_t random_group_count;//0x38c4���������
	RandomGroupData* random_groups;//0x38c8//�����
	char unknow2[0x8];//0x38cc
	uint32_t item_table_count;//0x38d4 ��Ʒ�б�����
	ItemTable* item_table;//0x38d8		��Ʒ��
	char unknow3[0x4];//0x38dc
	void* terrain;//0x38e0
	UnitData* doodas;//0x38e4
	UnitData* units;//0x38e8
	void* rects;//0x38ec
	struct TriggerData* triggers;//0x38f0 //�����༭������
	void* cameras; //0x38f4
	void* objects;//0x38f8
	SoundData* sounds; //0x38fc
};

class WorldEditor
{
public:

	WorldEditor();
	~WorldEditor();
	
	static WorldEditor* getInstance();

	uintptr_t getAddress(uintptr_t addr);

	EditorData* getEditorData();

	const char* getCurrentMapPath();

	const char* getTempSavePath();

	int getSoundDuration(const char* path);

	void saveMap(const char* outPath);

	void onSaveMap(const char* tempPath);

private:

	int saveWts();
	int saveW3i();
	int saveImp();
	int saveW3e();
	int saveShd();
	int saveWpm();
	int saveMiniMap();
	int saveMmp();
	int saveObject();
	int saveDoodas();
	int saveUnits();
	int saveRect();
	int saveCamara();
	int saveSound();
	int saveTrigger();
	int saveScript();
	int saveArchive();

protected: 

	EditorData* editData;


	uintptr_t m_editorObject;
	const char* m_tempPath;
	bool m_bInit;

};