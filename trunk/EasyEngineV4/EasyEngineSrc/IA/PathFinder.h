#pragma once

#ifndef PATHFINDER_H
#define PATHFINDER_H


#include "IPathFinder.h"
#include <map>
#include <set>

using namespace std;

class CGrid : public IGrid
{
public:
	class CCell : public ICell
	{
	public:
		CCell();
		void AddNodeFlag(TCell n);
		void RemoveNodeFlag(TCell n);
		ICell* GetParent();
		int GetFCost() const;
		int GetGCost() const;
		int GetHCost() const;
		int ComputeCoast(int x, int y);
		int ComputeCoastToParent();
		void Update(CCell* parent, CCell* destination);
		void SetParent(CCell* parent);
		bool IsInTheSameCell(CCell& cell);
		void GetCoordinates(int& row, int& column) const;
		void SetCoordinates(int row, int column);
		TCell GetCellType() const;
		void Init(CCell& cell);
		void Reset();
		void ResetAllExceptObstacles();
		CCell& operator >> (ofstream& stream);
		CCell& operator << (ifstream& stream);
		
	private:
		int m_column;
		int m_row;
		CCell* m_pParent;
		int m_nGCost;
		int m_nHCost;
		int m_nFCost;
		TCell m_eCellType;
	};

	CGrid(int rowCount, int columnCount);
	void AddObstacle(int row, int column);
	void RemoveObstacle(int row, int column);
	void SetDepart(int x, int y);
	void SetDestination(int x, int y);
	IGrid::ICell* GetDepart();
	IGrid::ICell* GetDestination();
	void FindPath();
	int RowCount() const;
	int ColumnCount() const;
	void ProcessNode(int row, int column);
	void BuildPath();
	void GetOpenList(vector<ICell*>& openList);
	void GetCloseList(vector<ICell*>& closeList);
	void GetPath(vector<ICell*>& path);
	ICell& GetCell(int row, int column);
	int GetNodeInOpenList(int row, int column);
	void UpdateOpenList(CCell& cell);
	void Reset();
	void ResetAllExceptObstacles();
	void Save(string sFileName);
	void Load(string sFileName);
	void SetManualMode(bool manual);
	bool GetManualMode();

private:

	void InsertToOpen(CCell* b);
	void InsertToOpenV2(CCell* b);
	void InsertToClose(CCell* b);
	void Init();

	CCell** m_grid;
	int m_nRowCount;
	int m_nColumnCount;
	CCell* m_pDepart;
	CCell* m_pDestination;
	vector<CGrid::ICell*> m_vOpenList;
	vector<CGrid::ICell*> m_vCloseList;
	vector<CGrid::ICell*> m_vPath;
	bool m_bManualMode;
};

class CPathFinder : public IPathFinder
{
public:
	CPathFinder(EEInterface& oInterface);
	IGrid* CreateGrid(int rowCount, int columnCount);
	void FindPath(IGrid* grid);
	string GetName() override;

private:

};


extern "C" _declspec(dllexport) CPathFinder* CreatePathFinder(EEInterface& oInterface);

#endif // PATHFINDER_H