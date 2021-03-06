#include "pathFinder.h"

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;


CGrid::CCell::CCell()
{
	Reset();
}

void CGrid::CCell::AddNodeFlag(TCell type)
{
	int t = (int)(m_eCellType | type);
	m_eCellType = (TCell)t;
}

void CGrid::CCell::RemoveNodeFlag(TCell n)
{
	int nn = ~n;
	int t = m_eCellType & nn;
	m_eCellType = (TCell)t;
}

IGrid::ICell* CGrid::CCell::GetParent()
{
	return m_pParent;
}

int CGrid::CCell::ComputeCoast(int column, int row)
{
	int dc = abs(m_column - column);
	int dr = abs(m_row - row);
	int r = 14 * min(dc, dr) + 10 * (max(dc, dr) - min(dc, dr));
	return r;
}

int CGrid::CCell::ComputeCoastToParent()
{
	return m_pParent->ComputeCoast(m_column, m_row);
}

void CGrid::CCell::Update(CCell* parent, CCell* destination)
{
	m_pParent = parent;
	m_nGCost = parent ? parent->m_nGCost + parent->ComputeCoast(m_column, m_row) : 0;
	m_nHCost = ComputeCoast(destination->m_column, destination->m_row);
	m_nFCost = m_nGCost + m_nHCost;
}

void CGrid::CCell::SetParent(CCell* parent)
{
	m_pParent = parent;
}

bool CGrid::CCell::IsInTheSameCell(CCell& cell)
{
	return (m_column == cell.m_column) && (m_row == cell.m_row);
}

void CGrid::CCell::GetCoordinates(int& row, int& column) const
{
	column = m_column;
	row = m_row;
}

void CGrid::CCell::SetCoordinates(int row, int column)
{
	m_column = column;
	m_row = row;
}

IGrid::ICell::TCell CGrid::CCell::GetCellType() const
{
	return m_eCellType;
}

void CGrid::CCell::Init(CCell& cell)
{
	m_row = cell.m_row;
	m_column = cell.m_column;
	m_nFCost = cell.m_nFCost;
	m_nGCost = cell.m_nGCost;
	m_nHCost = cell.m_nHCost;
	m_pParent = cell.m_pParent;
	m_eCellType = cell.m_eCellType;
}

void CGrid::CCell::Reset()
{
	m_eCellType = eUninitialized;
	m_pParent = NULL;
	m_nGCost = 0;
	m_nHCost = 0;
	m_nFCost = 0;
}

void CGrid::CCell::ResetAllExceptObstacles()
{
	if (m_eCellType != eObstacle) {
		m_eCellType = eUninitialized;
		m_pParent = NULL;
		m_nGCost = 0;
		m_nHCost = 0;
		m_nFCost = 0;
	}
}

CGrid::CCell& CGrid::CCell::operator >> (ofstream& stream)
{
	int cellType = (m_eCellType & ~eOpen) & ~eClose;
	stream.write((char*)&cellType, sizeof(int));
	stream.write((char*)&m_nGCost, sizeof(int));
	stream.write((char*)&m_nHCost, sizeof(int));
	stream.write((char*)&m_nFCost, sizeof(int));
	return *this;
}

CGrid::CCell& CGrid::CCell::operator << (ifstream& stream)
{
	int cellType = eUninitialized;
	stream.read((char*)&cellType, sizeof(int));
	stream.read((char*)&m_nGCost, sizeof(int));
	stream.read((char*)&m_nHCost, sizeof(int));
	stream.read((char*)&m_nFCost, sizeof(int));
	m_eCellType = (TCell)cellType;
	return *this;
}

int CGrid::CCell::GetFCost() const
{
	return m_nFCost;
}

int CGrid::CCell::GetGCost() const
{
	return m_nGCost;
}

int CGrid::CCell::GetHCost() const
{
	return m_nHCost;
}

CGrid::CGrid(int rowCount, int columnCount) :
m_nRowCount(rowCount),
m_nColumnCount(columnCount),
m_pDepart(NULL),
m_pDestination(NULL),
m_bManualMode(false)
{
	Init();
}

void CGrid::Init()
{
	m_grid = new CCell*[m_nRowCount];
	for (int row = 0; row < m_nRowCount; row++)
		m_grid[row] = new CCell[m_nColumnCount];

	for (int row = 0; row < m_nRowCount; row++)
		for (int column = 0; column < m_nColumnCount; column++)
			m_grid[row][column].SetCoordinates(row, column);
}

void CGrid::Reset()
{
	m_vOpenList.clear();
	m_vCloseList.clear();
	m_vPath.clear();

	for (int row = 0; row < m_nRowCount; row++)
		for (int column = 0; column < m_nColumnCount; column++)
			m_grid[row][column].Reset();

	m_pDepart = NULL;
	m_pDestination = NULL;
}

