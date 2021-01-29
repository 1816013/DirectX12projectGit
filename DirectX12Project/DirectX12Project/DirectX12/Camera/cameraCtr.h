#pragma once
#include <unordered_map>
#include <DirectXMath.h>

// カメラコントロールクラス
class Camera;
class CameraCtr
{
public:
	CameraCtr();
	~CameraCtr();
	void AddCamera(std::string name);
	Camera& GetCamera(std::string name = "default");
	void MoveCamera(DirectX::XMFLOAT3 vec, std::string name = "default");
private:
	std::unordered_map<std::string, Camera>cameraList_;
};

