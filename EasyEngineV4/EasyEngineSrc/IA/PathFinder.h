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
		bool IsInTheSameCell(CCell& cell);
		void GetCoordinates(int& row, int& column) const;
		void SetCellCoordinates(int row, int column);
		TCell GetCellType() const;
		void Init(CCell& cell);
		void Reset();
		
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

private:

	void InsertToOpen(CCell* b);
	void InsertToOpenV2(CCell* b);

	void InsertToClose(CCell* b);

	CCell** m_grid;
	int m_nRowCount;
	int m_nColumnCount;
	CCell* m_pDepart;
	CCell* m_pDestination;
	vector<CGrid::ICell*> m_vOpenList;
	vector<CGrid::ICell*> m_vCloseList;
	vector<CGrid::ICell*> m_vPath;
};

class CPathFinder : public IPathFinder
{
public:
	CPathFinder(const Desc& oDesc);
	IGrid* CreateGrid(int rowCount, int columnCount);
	void FindPath(IGrid* grid);


private:

};


extern "C" _declspec(dllexport) CPathFinder* CreatePathFinder(const CPathFinder::Desc& oDesc);

#endif // PATHFINDER_H