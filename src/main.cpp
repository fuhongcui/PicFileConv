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

const bool PixelToFile(std::string& strFileIn);
const bool FileToPixel(std::string& strFileIn);

std::vector<std::string> Split(const std::string& str, const std::string& delim);
int main(int argc, char* argv[])
{
	
	CTrace::Instance()->Init("TraceLog.txt");
	while(true)
	{
		std::cout << "----------------------" << std::endl;
		std::cout << "1.Pixel -> File" << std::endl;
		std::cout << "2.File -> Pixel" << std::endl;
		std::cout << "----------------------" << std::endl;
		std::string strInput;
		//std::getline(std::cin, strInput);
		if(strInput == "1")
		{
			while(true)
			{
				SYS_CLEAR
				std::string strFileIn("");
				std::cout << "Pixel -> File" << std::endl;
				std::cout << "Input picture file path:" << std::endl;
				std::getline(std::cin, strFileIn);
				if(!PixelToFile(strFileIn))
				{
					SYS_PAUSE
					continue;
				}
				SYS_PAUSE
				return 0;
			}
		}
		else if(1/*strInput == "2"*/)
		{
			SYS_CLEAR
			std::string strFileIn("");
			std::cout << "File -> Pixel" << std::endl;
			std::cout << "Input pixel text file path:" << std::endl;
			//std::getline(std::cin, strFileIn);
			strFileIn = "D:\\CFH\\CFH\\PicFileConv\\bin\\c_Pix.txt";
			if(!FileToPixel(strFileIn))
			{
				SYS_PAUSE
				continue;
			}
			SYS_PAUSE
			return 0;
		}
		else
		{
			SYS_CLEAR
			continue;
		}
	}
	CTrace::Instance()->Destroy();
    return 0;
}
const bool PixelToFile(std::string& strFileIn)
{
	std::string strFileOut("");
	if(strFileIn.empty())
	{
		TraceLevel(LOG_ERROR, "Picture = [%s] load failed", strFileIn.c_str());
		return false;
	}
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
	strFileOut += ".pix.txt";
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	unsigned char* data = stbi_load(strFileIn.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		TraceLevel(LOG_ERROR, "Picture = [%s] load failed", strFileIn.c_str());
		return false;
	}
	std::ofstream oFile;
	oFile.open(strFileOut, std::ios::ate | std::ios::out);
	if (!oFile.is_open())
	{
		TraceLevel(LOG_ERROR, "FILE = [%s] open failed", strFileOut);
		return false;
	}
	TraceLevel(LOG_INFO, "------------------- Begin write text file -------------------", "");
	TraceLevel(LOG_INFO, "Picture info = [width = %d, height = %d, bit = %d]", width, height, nrChannels);
	TraceLevel(LOG_INFO, "[%s] ======> [%s] ", strFileIn.c_str(), strFileOut.c_str());
	char buf[9];
	for (int i = 0; i < height; ++i)
	{
		if (i != 0)
		{
			oFile << std::endl;
		}
		for (int j = 0; j < width; ++j)
		{
			memset(buf, 0x0, sizeof(buf));
			unsigned int* uiPix = (unsigned int*)(data + (i * width + j)*nrChannels);
			if (uiPix)
				if (nrChannels == 3)
				{
					unsigned int uiPixVal = *uiPix & 0x00ffffff;
					snprintf(buf, sizeof(buf),"0x%06X", uiPixVal);
					oFile << buf;
					
				}
			if (nrChannels == 4)
			{
				unsigned int uiPixVal = *uiPix;
				snprintf(buf, sizeof(buf), "0x%08X", uiPixVal);
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
	TraceLevel(LOG_INFO, "------------------- End write text file -------------------", "");
	stbi_image_free(data);
	return true;
}
const bool FileToPixel(std::string& strFileIn)
{
	std::ifstream iFile(strFileIn);;
	if(!iFile.is_open())
	{
		TraceLevel(LOG_ERROR, "Text pix file = [%s] load failed", strFileIn.c_str());
		return false;
	}
	std::vector<std::string> vecBuffer;
	unsigned int uiWidth = 0;
	unsigned int uiHeight = 0;
	bool bWidthValid = true;
	unsigned int uiLineIndx = 0; 
	while(!iFile.eof())
	{
		std::string strLine = "";
		std::getline(iFile, strLine);
		unsigned int uiCurrentWidth = static_cast<unsigned int>(strLine.size());
		if(uiWidth == 0)
		{
			uiWidth = uiCurrentWidth;
		}
		else
		{
			if(uiWidth != uiCurrentWidth)
			{
				TraceLevel(LOG_ERROR, "Text pix file = [%s] line = %d width invalid", strFileIn.c_str(), uiLineIndx);
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
		TraceLevel(LOG_ERROR, "Text pix file = [%s] width invalid", strFileIn.c_str());
		return false;
	}
	std::vector< std::vector<std::string> > vv;
	vv.resize(vecBuffer.size());
	for(int iHeight = 0; iHeight < vecBuffer.size(); ++iHeight)
	{
		std::string& strLine = vecBuffer[iHeight];
		std::vector<std::string> vecSplitLine = Split(strLine, "0x");
		if(vecSplitLine.empty())
		{
			continue;
		}
		vv[iHeight] = vecSplitLine;
	}
	if(vv.empty() || vv[0].empty())
	{
		TraceLevel(LOG_ERROR, "Text pix file = [%s] parse failed", strFileIn.c_str());
		return false;
	}
	unsigned int uiWriteWidth = vv[0].size();
	unsigned int uiWriteHeight = vv.size();
	unsigned int uiWriteBitCount = static_cast<unsigned int>(vv[0][0].length() / 2);
	unsigned char* pData = new unsigned char[uiWriteWidth*uiWriteHeight*uiWriteBitCount];
	for(unsigned int HeightIndex = 0; HeightIndex < uiWriteHeight; ++HeightIndex)
	{
		for(unsigned int WidthIndex = 0; WidthIndex < uiWriteWidth; ++WidthIndex)
		{
			std::string strPix = "0x" + vv[HeightIndex][WidthIndex];
			unsigned int uiPix = static_cast<unsigned int>(std::stoi(strPix, nullptr, 16));
			memcpy(pData + (HeightIndex * uiWriteWidth + WidthIndex) * uiWriteBitCount, (void*)&uiPix, uiWriteBitCount);
		}
	}
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
	strFileOut += "_pix2pic";
	if(uiWriteBitCount == 3)
	{
		strFileOut += ".jpg";
		stbi_write_jpg(strFileOut.c_str(), uiWriteWidth, uiWriteHeight, uiWriteBitCount, (void*)pData, 0);
	}
	else if(uiWriteBitCount == 4)
	{
		strFileOut += ".png";
		stbi_write_png(strFileOut.c_str(), uiWriteWidth, uiWriteHeight, uiWriteBitCount, (void*)pData, 0);
	}
	else
	{
		/* code */
	}
	
	delete[] pData;
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