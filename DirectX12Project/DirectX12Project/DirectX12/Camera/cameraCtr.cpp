#include "cameraCtr.h"
#include "Camera.h"

using namespace DirectX;

CameraCtr::CameraCtr()
{
}

CameraCtr::~CameraCtr()
{
}

void CameraCtr::AddCamera(std::string name)
{
	cameraList_.emplace(name, Camera());
}

Camera& CameraCtr::GetCamera(std::string name)
{
	return cameraList_[name];
}

void CameraCtr::MoveCamera(DirectX::XMFLOAT3 vec, std::string name)
{
	auto& view = cameraList_[name].GetCameaView();
	view *= XMMatrixTranslation(vec.x, vec.y, vec.z);
}
