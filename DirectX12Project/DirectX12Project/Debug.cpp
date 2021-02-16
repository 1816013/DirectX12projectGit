#include "Debug.h"
#include "Common/StrOperater.h"
using namespace std;
Logger::Logger()
{
	ofs_.open("log.txt");
	ofs_ << "APP START" << endl;
}

Logger::~Logger()
{
	ofs_ << "APP END" << endl;
	ofs_.close();
}

void Logger::Output(std::string log)
{
	ofs_ << log << endl;
}

void Logger::Output(std::wstring log)
{
	ofs_ << StrOperater::GetStringfromWideString(log) << endl;
}

