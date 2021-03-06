#pragma once

#include <map>
#include <string>
#include "iscenegraph.h"
#include "imodel.h"
#include "modelskin.h"

namespace scene
{

/**
 * greebo: This object traverses the scenegraph on construction
 * counting all occurrences of each model (plus skins).
 */
class ModelBreakdown :
	public scene::NodeVisitor
{
public:
	struct ModelCount
	{
		std::size_t count;
		std::size_t polyCount;

		typedef std::map<std::string, std::size_t> SkinCountMap;
		SkinCountMap skinCount;

		ModelCount() :
			count(0)
		{}
	};

	// The map associating model names with occurrences
	typedef std::map<std::string, ModelCount> Map;

private:
	mutable Map _map;

public:
	ModelBreakdown()
	{
		_map.clear();
		GlobalSceneGraph().root()->traverseChildren(*this);
	}

	bool pre(const scene::INodePtr& node) override
	{
		// Check if this node is a model
		model::ModelNodePtr modelNode = Node_getModel(node);

		if (modelNode)
		{
			// Get the actual model from the node
			const model::IModel& model = modelNode->getIModel();

			Map::iterator found = _map.find(model.getModelPath());

			if (found == _map.end())
			{
				auto result = _map.emplace(model.getModelPath(), ModelCount());

				found = result.first;

				// Store the polycount in the map
				found->second.polyCount = model.getPolyCount();
				found->second.skinCount.clear();
			}

			// The iterator "found" is valid at this point
			// Get a shortcut reference
			ModelCount& modelCount = found->second;

			modelCount.count++;

			// Increase the skin count, check if we have a skinnable model
			auto skinned = std::dynamic_pointer_cast<SkinnedModel>(node);

			if (skinned)
			{
				std::string skinName = skinned->getSkin();

				auto foundSkin = modelCount.skinCount.find(skinName);

				if (foundSkin == modelCount.skinCount.end())
				{
					auto result = modelCount.skinCount.emplace(skinName, 0);

					foundSkin = result.first;
				}

				foundSkin->second++;
			}
		}

		return true;
	}

	// Accessor method to retrieve the entity breakdown map
	const Map& getMap() const
	{
		return _map;
	}

	std::size_t getNumSkins() const
	{
		std::set<std::string> skinMap;

		// Determine the number of distinct skins
		for (auto m = _map.begin(); m != _map.end(); ++m)
		{
			for (auto s = m->second.skinCount.begin(); s != m->second.skinCount.end(); ++s)
			{
				if (!s->first.empty())
				{
					skinMap.insert(s->first);
				}
			}
		}

		return skinMap.size();
	}

	Map::const_iterator begin() const
	{
		return _map.begin();
	}

	Map::const_iterator end() const
	{
		return _map.end();
	}
};

} // namespace
