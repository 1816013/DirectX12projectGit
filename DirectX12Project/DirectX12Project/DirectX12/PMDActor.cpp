#include "PMDActor.h"
#include "../PMDLoder/PMDLoder.h"
#include <Windows.h>
#include <DirectXTex.h>
#include "../Common/StrOperater.h"
#include "PMDResource.h"
#include "../Common.h"
#include "TexManager.h"
#include "../Application.h"
#include "../PMDLoder/VMDLoder.h"
#include "Camera/Camera.h"
using namespace DirectX;
using namespace std;

namespace
{
	string texTable[] = { "bmp", "spa", "sph", "toon" };
}

PMDActor::PMDActor(ComPtr<ID3D12Device>& dev, const char* path, XMFLOAT3 pos, Camera& camera) : dev_(dev)
{
	pos_ = pos;
	//texManager_ = make_shared<TexManager>(*dev.Get());
	pmdModel_ = make_shared<PMDLoder>();
	pmdModel_->Load(path);
	pmdResource_ = make_shared<PMDResource>(*dev.Get());
	vmdMotion_ = make_shared<VMDLoder>();
	vmdMotion_->Load("Resource/VMD/ヤゴコロダンス.vmd");
	CreateVertexBufferView();
	CreateIndexBufferView();
	frame_ = 0.0f;
	camera_ = &camera;
}

PMDActor::~PMDActor()
{
}

void PMDActor::CreatePMDModelTexture()
{
	auto& modelPath = pmdModel_->GetModelPath();
	auto& paths = pmdModel_->GetTexturePaths();
	auto& toonPaths = pmdModel_->GetToonPaths();
	auto& texManager = TexManager::GetInstance();
	for (auto& tex : texTable)
	{
		if (textures_[tex].size() == 0)
		{
			textures_[tex].resize(paths.size());
		}
	}
	Textures toonBuffers(paths.size());
	Textures matBuffers(paths.size());

	for (int i = 0; i < paths.size(); ++i)
	{
		if (toonPaths[i] != "")
		{
			string strToonPath = StrOperater::GetTextureFromModelAndTexPath(modelPath, toonPaths[i]);
					
			if (!texManager.CreateTexture(
				StrOperater::GetWideStringfromString(strToonPath),
				toonBuffers[i]))
			{
				strToonPath = "Resource/PMD/toon/" + toonPaths[i];
				auto result = texManager.CreateTexture(
					StrOperater::GetWideStringfromString(strToonPath),
					toonBuffers[i]);
				assert(result);
			}
			textures_["toon"][i] = toonBuffers[i];
			
		}
		if (paths[i] == "")
		{
			continue;
		}

		auto pathVec = StrOperater::SplitFileName(paths[i]);
		for (auto& path : pathVec)
		{
			auto ext = StrOperater::GetExtension(path);
			if (ext != "spa" && ext != "sph")
			{
				ext = "bmp";
			}
			auto str = StrOperater::GetTextureFromModelAndTexPath(modelPath, path);
			auto result = texManager.CreateTexture(
				StrOperater::GetWideStringfromString(str),
				matBuffers[i]);
			
			assert(result);
			textures_[ext][i] = matBuffers[i];
		}	
	}
	
}

PMDLoder& PMDActor::GetPMDModel()
{
	return *pmdModel_;
}

PMDResource& PMDActor::GetPMDResource()
{
	return *pmdResource_;
}

const Textures& PMDActor::GetTextures(std::string key)
{
	Textures* ret = nullptr;
	if (textures_[key].size() != 0)
	{
		ret = &textures_[key];
	}
	return *ret;
}

