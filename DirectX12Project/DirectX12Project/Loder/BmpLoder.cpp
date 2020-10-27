#include "BmpLoder.h"
#include <cassert>
#include <Windows.h>

BmpLoder::BmpLoder(const char* filepath)
{
	assert(LoadFile(filepath));
}

Size BmpLoder::GetBmpSize()
{
	return bmpSize_;
}

const std::vector<uint8_t>& BmpLoder::GetRawData() const
{
	return rawData_;
}

bool BmpLoder::LoadFile(const char* filepath)
{
	FILE* fp = nullptr;
	errno_t err = fopen_s(&fp, filepath, "rb");
	if (err != 0)
	{
		char cerr[256];
		strerror_s(cerr, 256, err);
		OutputDebugStringA(cerr);
		return false;
	}

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fread_s(&fileHeader, 
		sizeof(fileHeader),
		sizeof(fileHeader), 1, fp);
	fread_s(&infoHeader,
		sizeof(infoHeader),
		sizeof(infoHeader), 1, fp);
	

	auto byteSizePerPixel = infoHeader.biBitCount / 8;
	rawData_.resize(byteSizePerPixel * infoHeader.biWidth * infoHeader.biHeight);
	fread_s(rawData_.data(), rawData_.size(), rawData_.size(), 1, fp);

	fclose(fp);
	bmpSize_.width = infoHeader.biWidth;
	bmpSize_.height = infoHeader.biHeight;

	return true;
}
