#ifndef CSVREADER_H
#define CSVREADER_H

#include <stdio.h>
#include <string>
#include <map>
#include <vector>

class CCSVReader
{
	char			m_cSeparator;
	FILE*			m_pFile;
	std::string		m_sFileName;
	int				m_nLineCount;
	void			CalculLineCount();
public:
	class CEOLineException{};
	class CEOFException{};
	class Exception
	{
	public:
		std::string m_sMsg;
	};


				CCSVReader( char cSeparator );
	virtual 	~CCSVReader(void);

	void		OpenFile( std::string sCSVFileName );
	bool 		ReadCell( std::string& );
	void		ReadColumnStates( std::string sStateName );
	void		ReadRow( std::vector< std::string >&, unsigned int nMaxColumn = -1 );
	void 		ReadRows( const std::vector< std::string >& vColumnName, std::vector< std::vector< std::string > >& );
	bool 		NextLine();
	void		Close();
	int			GetLineCount()const;
	FILE*		GetFile();
	void		Rewind();
};

#endif //CSVREADER_H