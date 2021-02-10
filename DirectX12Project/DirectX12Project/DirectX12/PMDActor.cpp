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
	vmdMotion_->Load("Resource/VMD/���S�R���_���X.vmd");
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
	// ���W�ϊ��q�[�v�Z�b�g
	auto resHeap = pmdResource_->GetGroops(GroopType::TRANSFORM).descHeap_.Get();
	ID3D12DescriptorHeap* deskHeaps[] = { resHeap/*resViewHeap_.Get()*/ };
	auto heapPos = /*resViewHeap_*/resHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetDescriptorHeaps(1, deskHeaps);
	cmdList->SetGraphicsRootDescriptorTable(0, heapPos);

	// ���\�[�X�o���A��ݒ�f�v�X����s�N�Z���V�F�[�_
	auto depthBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		pmdResource_->GetGroops(GroopType::DEPTH).resources_[0].resource,	// ���\�[�X
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// �O�^�[�Q�b�g
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	// ���^�[�Q�b�g
	);
	cmdList->ResourceBarrier(1, &depthBarrier);
	// �Z���t�e�q�[�v�Z�b�g
	auto shadowResHeap = pmdResource_->GetGroops(GroopType::DEPTH).descHeap_.Get();
	ID3D12DescriptorHeap* shadowDeskHeaps[] = { shadowResHeap/*resViewHeap_.Get()*/ };
	auto shadowHeapPos = /*resViewHeap_*/shadowResHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetDescriptorHeaps(1, shadowDeskHeaps);
	cmdList->SetGraphicsRootDescriptorTable(2, shadowHeapPos);

	
	// �}�e���A��&�e�N�X�`���q�[�v�Z�b�g
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
			indexNum,		// �C���f�b�N�X��
			instNum.x * instNum.y,				// �C���X�^���X��
			indexOffset,	// �C���f�b�N�X�I�t�Z�b�g
			0,				// ���_�I�t�Z�b�g
			0);				// �C���X�^���X�I�t�Z�b�g
		indexOffset += indexNum;
		materialHeapPos.ptr += static_cast<UINT64>(heapSize) * descNum;
	}
	// ���\�[�X�o���A��ݒ�f�v�X����s�N�Z���V�F�[�_
	depthBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		pmdResource_->GetGroops(GroopType::DEPTH).resources_[0].resource,	// ���\�[�X
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// �O�^�[�Q�b�g
		D3D12_RESOURCE_STATE_DEPTH_WRITE	// ���^�[�Q�b�g
	);
	cmdList->ResourceBarrier(1, &depthBarrier);
	
}

