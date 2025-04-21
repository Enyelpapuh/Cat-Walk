#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "bone.h"
#include <functional>
#include "animdata.h"
#include "model_animation.h"

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;

	Animation(const std::string& animationPath, Model* model, const std::string& name)
		: m_Name(name), m_HasAnimation(false)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);

		if (!scene || !scene->mRootNode || scene->mNumAnimations == 0)
		{
			std::cerr << "[Animation] No se encontraron animaciones en: " << animationPath << std::endl;
			m_HasAnimation = false;
			return; // No continuar si no hay animaciones
		}

		auto animation = scene->mAnimations[0];
		m_Duration = static_cast<float>(animation->mDuration);
		m_TicksPerSecond = static_cast<int>(animation->mTicksPerSecond);

		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();

		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);

		m_HasAnimation = true;
	}

	bool HasAnimationModel() const 
	{
		return m_HasAnimation; 
	}

	~Animation() {}

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}
	bool HasAnimation() const
	{
		return !m_Bones.empty();
	}

	inline float GetTicksPerSecond() const { return m_TicksPerSecond; }
	inline float GetDuration() const { return m_Duration; }
	inline const AssimpNodeData& GetRootNode() const { return m_RootNode; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap() const { return m_BoneInfoMap; }
	inline const std::string& GetName() const { return m_Name; }

private:
	void ReadMissingBones(const aiAnimation* animation, Model& model)
	{
		int size = animation->mNumChannels;

		auto& boneInfoMap = model.GetBoneInfoMap(); // getting m_BoneInfoMap from Model class
		int& boneCount = model.GetBoneCount(); // getting the m_BoneCounter from Model class

		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));
		}

		m_BoneInfoMap = boneInfoMap;
	}

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		assert(src);

		dest.name = src->mName.data;
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}


	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	std::string m_Name;
	bool m_HasAnimation = false;
};
