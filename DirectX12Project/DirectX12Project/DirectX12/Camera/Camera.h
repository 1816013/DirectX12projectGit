#pragma once
#include <d3d12.h>
#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	~Camera();
	DirectX::XMMATRIX& GetCameaProj();
	DirectX::XMMATRIX& GetCameaView();
private:
	// �J�����s��(�r���[�s��)
	DirectX::XMMATRIX view;
	// �v���W�F�N�V�����s��(�p�[�X�y�N�e�B�u�s��or�ˉe�s��)
	DirectX::XMMATRIX proj;
};

