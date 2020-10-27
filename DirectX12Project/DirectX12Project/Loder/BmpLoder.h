#pragma once
#include "../Common.h"
#include <vector>
class BmpLoder
{
public:
	BmpLoder(const char* filepath);

	/// <summary>
	/// �r�b�g�}�b�v�̏c���̃T�C�Y��Ԃ�
	/// </summary>
	/// <returns>�r�b�g�}�b�v�̏c���̃T�C�Y</returns>
	Size GetBmpSize();

	/// <summary>
	/// BMP���f�[�^���擾����
	/// </summary>
	/// <returns>BMP���f�[�^</returns>
	const std::vector<uint8_t>& GetRawData()const;
private:
	bool LoadFile(const char* filepath);
	std::vector<uint8_t> rawData_;
	Size bmpSize_;


};

