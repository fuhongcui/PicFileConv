#ifndef _CTRACE_
#define _CTRACE_
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
namespace Namspace_Trace
{
#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif
#define TraceLevel(LogLevel, format, ...)\
CTrace::Instance()->Trace(LogLevel, "[%s:%d]" format, __FILENAME__, __LINE__, __VA_ARGS__)
#define TraceDebug(format, ...)\
CTrace::Instance()->Trace("[%s:%d]" format, __FILENAME__, __LINE__, __VA_ARGS__)

enum LogLevel
{
	LOG_INFO,
	LOG_WARNING,
	LOG_DEBUG,
	LOG_ERROR,
	LOG_INVALID
};
class CTrace
{
private:
	CTrace();
public:
	~CTrace();
public:
	static CTrace* Instance();
	const bool Init(const std::string& strLogFile);
	void Trace(LogLevel enLevel, const char* format, ...);
	static void Destroy();
private:
	void ToFile(const std::string& strLog);
	void SetLogLevel(LogLevel enLevel);
	void CloseLogFile();
	std::string GetCurrTime();
	const bool CheckLogLevl(LogLevel enLevel);
	const std::string GetLogLevelString(LogLevel Level);
private:
	static CTrace*	m_sInstance;
	std::fstream	m_file;
	std::string		m_strfilePath;
	bool			m_bInit;
	LogLevel		m_enCurrLogLevl;
};

} //Namspace_Trace_CFH

#endif // !_CTRACE_

