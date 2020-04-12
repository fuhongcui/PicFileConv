#include "CTrace.h"
#include <ctime>
#include <cstdarg>

namespace Namspace_Trace
{
using namespace Namspace_Trace;
#ifdef WIN32
const std::string DEFAULT_LOG_FILE = ".\\trace.log";
#else
const std::string DEFAULT_LOG_FILE = "./trace.log";
#endif

CTrace* CTrace::m_sInstance = NULL;
CTrace::CTrace()
: m_bInit(false)
, m_enCurrLogLevl(LOG_INVALID)
{

}
CTrace::~CTrace()
{
	CloseLogFile();
}
CTrace* CTrace::Instance()
{
	if (!m_sInstance)
	{
		m_sInstance = new CTrace();
	}
	
	return m_sInstance;
}
void CTrace::Destroy()
{
	if (m_sInstance)
	{
		delete m_sInstance;
		m_sInstance = NULL;
	}
}
std::string CTrace::GetCurrTime()
{
	time_t tt = time(NULL);
	tm* t = localtime(&tt);
	char temp[256] = { 0 };
	sprintf(temp,"%d-%02d-%02d %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	char* ch = temp;
	return ch;
}
void CTrace::CloseLogFile()
{
	if (m_file.is_open())
	{
		m_file.flush();
		m_file.close();
		m_bInit = false;
	}
}
const bool CTrace::Init(const std::string& strLogFile)
{
	if (strLogFile != m_strfilePath)
	{
		if (m_file.is_open())
		{
			CloseLogFile();
		}
	}
	else
	{
		if (m_bInit)
		{
			return true;
		}
		
	}
	m_file.open(strLogFile, std::ios::out);
	if (m_file.is_open())
	{
		m_strfilePath = strLogFile;
		m_bInit = true;
		m_enCurrLogLevl = LOG_INVALID;
		return true;
	}
	return false;
}
const std::string CTrace::GetLogLevelString(LogLevel Level)
{
	std::string str("");
	switch (Level)
	{
	case LOG_INFO:
		str = "[INFO]";
		break;
	case LOG_DEBUG:
		str = "[DEBUG]";
		break;
	case LOG_WARNING:
		str = "[WARNING]";
		break;
	case LOG_ERROR:
		str = "[ERROR]";
		break;
	default:
		break;
	}
	return str;
}
void CTrace::ToFile(const std::string& strLog)//main out
{
	m_file << strLog.c_str();
	m_file.flush();
}

void CTrace::Trace(LogLevel Level, const char* format, ...)
{
	std::string str = "";
	char cTemp[512] = { 0 };
	va_list ap;
	va_start(ap, format);
	vsnprintf(cTemp, sizeof(cTemp), format, ap);
	va_end(ap);
	str += "[";
	str += GetCurrTime();
	str += "]";
	str += GetLogLevelString(Level);
	str += cTemp;
	str += "\n";
	if (CheckLogLevl(Level))
	{
		if (!m_bInit)
		{
			if (Init(DEFAULT_LOG_FILE))
			{
				ToFile(str);
			}
		}
		else
		{
			ToFile(str);
		}
		std::cout << str;
	}
	
}

void CTrace::SetLogLevel(LogLevel enLevel)
{
	m_enCurrLogLevl = enLevel;
}

const bool CTrace::CheckLogLevl(LogLevel enLevel)
{
	bool bRet = false;
	if (m_enCurrLogLevl == LOG_INVALID)
	{
		bRet = true;
	}
	else if (m_enCurrLogLevl == enLevel)
	{
		bRet = true;
	}
	else
	{
		bRet = false;
	}
	return bRet;
}
}
