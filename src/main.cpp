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

#ifdef _WIN32
static const std::string PATH_SEPARATOR = "\\";
static const char* CLEAR = "cls";
static const char* PAUSE = "pause";
#else
static const std::string PATH_SEPARATOR = "/";
static const char* CLEAR = "clear";
static const char* PAUSE = "pause";
#endif
const bool PixelToFile(std::string& strFileIn);
const bool FileToPixel(std::string& strFileIn);
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
		std::getline(std::cin, strInput);
		if(strInput == "1")
		{
			while(true)
			{
				system(CLEAR);
				std::string strFileIn("");
				std::cout << "Pixel -> File" << std::endl;
				std::cout << "Input picture file path:" << std::endl;
				std::getline(std::cin, strFileIn);
				if(!PixelToFile(strFileIn))
				{
					system(PAUSE);
					continue;
				}
				system(PAUSE);
				return 0;
			}
		}
		else if(strInput == "2")
		{
			system(CLEAR);
			std::string strFileIn("");
			std::cout << "File -> Pixel" << std::endl;
			std::cout << "Input pixel text file path:" << std::endl;
			std::getline(std::cin, strFileIn);
			if(!FileToPixel(strFileIn))
			{
				system(PAUSE);
				continue;
			}
			system(PAUSE);
			return 0;
		}
		else
		{
			system(CLEAR);
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
	strFileOut += "_Pix.txt";
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
	TraceLevel(LOG_INFO, "------------------- Begin write text file -------------------");
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
	TraceLevel(LOG_INFO, "------------------- End write text file -------------------");
	stbi_image_free(data);
	return true;
}
const bool FileToPixel(std::string& strFileIn)
{
	std::ifstream iFile(strFileIn);
	iFile.open(strFileIn);
	if(!iFile.is_open())
	{
		TraceLevel(LOG_ERROR, "Text pix file = [%s] load failed", strFileIn.c_str());
		return false;
	}
	std::vector<std::string> vecBuffer;
	std::string strLine = "";
	while(!iFile.eof())
	{
		std::getline(iFile, strLine);
		vecBuffer.push_back(strLine);
		std::cout << strLine.size() << std::endl;
	}
	
	return true;
}