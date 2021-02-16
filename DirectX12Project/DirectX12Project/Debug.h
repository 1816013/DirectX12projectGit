#pragma once
#include <string>
#include <iostream>
#include <fstream>
class Debug
{
};
class Logger
{
public:
	static Logger& GetInstance()
	{
		static Logger logger;
		return logger;
	}
	~Logger();
	void Output(std::string log);
	void Output(std::wstring log);

	std::ofstream ofs_;
private:
	Logger();
};