void PMDActor::DrawModel(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	// 座標変換ヒープセット
	auto resHeap = pmdResource_->GetGroops(GroopType::TRANSFORM).descHeap_.Get();
	ID3D12DescriptorHeap* deskHeaps[] = { resHeap/*resViewHeap_.Get()*/ };
	auto heapPos = /*resViewHeap_*/resHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetDescriptorHeaps(1, deskHeaps);
	cmdList->SetGraphicsRootDescriptorTable(0, heapPos);

	// リソースバリアを設定デプスからピクセルシェーダ
	auto depthBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		pmdResource_->GetGroops(GroopType::DEPTH).resources_[0].resource,	// リソース
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 前ターゲット
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	// 後ろターゲット
	);
	cmdList->ResourceBarrier(1, &depthBarrier);
	// セルフ影ヒープセット
	auto shadowResHeap = pmdResource_->GetGroops(GroopType::DEPTH).descHeap_.Get();
	ID3D12DescriptorHeap* shadowDeskHeaps[] = { shadowResHeap/*resViewHeap_.Get()*/ };
	auto shadowHeapPos = /*resViewHeap_*/shadowResHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetDescriptorHeaps(1, shadowDeskHeaps);
	cmdList->SetGraphicsRootDescriptorTable(2, shadowHeapPos);

	
	// マテリアル&テクスチャヒープセット
	auto& material = GetPMDModel().GetMaterialData();
	uint32_t indexOffset = 0;
	auto matHeap = pmdResource_->GetGroops(GroopType::MATERIAL).descHeap_.Get();
	ID3D12DescriptorHeap* matDeskHeaps[] = { matHeap };
	cmdList->SetDescriptorHeaps(1, matDeskHeaps);
	auto materialHeapPos = matHeap->GetGPUDescriptorHandleForHeapStart();
	const auto heapSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto descNum = pmdResource_->GetGroops(GroopType::MATERIAL).types_.size();
	for (auto& m : material)
	{
		cmdList->SetGraphicsRootDescriptorTable(1, materialHeapPos);
		auto indexNum = m.indexNum;

		cmdList->DrawIndexedInstanced(
			indexNum,		// インデックス数
			instNum.x * instNum.y,				// インスタンス数
			indexOffset,	// インデックスオフセット
			0,				// 頂点オフセット
			0);				// インスタンスオフセット
		indexOffset += indexNum;
		materialHeapPos.ptr += static_cast<UINT64>(heapSize) * descNum;
	}
	// リソースバリアを設定デプスからピクセルシェーダ
	depthBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		pmdResource_->GetGroops(GroopType::DEPTH).resources_[0].resource,	// リソース
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// 前ターゲット
		D3D12_RESOURCE_STATE_DEPTH_WRITE	// 後ろターゲット
	);
	cmdList->ResourceBarrier(1, &depthBarrier);
	
}

void PMDActor::CreateVertexBufferView()
{
	const auto& vertices = pmdModel_->GetVertexData();
	vertexBuffer = CreateBuffer(vertices.size() * sizeof(vertices[0]));
	//頂点データ転送
	PMDVertex* mappedData = nullptr;
	auto result = vertexBuffer->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(vertices.begin(), vertices.end(), mappedData);
	vertexBuffer->Unmap(0, nullptr);

	// 頂点バッファビュー
	vbView_.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vbView_.StrideInBytes = sizeof(vertices[0]);
	vbView_.SizeInBytes = static_cast<UINT>(vertices.size() * sizeof(vertices[0]));
}

void PMDActor::CreateIndexBufferView()
{
	const auto& indices = pmdModel_->GetIndexData();
	indexBuffer = CreateBuffer(indices.size() * sizeof(indices[0]));
	//インデックスデータ転送
	auto forType = indices.back();
	decltype(forType)* mappedData = nullptr;
	auto result = indexBuffer->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(indices.begin(), indices.end(), mappedData);
	indexBuffer->Unmap(0, nullptr);

	// インデックスビュー
	ibView_.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = static_cast<UINT>(indices.size() * sizeof(indices[0]));
}

ComPtr<ID3D12Resource> PMDActor::CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType)
{
	ComPtr<ID3D12Resource>ret;
	CD3DX12_HEAP_PROPERTIES heapProp(heapType);

	// 確保する用途(Resource)に関する設定
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	auto result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	return ret;
}

