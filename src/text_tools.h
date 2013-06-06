/// @Brief: functions used in text process
/// @Date: 2013Äê6ÔÂ5ÈÕ 15:15:58
/// @Author: garfieldwang

#ifndef __TEXT_TOOLS_H__
#define __TEXT_TOOLS_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <errno.h>

namespace TextTools {

void InplaceTrimLeft(std::string& strValue);

void InplaceTrimRight(std::string& strValue);

void InplaceTrim(std::string& strValue);

void Split(
	const std::string& strMain,
	char chSpliter,
	std::vector<std::string>& strList,
	bool bReserveNullString);

}

#endif /* __TEXT_TOOLS_H__ */
