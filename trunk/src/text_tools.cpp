#include "text_tools.h"

namespace TextTools {

void InplaceTrimLeft(std::string& strValue)
{
	size_t pos = 0;
	for (size_t i = 0; i < strValue.size(); ++i)
	{
		if (isspace((unsigned char)strValue[i]))
			++pos;
		else
			break;
	}
	if (pos > 0)
		strValue.erase(0, pos);
}

void InplaceTrimRight(std::string& strValue)
{
	size_t n = 0;
	for (size_t i = 0; i < strValue.size(); ++i)
	{
		if (isspace((unsigned char)strValue[strValue.length() - i - 1]))
			++n;
		else
			break;
	}
	if (n != 0)
		strValue.erase(strValue.length() - n);
}

void InplaceTrim(std::string& strValue)
{
	InplaceTrimRight(strValue);
	InplaceTrimLeft(strValue);
}

void Split(
	const std::string& strMain,
	char chSpliter,
	std::vector<std::string>& strList,
	bool bReserveNullString)
{
	strList.clear();

	if (strMain.empty())
		return;

	size_t nPrevPos = 0;
	size_t nPos;
	std::string strTemp;
	while ((nPos = strMain.find(chSpliter, nPrevPos)) != std::string::npos)
	{
		strTemp.assign(strMain, nPrevPos, nPos - nPrevPos);
		InplaceTrim(strTemp);
		if (bReserveNullString || !strTemp.empty())
			strList.push_back(strTemp);
		nPrevPos = nPos + 1;
	}

	strTemp.assign(strMain, nPrevPos, strMain.length() - nPrevPos);
	InplaceTrim(strTemp);
	if (bReserveNullString || !strTemp.empty())
		strList.push_back(strTemp);
}

}
