#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <CTrace.h>
using namespace Namspace_Trace;

#define SYS_PAUSE do{ printf("Press Enter key to continue..."); fgetc(stdin);} while(0);
#ifdef _WIN32
static const std::string PATH_SEPARATOR = "\\";
#define SYS_CLEAR do{ system("cls");} while(0);
#else
static const std::string PATH_SEPARATOR = "/";
#define SYS_CLEAR do{ system("clear");} while(0);
#endif

const bool Picture2Text(std::string& strFileIn);
const bool Text2Picture(std::string& strFileIn);
void  ExecMenu();
void  ExecCommand(int argc, char* argv[]);
std::vector<std::string> Split(const std::string& str, const std::string& delim);

int main(int argc, char* argv[])
{
	
	CTrace::Instance()->Init("trace.txt");
	if(argc < 2)
	ExecMenu();
	else ExecCommand(argc, argv);
    CTrace::Instance()->Destroy();
    return 0;
}
void  ExecMenu()
{
	while(true)
	{
		std::cout << "----------------------" << std::endl;
		std::cout << "1.Picture -> Text" << std::endl;
		std::cout << "2.Text -> Picture" << std::endl;
		std::cout << "3.Exit" << std::endl;
		std::cout << "----------------------" << std::endl;
		std::string strInput;
		std::getline(std::cin, strInput);
		if(strInput == "1")
		{
			while(true)
			{
				SYS_CLEAR
				std::string strFileIn("");
				std::cout << "Picture -> Text" << std::endl;
				std::cout << "Input Picture File Path:" << std::endl;
				std::getline(std::cin, strFileIn);
				if(!Picture2Text(strFileIn))
				{
					SYS_PAUSE
					continue;
				}
				SYS_PAUSE
				break;
			}
		}
		else if(strInput == "2")
		{
			while (true)
			{
				SYS_CLEAR
				std::string strFileIn("");
				std::cout << "Text -> Picture" << std::endl;
				std::cout << "Input Text File Path:" << std::endl;
				std::getline(std::cin, strFileIn);
				if (!Text2Picture(strFileIn))
				{
					SYS_PAUSE
					continue;
				}
				SYS_PAUSE
				break;
			}
		}
		else if (strInput == "3")
		{
			break;
		}
		else
		{
			SYS_CLEAR
			continue;
		}
	}
}
void  ExecCommand(int argc, char* argv[])
{
	std::string strText("");
	std::string strPicture("");
	for(int i = 1; i < argc; ++i)
	{
		std::string strVar = argv[i];
		if(strVar == "-t2p" && ((i + 1) < argc))
		{
			strText = argv[i + 1];
		}
		else if(strVar == "-p2t" && ((i + 1) < argc))
		{
			strPicture = argv[i + 1];
		}
		else
		{
			/* no */
		}
	}
	if(strText.empty() && strPicture.empty())
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "Picture To Text:\t" << argv[0] << " -p2t " << "picture file path" << std::endl;
		std::cout << "Text To Picture:\t" << argv[0] << " -t2p " << "text file path" << std::endl;
	}
	if(!strPicture.empty())
	{
		(void)Picture2Text(strPicture);
	}
	if(!strText.empty())
	{
		(void)Text2Picture(strText);
	}
	return;
}
const bool Picture2Text(std::string& strFileIn)
{
	TraceLevel(LOG_INFO, "Begin read picture file", "");
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	unsigned char* data = stbi_load(strFileIn.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		TraceLevel(LOG_ERROR, "Picture = [%s] load failed, please check out your input file path", strFileIn.c_str());
		return false;
	}
	TraceLevel(LOG_INFO, "Picture = [%s] width = %d  height = %d bit = %d", strFileIn.c_str(), width, height, nrChannels);
	std::string strFileOut("");
	std::string::size_type PosBegin = strFileIn.find_last_of(PATH_SEPARATOR);
	std::string::size_type PosEnd = strFileIn.find_last_of(".");
	if (PosBegin == std::string::npos)
	{
		if (PosEnd != std::string::npos)
		{
			strFileOut = strFileIn.substr(0, PosEnd);
		}
		else
		{
			strFileOut = strFileIn;
		}
	}
	else
	{
		if (PosEnd != std::string::npos)
		{
			strFileOut = strFileIn.substr(PosBegin + 1, PosEnd - PosBegin - 1);
		}
		else
		{
			strFileOut = strFileIn.substr(PosBegin);
		}
	}
	strFileOut += "_text.txt";
	std::ofstream oFile;
	oFile.open(strFileOut, std::ios::ate | std::ios::out);
	if (!oFile.is_open())
	{
		TraceLevel(LOG_ERROR, "Output file = [%s] open failed", strFileOut);
		return false;
	}
	TraceLevel(LOG_INFO, "End read picture file", "");
	TraceLevel(LOG_INFO, "Begin write text file", "");
	TraceLevel(LOG_INFO, "[%s] ======> [%s] ", strFileIn.c_str(), strFileOut.c_str());
	char buf[128];
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			memset(buf, 0x0, sizeof(buf));
			unsigned int* uiPix = (unsigned int*)(data + (i * width + j)*nrChannels);
			if (uiPix)
				if (nrChannels == 3)
				{
					unsigned int uiPixVal = *uiPix & 0x00ffffff;
					if(j == (width - 1)) snprintf(buf, sizeof(buf), "0x%06X\n", uiPixVal);
					else snprintf(buf, sizeof(buf), "0x%06X\t", uiPixVal);
					oFile << buf;
					
				}
			if (nrChannels == 4)
			{
				unsigned int uiPixVal = *uiPix;
				if (j == (width - 1)) snprintf(buf, sizeof(buf), "0x%08X\n", uiPixVal);
				else snprintf(buf, sizeof(buf), "0x%08X\t", uiPixVal);
				oFile << buf;
			}
			else
			{
				/* code */
			}
		}
	}
	oFile.flush();
	oFile.close();
	TraceLevel(LOG_INFO, "End write text file", "");
	stbi_image_free(data);
	return true;
}
const bool Text2Picture(std::string& strFileIn)
{
	TraceLevel(LOG_INFO, "Begin read text file", "");
	TraceLevel(LOG_INFO, "Text = [%s]", strFileIn.c_str());
	std::ifstream iFile(strFileIn);;
	if(!iFile.is_open())
	{
		TraceLevel(LOG_ERROR, "Text = [%s] load failed, please check out your input file path", strFileIn.c_str());
		return false;
	}
	std::vector<std::string> vecBuffer;
	vecBuffer.reserve(2160);
	unsigned int uiWidth = 0;
	unsigned int uiHeight = 0;
	bool bWidthValid = true;
	unsigned int uiLineIndx = 0; 
	while(!iFile.eof())
	{
		std::string strLine = "";
		std::getline(iFile, strLine);
		unsigned int uiCurrentWidth = static_cast<unsigned int>(strLine.size());
		if (uiCurrentWidth == 0)
		{
			continue;
		}
		if(uiWidth == 0)
		{
			uiWidth = uiCurrentWidth;
		}
		else
		{
			if(uiWidth != uiCurrentWidth)
			{
				TraceLevel(LOG_ERROR, "Text = [%s] line = %d width invalid", strFileIn.c_str(), uiLineIndx);
				bWidthValid = false;
				break;
			}
		}
		vecBuffer.push_back(strLine);
		++uiLineIndx;
	}
	iFile.close();
	if(!bWidthValid)
	{
		TraceLevel(LOG_ERROR, "Text = [%s] width invalid", strFileIn.c_str());
		return false;
	}
	std::vector< std::vector<std::string> > vv;
	vv.resize(vecBuffer.size());
	std::size_t szLineWith = 0;
	for(std::size_t iHeight = 0; iHeight < vecBuffer.size(); ++iHeight)
	{
		std::string& strLine = vecBuffer[iHeight];
		std::vector<std::string> vecSplitLine = Split(strLine, "0x");
		std::size_t szCurrentLineWith = vecSplitLine.size();
		if(szCurrentLineWith <= 0)
		{
			continue;
		}
		if(szLineWith == 0)
		{
			szLineWith = szCurrentLineWith;
		}
		if(szLineWith != szCurrentLineWith)
		{
			TraceLevel(LOG_ERROR, "Text = [%s] line = [%u] width = [%u] invalid", strFileIn.c_str(), iHeight, szCurrentLineWith);
		}
		vv[iHeight] = vecSplitLine;
	}
	if(vv.empty() || vv[0].empty())
	{
		TraceLevel(LOG_ERROR, "Text = [%s] parse failed, please check it", strFileIn.c_str());
		return false;
	}
	TraceLevel(LOG_INFO, "End read text file", "");
	unsigned int uiWriteWidth = vv[0].size();
	unsigned int uiWriteHeight = vv.size();
	unsigned int uiWriteBitCount = static_cast<unsigned int>(vv[0][0].length() / 2);
	std::string strFileOut = "";
	std::string::size_type PosBegin = strFileIn.find_last_of(PATH_SEPARATOR);
	std::string::size_type PosEnd = strFileIn.find_last_of(".");
	if(PosBegin == std::string::npos)
	{
		if(PosEnd != std::string::npos)
		{
			strFileOut = strFileIn.substr(0, PosEnd);
		}
		else
		{
			strFileOut = strFileIn;
		}
	}
	else
	{
		if(PosEnd != std::string::npos)
		{
			strFileOut = strFileIn.substr(PosBegin + 1, PosEnd - PosBegin - 1);
		}
		else
		{
			strFileOut = strFileIn.substr(PosBegin);
		}
	}
	strFileOut += "_picture";
	if(uiWriteBitCount == 3)
	{
		strFileOut += ".jpg";
	}
	else if(uiWriteBitCount == 4)
	{
		strFileOut += ".png";
	}
	else
	{
		/* code */
	}
	TraceLevel(LOG_INFO, "Begin write picture file", "");
	TraceLevel(LOG_INFO, "Picture = [width = %d, height = %d, bit = %d]", uiWriteWidth, uiWriteHeight, uiWriteBitCount);
	TraceLevel(LOG_INFO, "[%s] ======> [%s] ", strFileIn.c_str(), strFileOut.c_str());
	unsigned char* pData = new unsigned char[uiWriteWidth*uiWriteHeight*uiWriteBitCount];
	if(!pData)
	{
		TraceLevel(LOG_ERROR, "Malloc picture buffer failed", "");
		return false;
	}
	for(unsigned int HeightIndex = 0; HeightIndex < uiWriteHeight; ++HeightIndex)
	{
		for(unsigned int WidthIndex = 0; WidthIndex < uiWriteWidth; ++WidthIndex)
		{
			std::string strPix = "0x" + vv[HeightIndex][WidthIndex];
			unsigned int uiPix = static_cast<unsigned int>(std::stoul(strPix, nullptr, 16));
			memcpy(pData + (HeightIndex * uiWriteWidth + WidthIndex) * uiWriteBitCount, (void*)&uiPix, uiWriteBitCount);
		}
	}
	if(uiWriteBitCount == 3)
	{
		stbi_write_jpg(strFileOut.c_str(), uiWriteWidth, uiWriteHeight, uiWriteBitCount, (void*)pData, 0);
	}
	else if(uiWriteBitCount == 4)
	{
		stbi_write_png(strFileOut.c_str(), uiWriteWidth, uiWriteHeight, uiWriteBitCount, (void*)pData, 0);
	}
	else
	{
		/* code */
	}
	delete[] pData;
	TraceLevel(LOG_INFO, "End write picture file", "");
	return true;
}
std::vector<std::string> Split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    std::size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}