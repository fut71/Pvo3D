#include "test.h"

void TestDirectXTK()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	Vector3 TestVec = Vector3(1, 2, 3);
	TestVec.Normalize();
}

void TestAssimp()
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("Coin.fbx",
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
}