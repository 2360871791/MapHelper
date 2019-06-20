#pragma once
#include "stdafx.h"
#include "inline.h"
#include "EditorData.h"
#include "WorldEditor.h"

template<typename dst_type, typename src_type>
dst_type union_cast(src_type src)
{
	union {
		src_type s;
		dst_type d;
	}u;
	u.s = src;
	return u.d;
}


class Helper
{
public:
	Helper();
	~Helper();
	
	static Helper* getInstance();

	void enableConsole();

	void attach();//����

	void detach();//����

	int getConfig();
private: 

	//�����ͼ
	uintptr_t onSaveMap();

	//ѡ��ת��ģʽ
	int onSelectConvartMode();

	//���Զ���ת������ʱ
	int onConvertTrigger(Trigger* trg);

protected:
	bool m_bAttach;

	hook::hook_t* m_hookSaveMap;
	hook::hook_t* m_hookConvertTrigger;

	fs::path m_configPath;
};
