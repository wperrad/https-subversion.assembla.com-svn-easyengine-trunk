#include "TopicsWindow.h"
#include "Utils2/rectangle.h"
#include "GUIManager.h"
#include "IRessource.h"
#include <sstream>

CTopicsWindow::CTopicsWindow(IGUIManager& oGUIManager, int width, int height) :
	CGUIWindow(width, height),
	m_oGUIManager(static_cast<CGUIManager&>(oGUIManager)),
	m_nMaxCharPerLine(96)
{
	CRectangle rect;
	rect.SetDimension(width, height);
	//CGUIManager& oGUI = static_cast<CGUIManager&>(oGUIManager);
	IMesh* pRect = m_oGUIManager.CreateImageFromFile("TopicsWindow.bmp", rect, CDimension(width, height));
	SetRect(pRect);
	_Position.SetPosition(400, 200);
	
	m_pTopicFrame = new CTopicFrame(oGUIManager, 198, 759);
	AddWidget(m_pTopicFrame);
}

void CTopicsWindow::AddTopic(string sTopicName, string sTopicValue, int nSpeakerId)
{
	m_pTopicFrame->AddTopic(sTopicName, sTopicValue, nSpeakerId);
}

CTopicsWindow::~CTopicsWindow()
{
}

void CTopicsWindow::Display()
{
	CGUIWindow::Display();
	m_oGUIManager.Print(m_sText, GetPosition().GetX(), GetPosition().GetY());
}

void CTopicsWindow::DisplayTopicInfos(string sTopicInfo)
{
	vector<int> returnIndices;
	int textIndex = 0;
	while (textIndex < sTopicInfo.size()) {
		int lineIndex = 0;
		int lastWordIndex = 0;
		while ( (textIndex < sTopicInfo.size()) && (lineIndex < m_nMaxCharPerLine) ) {
			if (sTopicInfo[textIndex] == ' ') {
				lastWordIndex = textIndex;
			}
			lineIndex++;
			textIndex++;
		}
		if(lineIndex >= m_nMaxCharPerLine)
			returnIndices.push_back(lastWordIndex);
	}

	string sTruncatedString = sTopicInfo;
	for (int i = 0; i < returnIndices.size(); i++) {
		string::iterator itIndex = sTruncatedString.begin() + returnIndices[i];
		itIndex = sTruncatedString.erase(itIndex);
		sTruncatedString.insert(itIndex, '\n');
	}
	m_sText += "\n" + sTruncatedString + "\n";

	int indexLast = 0;
	int indexNext = 0;
	int lineCount = 0;
	do {
		indexNext = m_sText.find('\n', indexLast);
		indexLast = indexNext + 1;
		lineCount++;
	} while (indexNext != -1);
	
	int maxLineCount = (GetDimension().GetHeight() / m_oGUIManager.GetCurrentFontEspacementY()) - 1;
	int numberOflineToRemove = lineCount - maxLineCount;
	bool bTruncateLine = numberOflineToRemove > 0;
	
	while (numberOflineToRemove-- > 0) {
		int index = m_sText.find('\n');
		m_sText = m_sText.substr(index + 1);
	}
	if(bTruncateLine)
		m_sText.insert(0, "\n");
}

CTopicFrame::CTopicFrame(IGUIManager& oGUIManager, int width, int height) : 
	CGUIWidget(width, height),
	m_oGUIManager(oGUIManager),
	m_nXTextMargin(5),
	m_nYTextmargin(5),
	m_nYmargin(21),
	m_nTextHeight(25), 
	m_nTopicBorderWidth(218)
{
	CGUIManager& oGUI = static_cast<CGUIManager&>(oGUIManager);

	CListener* pEventListener = new CListener;
	pEventListener->SetEventCallBack(OnEventCallback);
	CRectangle rect;
	rect.SetDimension(width, height);
	IMesh* pTopicFrameMesh = oGUI.CreateImageFromFile("topic-frame4.bmp", rect, rect.m_oDim);
	SetRect(pTopicFrameMesh);
	SetListener(pEventListener);
	m_mFontColorFromTopicState[eNormal] = IGUIManager::eWhite;
	m_mFontColorFromTopicState[ePressed] = IGUIManager::eTurquoise;
}

int CTopicFrame::GetTextHeight()
{
	return m_nTextHeight;
}

