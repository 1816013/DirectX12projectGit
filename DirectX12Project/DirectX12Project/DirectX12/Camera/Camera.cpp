#include "Camera.h"
#include "../../Application.h"

using namespace DirectX;
Camera::Camera()
{
	const auto wSize = Application::GetInstance().GetWindowSize();
	proj = XMMatrixPerspectiveFovRH(XM_PIDIV4, // 画角(FOV)
		static_cast<float>(wSize.width) / static_cast<float>(wSize.height),
		0.1f,	// ニア(近い)
		1000.0f);	//　ファー(遠い)
	view = XMMatrixLookAtRH(
		{ 0.0f, 10.0f, 30.0f, 1.0f },	// 視点
		{ 0.0f, 10.0f, 0.0f, 1.0f },		// 注視店
		{ 0.0f, 1.0f, 0.0f,1.0f });		// 上(仮の上)
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