void CGrid::ResetAllExceptObstacles()
{
	m_vOpenList.clear();
	m_vCloseList.clear();
	m_vPath.clear();

	for (int row = 0; row < m_nRowCount; row++)
		for (int column = 0; column < m_nColumnCount; column++)
			m_grid[row][column].ResetAllExceptObstacles();

	m_pDepart = NULL;
	m_pDestination = NULL;
}


void CGrid::Save(string sFileName)
{
	/* format
	 row_count(int), 
	 column_count(int),
	 originRow(int),
	 originColumn,	 
	 destinationRow(int),
	 destinationColumn(int),
	 Foreach row{
		Foreach column{
			type(int),
			GCost(int),
			HCost(int),
			FCost(int)
		}
	 } */

	ofstream file;
	file.open(sFileName.c_str(), ios::out | ios::binary);
	int originRow, originColumn, destinationRow, destinationColumn;
	m_pDepart->GetCoordinates(originRow, originColumn);
	m_pDestination->GetCoordinates(destinationRow, destinationColumn);
	file.write((char*)&m_nRowCount, sizeof(int));
	file.write((char*)&m_nColumnCount, sizeof(int));
	file.write((char*)&originRow, sizeof(int));
	file.write((char*)&originColumn, sizeof(int));
	file.write((char*)&destinationRow, sizeof(int));
	file.write((char*)&destinationColumn, sizeof(int));
	for (int row = 0; row < m_nRowCount; row++)
		for (int column = 0; column < m_nColumnCount; column++)
			m_grid[row][column] >> file;
	file.close();
}

void CGrid::Load(string sFileName)
{
	for (int row = 0; row < m_nRowCount; row++)
		delete m_grid[row];
	delete[] m_grid;
	m_vOpenList.clear();
	m_vCloseList.clear();
	m_vPath.clear();
	m_pDepart = NULL;
	m_pDestination = NULL;

	ifstream file;
	file.open(sFileName.c_str(), ios::in | ios::binary);
	int originRow, originColumn, destinationRow, destinationColumn;
	file.read((char*)&m_nRowCount, sizeof(int));
	file.read((char*)&m_nColumnCount, sizeof(int));
	file.read((char*)&originRow, sizeof(int));
	file.read((char*)&originColumn, sizeof(int));
	file.read((char*)&destinationRow, sizeof(int));
	file.read((char*)&destinationColumn, sizeof(int));

	Init();

	SetDepart(originColumn, originRow);
	SetDestination(destinationColumn, destinationRow);

	for (int row = 0; row < m_nRowCount; row++)
		for (int column = 0; column < m_nColumnCount; column++)
			m_grid[row][column] << file;

	file.close();
}

void CGrid::SetManualMode(bool manual)
{
	m_bManualMode = manual;
}

bool CGrid::GetManualMode()
{
	return m_bManualMode;
}

void CGrid::AddObstacle(int row, int column)
{
	m_grid[row][column].AddNodeFlag(CCell::eObstacle);
}

void CGrid::RemoveObstacle(int row, int column)
{
	m_grid[row][column].RemoveNodeFlag(CCell::eObstacle);
}

void CGrid::SetDepart(int column, int row)
{
	if(m_pDepart)
		m_pDepart->RemoveNodeFlag(CCell::eDepart);
	m_pDepart = &m_grid[row][column];
	m_pDepart->AddNodeFlag(CCell::eDepart);
	
}

void CGrid::SetDestination(int column, int row)
{
	if(m_pDestination)
		m_pDestination->RemoveNodeFlag(CCell::eArrivee);
	m_pDestination = &m_grid[row][column];
	m_pDestination->AddNodeFlag(CCell::eArrivee);
	
}

IGrid::ICell* CGrid::GetDepart()
{
	return m_pDepart;
}

IGrid::ICell* CGrid::GetDestination()
{
	return m_pDestination;
}

/*
void CGrid::GetDepart(int& x, int& y)
{
	m_oDepart.GetCoordinates(y, x);
}

void CGrid::GetDestination(int& x, int& y)
{
	m_oDestination.GetCoordinates(y, x);
}*/

void CGrid::FindPath()
{

}

int CGrid::RowCount() const
{
	return m_nRowCount;
}

int CGrid::ColumnCount() const
{
	return m_nColumnCount;
}

void CGrid::UpdateOpenList(CCell& cell)
{
	for (vector<ICell*>::iterator it = m_vOpenList.begin(); it != m_vOpenList.end(); it++) {
		CCell* c = (CCell*)(*it);
		if ( cell.IsInTheSameCell(*c)  && (cell.GetHCost() < c->GetHCost()) ) {
			c->Update((CCell*)cell.GetParent(), m_pDestination);
		}
	}
}