void CTopicFrame::AddTopic(string sTopicName, string sTopicValue, int nSpeakerId)
{
	CTopicInfo topic;
	topic.m_nSpeakerId = nSpeakerId;
	topic.m_sValue = sTopicValue;
	m_mTopics[sTopicName].push_back(topic);
}

void CTopicFrame::Display()
{
	CGUIWidget::Display();
	int i = 0;
	for(map<string, vector<CTopicInfo>>::iterator itTopic = m_mTopics.begin(); itTopic != m_mTopics.end(); itTopic++){
		int y = _Position.GetY() + i * m_nTextHeight + m_nYTextmargin;
		m_oGUIManager.Print(itTopic->first, GetPosition().GetX() + m_nXTextMargin, y, m_mFontColorFromTopicState[m_mTopicsState[itTopic->first]]);
		i++;
	}
}

int CTopicFrame::GetTopicIndexFromY(int y)
{
	int i = (int)(((float)y - _Position.GetY() - m_nYTextmargin) / (float)m_nTextHeight);
	return i;
}

void CTopicFrame::SetParent(CGUIWidget* parent)
{
	CGUIWidget::SetParent(parent);
	SetPosition(GetPosition().GetX() + m_pParent->GetDimension().GetWidth() - m_nTopicBorderWidth, GetPosition().GetY() + m_nYmargin);
}

CTopicsWindow* CTopicFrame::GetParent()
{
	return (CTopicsWindow*)m_pParent;
}

void CTopicFrame::OnItemSelected(int itemIndex) 
{
	map<string, vector<CTopicInfo>>::iterator itTopic = m_mTopics.begin();
	std::advance(itTopic, itemIndex);
	m_mTopicsState[itTopic->first] = ePressed;
}

void CTopicFrame::OnItemRelease(int itemIndex)
{
	map<string, vector<CTopicInfo>>::iterator itTopic = m_mTopics.begin();
	std::advance(itTopic, itemIndex);
	m_mTopicsState[itTopic->first] = eReleased;
	GetParent()->DisplayTopicInfos(itTopic->second[0].m_sValue);
}

void CTopicFrame::OnItemHover(int itemIndex)
{
	map<string, vector<CTopicInfo>>::iterator itTopic = m_mTopics.begin();
	std::advance(itTopic, itemIndex);
	if(itTopic != m_mTopics.end())
		m_mTopicsState[itTopic->first] = eHover;
}

void CTopicFrame::OnEventCallback(IGUIManager::ENUM_EVENT nEvent, CGUIWidget* pWidget, int x, int y)
{
	switch (nEvent) {
	case IGUIManager::EVENT_OUTSIDE:
	case IGUIManager::EVENT_NONE:
	case IGUIManager::EVENT_MOUSEMOVE:
	{
		//ostringstream oss;
		//oss << "Mouse position : (" << x << ", " << y << ")";
		CTopicFrame* pTopicFrame = dynamic_cast<CTopicFrame*>(pWidget);
		if (pTopicFrame) {
			int index = pTopicFrame->GetTopicIndexFromY(y);
			//oss << ", index de l'item sélectionné : " << index;
			//pTopicFrame->m_oGUIManager.Print(oss.str(), 500, 300);
			if(index < pTopicFrame->m_mTopics.size())
				pTopicFrame->OnItemHover(pTopicFrame->GetTopicIndexFromY(y));
		}
	}
	break;
	case IGUIManager::EVENT_LMOUSECLICK: {
		CTopicFrame* pTopicFrame = dynamic_cast<CTopicFrame*>(pWidget);
		if (pTopicFrame) {
			int index = pTopicFrame->GetTopicIndexFromY(y);
			if (index < pTopicFrame->m_mTopics.size())
				pTopicFrame->OnItemSelected(index);
		}
			
		break;	
	}
	case IGUIManager::EVENT_LMOUSERELEASED: {
		CTopicFrame* pTopicFrame = dynamic_cast<CTopicFrame*>(pWidget);
		if (pTopicFrame) {
			int index = pTopicFrame->GetTopicIndexFromY(y);
			if (index < pTopicFrame->m_mTopics.size())
				pTopicFrame->OnItemRelease(index);
		}

		break;
	}
	default:
		break;
	}
}
