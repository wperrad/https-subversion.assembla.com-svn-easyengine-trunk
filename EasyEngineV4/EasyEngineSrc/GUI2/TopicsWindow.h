#pragma once
#include "GUIWindow.h"

class CGUIManager;
class CTopicFrame;

struct CTopicInfo
{
	string m_sValue;
	int m_nSpeakerId;

	CTopicInfo() :m_nSpeakerId(-1) {}
};

class CTopicsWindow : public CGUIWindow
{
public:
	CTopicsWindow(IGUIManager& oGUIManager, int width, int height);
	virtual ~CTopicsWindow();
	void									AddTopic(string sTopicName, string sTopicValue, int nSpeakerId);
	void									Display();
	void									DisplayTopicInfos(string sTopic);

private:

	CTopicFrame*							m_pTopicFrame;
	CGUIManager&							m_oGUIManager;
	string									m_sText;
	const int								m_nMaxCharPerLine;
};

class CTopicFrame : public CGUIWidget
{
public:
	enum TTopicState {
		eNormal = 0,
		eHover,
		ePressed,
		eReleased
	};

	CTopicFrame(IGUIManager& oGUIManager, int width, int height);
	void										Display();
	void										AddTopic(string sTopicName, string sTopicValue, int nSpeakerId);
	void										SetParent(CGUIWidget* parent);
	int											GetTextHeight();

private:

	IGUIManager&								m_oGUIManager;
	const int									m_nXTextMargin;
	const int									m_nYTextmargin;
	const int									m_nYmargin;
	const int									m_nTextHeight;
	map<string, vector<CTopicInfo>>				m_mTopics;
	map<string, TTopicState>					m_mTopicsState;
	const int									m_nTopicBorderWidth;
	map<TTopicState, IGUIManager::TFontColor>	m_mFontColorFromTopicState;
	CTopicsWindow*								GetParent();

	int											GetTopicIndexFromY(int y);
	void										OnItemSelected(int itemIndex);
	void										OnItemRelease(int itemIndex);
	void										OnItemHover(int itemIndex);
	static void									OnEventCallback(IGUIManager::ENUM_EVENT nEvent, CGUIWidget* pWidget, int x, int y);
};