void CGrid::ProcessNode(int row, int column)
{
	//static int n = 32;
	CCell& current = m_grid[row][column];
	if (current.GetCellType() & IGrid::ICell::eArrivee)
		return;
	if (current.GetCellType() & ICell::eClose)
		return;
	if (m_grid[row][column].GetCellType() == IGrid::ICell::eDepart) {
		current.Update(NULL, m_pDestination);
	}
	current.RemoveNodeFlag(IGrid::ICell::eOpen);
	current.AddNodeFlag(IGrid::ICell::eClose);
	m_vCloseList.push_back(&current);
	int index = GetNodeInOpenList(row, column);
	if(index != -1)
		m_vOpenList.erase(m_vOpenList.begin() + index);
	for (int iRow = -1; iRow <= 1; iRow++) {
		for (int iColumn = -1; iColumn <= 1; iColumn++) {
			if ( (iColumn == 0) && (iRow == 0) )
				continue;
			if ( (column + iColumn < 0) || ( (column + iColumn) >= m_nColumnCount ) )
				continue;
			if ( (row + iRow < 0) || ((row + iRow) >= m_nRowCount) ) 
				continue;
			CCell& cell = m_grid[row + iRow][column + iColumn];
			if (cell.GetCellType() & CCell::eObstacle)
				continue;
			if ( (cell.GetCellType() == CCell::eUninitialized) || (cell.GetCellType() == CCell::eArrivee) ) {
				cell.Update(&current, (CCell*)m_pDestination);
				InsertToOpenV2(&cell);
			}
			else {
				CCell cell2;
				cell2.Init(cell);
				cell2.Update(&current, m_pDestination);
				if (cell2.GetFCost() < cell.GetFCost()) {
					cell.Init(cell2);
					UpdateOpenList(cell);
				}
			}			
		}
	}

	if (m_bManualMode)
		return;

	vector<ICell*>::iterator itBest = m_vOpenList.begin();
	ICell* cell = *itBest;
	m_vCloseList.push_back(cell);
	m_vOpenList.erase(itBest);
	int r, c;
	cell->GetCoordinates(r, c);
	ProcessNode(r, c);
}

void CGrid::BuildPath()
{
	ICell* cell = m_pDestination;
	while (cell) {
		m_vPath.insert(m_vPath.begin(), cell);
		cell = cell->GetParent();
	}
}

void CGrid::GetOpenList(vector<ICell*>& openList)
{
	openList = m_vOpenList;
}

void CGrid::GetCloseList(vector<ICell*>& closeList)
{
	closeList = m_vCloseList;
}

void CGrid::GetPath(vector<ICell*>& path)
{
	path = m_vPath;
}

IGrid::ICell& CGrid::GetCell(int row, int column)
{
	return m_grid[row][column];
}

int CGrid::GetNodeInOpenList(int row, int column)
{
	for (int i = 0; i < m_vOpenList.size(); i++) {
		int r, c;
		m_vOpenList[i]->GetCoordinates(r, c);
		if (r == row && c == column)
			return i;
	}
	return -1;
}

void CGrid::InsertToOpen(CCell* b)
{
	if ((b->GetCellType() == IGrid::ICell::eUninitialized) || (b->GetCellType() == IGrid::ICell::eArrivee))
		b->AddNodeFlag(IGrid::ICell::eOpen);
	for (vector<CGrid::ICell*>::iterator it = m_vOpenList.begin(); it != m_vOpenList.end(); it++) {
		CCell* pBox = static_cast<CCell*>(*it);
		if (b->GetFCost() < pBox->GetFCost()) {
			m_vOpenList.insert(it, b);
			return;
		}
	}
	m_vOpenList.push_back(b);
}

void CGrid::InsertToOpenV2(CCell* b)
{
	if ((b->GetCellType() == IGrid::ICell::eUninitialized) || (b->GetCellType() == IGrid::ICell::eArrivee))
		b->AddNodeFlag(IGrid::ICell::eOpen);
	for (vector<CGrid::ICell*>::iterator it = m_vOpenList.begin(); it != m_vOpenList.end(); it++) {
		CCell* pBox = static_cast<CCell*>(*it);
		if ( (b->GetFCost() < pBox->GetFCost() )  ||
			(b->GetFCost() == pBox->GetFCost()) && (b->GetHCost() < pBox->GetHCost()) ) {
			m_vOpenList.insert(it, b);
			return;
		}		
	}
	m_vOpenList.push_back(b);
}

void CGrid::InsertToClose(CCell* b)
{

}


CPathFinder::CPathFinder(EEInterface& oInterface)
{
}

IGrid* CPathFinder::CreateGrid(int rowCount, int columnCount)
{
	return new CGrid(rowCount, columnCount);
}

void CPathFinder::FindPath(IGrid* grid)
{
	int row, column;
	CGrid* pGrid = static_cast<CGrid*>(grid);
	pGrid->GetDepart()->GetCoordinates(row, column);
	pGrid->ProcessNode(row, column);
	pGrid->BuildPath();
}

string CPathFinder::GetName()
{
	return "PathFinder";
}

extern "C" _declspec(dllexport) CPathFinder* CreatePathFinder(EEInterface& oInterface)
{
	return new CPathFinder(oInterface);
}