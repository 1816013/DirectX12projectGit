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
	// カメラ行列(ビュー行列)
	DirectX::XMMATRIX view;
	// プロジェクション行列(パースペクティブ行列or射影行列)
	DirectX::XMMATRIX proj;
};