void PMDActor::CreateVertexBufferView()
{
	const auto& vertices = pmdModel_->GetVertexData();
	vertexBuffer = CreateBuffer(vertices.size() * sizeof(vertices[0]));
	//���_�f�[�^�]��
	PMDVertex* mappedData = nullptr;
	auto result = vertexBuffer->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(vertices.begin(), vertices.end(), mappedData);
	vertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[
	vbView_.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vbView_.StrideInBytes = sizeof(vertices[0]);
	vbView_.SizeInBytes = static_cast<UINT>(vertices.size() * sizeof(vertices[0]));
}

void PMDActor::CreateIndexBufferView()
{
	const auto& indices = pmdModel_->GetIndexData();
	indexBuffer = CreateBuffer(indices.size() * sizeof(indices[0]));
	//�C���f�b�N�X�f�[�^�]��
	auto forType = indices.back();
	decltype(forType)* mappedData = nullptr;
	auto result = indexBuffer->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(indices.begin(), indices.end(), mappedData);
	indexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�r���[
	ibView_.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = static_cast<UINT>(indices.size() * sizeof(indices[0]));
}

ComPtr<ID3D12Resource> PMDActor::CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType)
{
	ComPtr<ID3D12Resource>ret;
	CD3DX12_HEAP_PROPERTIES heapProp(heapType);

	// �m�ۂ���p�r(Resource)�Ɋւ���ݒ�
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
	// �S���P�ʍs��ɏ�����
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

	// 2D�\��
	//tmpMat.r[0].m128_f32[0] = 1.0f / (static_cast<float>(wSize.width) / 2.0f);
	//tmpMat.r[1].m128_f32[1] = -1.0f / (static_cast<float>(wSize.height) / 2.0f);
	//tmpMat.r[3].m128_f32[0] = -1.0f;
	//tmpMat.r[3].m128_f32[1] = 1.0f;
	// �����܂�2D�\��

	// 3D�\��
	// ���[���h�s��(���f�����g�ɑ΂���ϊ�)
	world *= XMMatrixRotationY(XM_PI);
	world *= XMMatrixTranslation(pos_.x, pos_.y, pos_.z);

	// �J�����s��(�r���[�s��)
	XMMATRIX view = camera_->GetCameaView();/*XMMatrixLookAtRH(
		{ 0.0f, 10.0f, 30.0f, 1.0f },	// ���_
		{ 0.0f, 10.0f, 0.0f, 1.0f },		// �����X
		{ 0.0f, 1.0f, 0.0f,1.0f });		// ��(���̏�)*/

		// �v���W�F�N�V�����s��(�p�[�X�y�N�e�B�u�s��or�ˉe�s��)
		XMMATRIX proj = camera_->GetCameaProj();//XMMatrixPerspectiveFovRH(XM_PIDIV4, // ��p(FOV)
	//	static_cast<float>(wSize.width) / static_cast<float>(wSize.height),
	//	0.1f,	// �j�A(�߂�)
	//	1000.0f);	//�@�t�@�[(����)

	mappedBasicMatrix_ = make_shared<BasicMatrix>();
	// ��ł����邽�߂ɊJ���������ɂ��Ă���
	transformBuffer_->Map(0, nullptr, (void**)&mappedBasicMatrix_);

	mappedBasicMatrix_->viewproj = view * proj;

	mappedBasicMatrix_->world = world;
	for (int i = 0; i < 25; ++i)
	{
		mappedBasicMatrix_->trans[i] = trans[i];
	}
	XMVECTOR plane = { 0,1,0,-0.01f };		// ���ʕ�����
	XMVECTOR light = { -1,1,1,0 };		// �����s��
	mappedBasicMatrix_->lightPos = light;
	mappedBasicMatrix_->lightVP = view * proj;
	mappedBasicMatrix_->shadow = mappedBasicMatrix_->world * XMMatrixShadow(plane, light);
	mappedBasicMatrix_->disolveTop = 20.0f;
	mappedBasicMatrix_->disolveBottom = -2.0f;
	return true;
}

void PMDActor::Update(float delta)
{
	// ���f���𓮂���
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


	XMVECTOR plane = { 0.0f,1.0f,0.0f,-0.01f };		// ���ʕ�����
	XMVECTOR light = { -1.0f,1.0f,1.0f,0.0f };		// �����s��
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
	// �}�e���A���o�b�t�@�̍쐬
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
		// �}�e���A���萔�o�b�t�@�r���[
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
		// �{�[�������邩�ǂ���
		if (boneTable_.find(motion.first) == boneTable_.end())
		{
			continue;
		}
		
		// ���̃t���[�����Ԃ����Ⴂ���̂�{��
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
				// ���`���
				auto t = static_cast<float>((currentFrameNo - rit->frameNo)) /
					static_cast<float>((it->frameNo - rit->frameNo));
				// �x�W�F���
				t = CalucurateFromBezier(t, it->bz);

				// ��Ԃ�K�p
				quaternion = XMQuaternionSlerp(quaternion, XMLoadFloat4(&it->quaternion), t);
				auto vPos = XMVectorLerp(XMLoadFloat3(&mov), XMLoadFloat3(&it->pos), t);
				XMStoreFloat3(&mov, vPos);
			}
		}
		// �{�[���̃C���f�b�N�X�ԍ�
		auto bidx = boneTable_[motion.first];
		// �{�[���̍��W
		auto& bpos = bData[bidx].pos;
		// ������
		mats[bidx] = XMMatrixIdentity();
		// ���_�ɖ߂�
		mats[bidx] *= XMMatrixTranslation(-bpos.x, -bpos.y, -bpos.z);
		// ��]
		mats[bidx] *= XMMatrixRotationQuaternion(quaternion);
		// ���̈ʒu�ֈړ�
		mats[bidx] *= XMMatrixTranslation(bpos.x, bpos.y, bpos.z);
		// �ړ�
		mats[bidx] *= XMMatrixTranslation(mov.x, mov.y, mov.z);
	}
	RecursiveCalucurate(bData, mats, boneTable_["�Z���^�["]);
	copy(mats.begin(), mats.end(), mappedBone_);
}

float PMDActor::CalucurateFromBezier(float x, const DirectX::XMFLOAT2 bz[2], size_t n)
{
	// �x�W�F��������������x��Ԃ�
	if (bz[0].x == bz[1].x && bz[0].y == bz[1].y)
	{
		return x;
	}
	// P0(0.0)*(1-t)^3 + 3*P1*(1-t)^2*t + 3* P2*(1-t)*t^2 + P3(1,1)*(1-t)^3
	// ---t = f(x);
	// 3*P1.x*(1-t)^2*t + 3 * P2.x*(1-t)*t^2 + (1-t)^3
	// 3*P1.x*(t - 2t^2 + t^3) + 3*P2.x*(t^2 - t^3) + t^3
	// �����ŕ�����
	// t^3 = 3*P1.x + 3*P2.x +1
	// t^2 = -6*P1.x + 3*P2.x;
	// t = 3*P1.x
	// MMD�̃x�W�F�̓�����t=x���߂����珉���lt=x
	float t = x;
	float k3 = 3 * bz[0].x - 3 * bz[1].x + 1;	// t^3
	float k2 = -6 * bz[0].x + 3 * bz[1].x;		// t^2
	float k1 = 3 * bz[0].x;						// t

	// �덷�̒萔
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
	// ������t�̋ߎ��l�����܂����̂�y�����߂ĕԂ�
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