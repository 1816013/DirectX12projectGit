#include "Camera.h"
#include "../../Application.h"

using namespace DirectX;
Camera::Camera()
{
	const auto wSize = Application::GetInstance().GetWindowSize();
	proj = XMMatrixPerspectiveFovRH(XM_PIDIV4, // ��p(FOV)
		static_cast<float>(wSize.width) / static_cast<float>(wSize.height),
		0.1f,	// �j�A(�߂�)
		1000.0f);	//�@�t�@�[(����)
	view = XMMatrixLookAtRH(
		{ 0.0f, 10.0f, 30.0f, 1.0f },	// ���_
		{ 0.0f, 10.0f, 0.0f, 1.0f },		// �����X
		{ 0.0f, 1.0f, 0.0f,1.0f });		// ��(���̏�)
}

Camera::~Camera() = default;


DirectX::XMMATRIX& Camera::GetCameaProj()
{
	return proj;
}

DirectX::XMMATRIX& Camera::GetCameaView()
{
	return view;
}