bool PMDActor::CreateBoneBuffer()
{
	HRESULT result = S_OK;
	auto size = Common::AligndValue(sizeof(XMFLOAT4X4) * 512, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	boneBuffer_ = CreateBuffer(size);
	result = boneBuffer_->Map(0, nullptr, (void**)&mappedBone_);
	assert(SUCCEEDED(result));
	const auto& bData = pmdModel_->GetBoneData();
	for (int i = 0; i < bData.size(); ++i)
	{
		boneTable_.emplace(bData[i].name, i);
	}
	// 全部単位行列に初期化
	fill_n(mappedBone_, 512, XMMatrixIdentity());
	return true;
}

bool PMDActor::CreateBasicDescriptors()
{
	auto& transResBind = pmdResource_->GetGroops(GroopType::TRANSFORM);
	//transResBind.Init({ BuffType::CBV, BuffType::CBV });
	transResBind.AddBuffers(transformBuffer_.Get());
	transResBind.AddBuffers(boneBuffer_.Get());

	return true;
}

bool PMDActor::CreateTransformBuffer()
{

	transformBuffer_ = CreateBuffer(Common::AligndValue(sizeof(BasicMatrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
	auto wSize = Application::GetInstance().GetWindowSize();
	/*XMFLOAT4X4 tempMat = {};
	tempMat._11 = 1.0f / (static_cast<float>(wSize.width) / 2.0f);
	tempMat._22 = 1.0f / (static_cast<float>(wSize.height) / 2.0f);
	tempMat._33 = 1.0f;
	tempMat._44 = 1.0f;
	tempMat._41 = -1.0f;
	tempMat._42 = 1.0f;*/

	XMMATRIX world = XMMatrixIdentity();


	XMMATRIX trans[25] = {};
	for (int z = 0; z < 5; ++z)
	{
		for (int x = 0; x < 5; ++x)
		{
			trans[x + z * 5] = XMMatrixRotationY(XM_PI);
			trans[x + z * 5] *= XMMatrixTranslation(x * 6.0f, 0, -z * 6.0f);
		}
	}

	// 2D表示
	//tmpMat.r[0].m128_f32[0] = 1.0f / (static_cast<float>(wSize.width) / 2.0f);
	//tmpMat.r[1].m128_f32[1] = -1.0f / (static_cast<float>(wSize.height) / 2.0f);
	//tmpMat.r[3].m128_f32[0] = -1.0f;
	//tmpMat.r[3].m128_f32[1] = 1.0f;
	// ここまで2D表示

	// 3D表示
	// ワールド行列(モデル自身に対する変換)
	world *= XMMatrixRotationY(XM_PI);
	world *= XMMatrixTranslation(pos_.x, pos_.y, pos_.z);

	// カメラ行列(ビュー行列)
	XMMATRIX view = camera_->GetCameaView();/*XMMatrixLookAtRH(
		{ 0.0f, 10.0f, 30.0f, 1.0f },	// 視点
		{ 0.0f, 10.0f, 0.0f, 1.0f },		// 注視店
		{ 0.0f, 1.0f, 0.0f,1.0f });		// 上(仮の上)*/

		// プロジェクション行列(パースペクティブ行列or射影行列)
		XMMATRIX proj = camera_->GetCameaProj();//XMMatrixPerspectiveFovRH(XM_PIDIV4, // 画角(FOV)
	//	static_cast<float>(wSize.width) / static_cast<float>(wSize.height),
	//	0.1f,	// ニア(近い)
	//	1000.0f);	//　ファー(遠い)

	mappedBasicMatrix_ = make_shared<BasicMatrix>();
	// 後でいじるために開けっ放しにしておく
	transformBuffer_->Map(0, nullptr, (void**)&mappedBasicMatrix_);

	mappedBasicMatrix_->viewproj = view * proj;

	mappedBasicMatrix_->world = world;
	for (int i = 0; i < 25; ++i)
	{
		mappedBasicMatrix_->trans[i] = trans[i];
	}
	XMVECTOR plane = { 0,1,0,-0.01f };		// 平面方程式
	XMVECTOR light = { -1,1,1,0 };		// 光源行列
	mappedBasicMatrix_->lightPos = light;
	mappedBasicMatrix_->lightVP = view * proj;
	mappedBasicMatrix_->shadow = mappedBasicMatrix_->world * XMMatrixShadow(plane, light);
	mappedBasicMatrix_->disolveTop = 20.0f;
	mappedBasicMatrix_->disolveBottom = -2.0f;
	return true;
}

void PMDActor::Update(float delta)
{
	// モデルを動かす
	float move = 0.0f;
	angle_ = 0.0f;
	BYTE keyState[256];
	auto result = GetKeyboardState(keyState);
	if (keyState[VK_UP] & 0x80)
	{
		move += 20 * delta;
	}
	if (keyState[VK_DOWN] & 0x80)
	{
		move += -20 * delta;
	}
	if (keyState[VK_RIGHT] & 0x80)
	{
		angle_ += 5 * delta;
	}
	if (keyState[VK_LEFT] & 0x80)
	{
		angle_ += -5 * delta;
	}
	pos_.z += move;
	mappedBasicMatrix_->world *= XMMatrixTranslation(-pos_.x, -pos_.y, -pos_.z);
	mappedBasicMatrix_->world *= XMMatrixRotationY(angle_);
	mappedBasicMatrix_->world *= XMMatrixTranslation(pos_.x, pos_.y, pos_.z);
	mappedBasicMatrix_->world *= XMMatrixTranslation(0, 0, move);

	mappedBasicMatrix_->viewproj = camera_->GetCameaView() * camera_->GetCameaProj();

	for (int z = 0; z < instNum.y; ++z)
	{
		for (int x = 0; x < instNum.x; ++x)
		{
			mappedBasicMatrix_->trans[x + z * instNum.x] *= XMMatrixTranslation(-pos_.x, -pos_.y, -pos_.z);
			mappedBasicMatrix_->trans[x + z * instNum.x] *= XMMatrixRotationY(angle_);
			mappedBasicMatrix_->trans[x + z * instNum.x] *= XMMatrixTranslation(pos_.x, pos_.y, pos_.z);

			mappedBasicMatrix_->trans[x + z * instNum.x] *= XMMatrixTranslation(0, 0, move);
		}
	}


	XMVECTOR plane = { 0.0f,1.0f,0.0f,-0.01f };		// 平面方程式
	XMVECTOR light = { -1.0f,1.0f,1.0f,0.0f };		// 光源行列
	//mappedBasicMatrix_->shadow = mappedBasicMatrix_->world * XMMatrixShadow(plane, light);
	frame_ += delta * 30;
	UpdateBones(static_cast<int>(fmodf(frame_, vmdMotion_->GetVMDData().duration)));
	
}

BasicMatrix& PMDActor::GetBasicMarix()
{
	return *mappedBasicMatrix_;
}

D3D12_VERTEX_BUFFER_VIEW& PMDActor::GetVbView()
{
	return vbView_;
}

D3D12_INDEX_BUFFER_VIEW& PMDActor::GetIbView()
{
	return ibView_;
}

DirectX::XMINT2& PMDActor::GetInstID()
{
	return instNum;
}


bool PMDActor::CreateMaterialBufferView()
{
	// マテリアルバッファの作成
	HRESULT result = S_OK;
	auto& mats = pmdModel_->GetMaterialData();
	auto strideBytes = Common::AligndValue(sizeof(BasicMaterial), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	materialBuffer_ = CreateBuffer(mats.size() * strideBytes);
	auto gAddress = materialBuffer_->GetGPUVirtualAddress();
	uint8_t* mappedMaterial = nullptr;
	result = materialBuffer_->Map(0, nullptr, (void**)&mappedMaterial);
	assert(SUCCEEDED(result));

	auto& texMng = TexManager::GetInstance();

	auto& transResBind = pmdResource_->GetGroops(GroopType::MATERIAL);
	array<pair<string, ID3D12Resource*>, 4>texPairList;
	texPairList = { make_pair("bmp",texMng.GetDefTex(ColTexType::White).Get()),
					make_pair("sph",texMng.GetDefTex(ColTexType::White).Get()),
					make_pair("spa",texMng.GetDefTex(ColTexType::Black).Get()),
					make_pair("toon",texMng.GetDefTex(ColTexType::Grad).Get()) };
	for (int i = 0; i < mats.size(); ++i)
	{
		// マテリアル定数バッファビュー
		((BasicMaterial*)mappedMaterial)->diffuse = mats[i].diffuse;
		((BasicMaterial*)mappedMaterial)->ambient = mats[i].ambient;
		((BasicMaterial*)mappedMaterial)->speqular = mats[i].speqular;
		((BasicMaterial*)mappedMaterial)->alpha = mats[i].alpha;
		((BasicMaterial*)mappedMaterial)->speqularity = mats[i].speqularity;

		transResBind.AddBuffers(materialBuffer_.Get(), strideBytes);
		mappedMaterial += strideBytes;
		for (auto& texpair : texPairList)
		{
			ID3D12Resource* res = textures_[texpair.first][i].Get();
			if (res == nullptr)
			{
				res = texpair.second;
			}
			transResBind.AddBuffers(res);
		}
	}
	materialBuffer_->Unmap(0, nullptr);
	return true;
}

void PMDActor::UpdateBones(int currentFrameNo)
{
	const auto& bData = pmdModel_->GetBoneData();
	auto mats = pmdModel_->GetBoneMat();
	mats.resize(bData.size());
	fill(mats.begin(), mats.end(), XMMatrixIdentity());

	for (auto& motion : vmdMotion_->GetVMDData().data)
	{
		// ボーンがあるかどうか
		if (boneTable_.find(motion.first) == boneTable_.end())
		{
			continue;
		}
		
		// 今のフレーム時間よりも低いものを捜索
		auto rit = find_if(motion.second.rbegin(), motion.second.rend(),
			[currentFrameNo](const auto& v)
			{
				return v.frameNo <= currentFrameNo;
			});

		auto quaternion = XMLoadFloat4(&motion.second[0].quaternion);
		XMFLOAT3 mov(0, 0, 0);
		if (rit != motion.second.rend())
		{
			mov = rit->pos;
			quaternion = XMLoadFloat4(&rit->quaternion);
			auto it = rit.base();
			if (it != motion.second.end())
			{
				// 線形補間
				auto t = static_cast<float>((currentFrameNo - rit->frameNo)) /
					static_cast<float>((it->frameNo - rit->frameNo));
				// ベジェ補間
				t = CalucurateFromBezier(t, it->bz);

				// 補間を適用
				quaternion = XMQuaternionSlerp(quaternion, XMLoadFloat4(&it->quaternion), t);
				auto vPos = XMVectorLerp(XMLoadFloat3(&mov), XMLoadFloat3(&it->pos), t);
				XMStoreFloat3(&mov, vPos);
			}
		}
		// ボーンのインデックス番号
		auto bidx = boneTable_[motion.first];
		// ボーンの座標
		auto& bpos = bData[bidx].pos;
		// 初期化
		mats[bidx] = XMMatrixIdentity();
		// 原点に戻す
		mats[bidx] *= XMMatrixTranslation(-bpos.x, -bpos.y, -bpos.z);
		// 回転
		mats[bidx] *= XMMatrixRotationQuaternion(quaternion);
		// 元の位置へ移動
		mats[bidx] *= XMMatrixTranslation(bpos.x, bpos.y, bpos.z);
		// 移動
		mats[bidx] *= XMMatrixTranslation(mov.x, mov.y, mov.z);
	}
	RecursiveCalucurate(bData, mats, boneTable_["センター"]);
	copy(mats.begin(), mats.end(), mappedBone_);
}

float PMDActor::CalucurateFromBezier(float x, const DirectX::XMFLOAT2 bz[2], size_t n)
{
	// ベジェが直線だったらxを返す
	if (bz[0].x == bz[1].x && bz[0].y == bz[1].y)
	{
		return x;
	}
	// P0(0.0)*(1-t)^3 + 3*P1*(1-t)^2*t + 3* P2*(1-t)*t^2 + P3(1,1)*(1-t)^3
	// ---t = f(x);
	// 3*P1.x*(1-t)^2*t + 3 * P2.x*(1-t)*t^2 + (1-t)^3
	// 3*P1.x*(t - 2t^2 + t^3) + 3*P2.x*(t^2 - t^3) + t^3
	// 次数で分ける
	// t^3 = 3*P1.x + 3*P2.x +1
	// t^2 = -6*P1.x + 3*P2.x;
	// t = 3*P1.x
	// MMDのベジェの特性上t=xが近いから初期値t=x
	float t = x;
	float k3 = 3 * bz[0].x - 3 * bz[1].x + 1;	// t^3
	float k2 = -6 * bz[0].x + 3 * bz[1].x;		// t^2
	float k1 = 3 * bz[0].x;						// t

	// 誤差の定数
	const float epsilon = 0.0005f;
	for (size_t i = 0; i < n; ++i)
	{
		auto tmpt = k3 * t * t * t + k2 * t * t + k1 * t - x;
		if (abs(tmpt) < epsilon)
		{
			break;
		}
		t -= tmpt / 2;
	}
	// ここでtの近似値が求まったのでyを求めて返す
	//float y = 0.0f;
	float yk3 = 3 * bz[0].y - 3 * bz[1].y + 1;	// t^3
	float yk2 = -6 * bz[0].y + 3 * bz[1].y;		// t^2
	float yk1 = 3 * bz[0].y;					// t
	float y = yk3 * t * t * t + yk2 * t * t + yk1 * t;
	assert(y >= 0.0f && y <= 1.0f);
	return y;
}

void PMDActor::RecursiveCalucurate(const std::vector<PMDBone>& bones, std::vector<DirectX::XMMATRIX>& mats, int idx)
{
	for (auto child : bones[idx].children)
	{
		mats[child] *= mats[idx];
		RecursiveCalucurate(bones, mats, child);
	}
}