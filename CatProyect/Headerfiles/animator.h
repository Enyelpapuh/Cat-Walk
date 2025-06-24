#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "animation.h"
#include "bone.h"

class Animator
{
public:
	Animator(Animation* animation)
	{
		m_CurrentTime = 0.0f;
		m_CurrentAnimation = animation;

		m_FinalBoneMatrices.reserve(100);
		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void PlayAnimation(Animation* pAnimation, bool resetIfNew = true)
	{
		if (!pAnimation) return;

		// Si era un clip diferente…
		if (pAnimation != m_CurrentAnimation)
		{
			// …guardamos el momento en que quedó el anterior:
			if (m_CurrentAnimation)
				m_TimeCache[m_CurrentAnimation] = m_CurrentTime;

			m_CurrentAnimation = pAnimation;

			// Recuperamos el instante donde lo dejamos (o 0 si nunca se reprodujo)
			auto found = m_TimeCache.find(pAnimation);
			if (found != m_TimeCache.end() && !resetIfNew)
				m_CurrentTime = found->second;
			else
				m_CurrentTime = 0.0f;               // primer run o queremos reiniciar
		}
		// Si es el mismo clip, no tocar m_CurrentTime (evita ‘rewind’)
	}


	void PlayAnimationByName(const std::string& name)
	{
		if (m_Animations.find(name) != m_Animations.end())
		{
			PlayAnimation(m_Animations[name]);
		}
	}

	void AddAnimation(Animation* animation)
	{
		m_Animations[animation->GetName()] = animation;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* bone = m_CurrentAnimation->FindBone(nodeName);

		if (bone)
		{
			bone->Update(m_CurrentTime);
			nodeTransform = bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		const auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		auto it = boneInfoMap.find(nodeName);
		if (it != boneInfoMap.end())
		{
			int index = it->second.id;
			glm::mat4 offset = it->second.offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}
	bool HasAnimation() const
	{
		return m_CurrentAnimation && m_CurrentAnimation->HasAnimation();
	}

private:
	std::unordered_map<const Animation*, float> m_TimeCache;
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	std::map<std::string, Animation*> m_Animations;
	float m_CurrentTime;
	float m_DeltaTime;


};
