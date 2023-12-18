#pragma once

#include "../WzFile.h"

namespace MapleLib {
	namespace WzLib {
		struct MapleNode {
			WzFile* fileParent{ nullptr };
			WzObject* object{ nullptr };
			MapleNode* parent{ nullptr };
			std::vector<MapleNode*> children{};

			MapleNode(WzObject* object) : object{ object } {}
			MapleNode(WzObject* object, MapleNode* parent) : fileParent{ parent->fileParent } {}

			void dispose() {
				for (auto& child : children) {
					child->dispose();
				}
				delete object;
			}
			MapleNode* constructChild(WzObject* data) { //void* just 
				//auto& child = children.emplace_back(data, fileParent)->parent = this;
				//child->parent = this;
				auto& child = children.emplace_back(data, this);
				return child;
			}
			void placeChild(MapleNode* node) {
				node->parent = this;
				node->fileParent = fileParent;
				children.push_back(node);
			}
		};
	}
}


