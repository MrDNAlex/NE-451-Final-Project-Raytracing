//#pragma once
//#include <vector>
//#include "Vec2.h"
//#include "Object.h"
//class ObjectNode
//{
//public:
//
//	ObjectNode* LeftNode;
//
//	ObjectNode* RightNode;
//
//	Object* Root;
//
//	std::vector<int> SegmentIndex;
//
//	Vec2 MinBound;
//
//	Vec2 MaxBound;
//
//	ObjectNode(Object* root) : MinBound(0, 0), MaxBound(0, 0)
//	{
//		SegmentIndex = std::vector<int>();
//		LeftNode = nullptr;
//		RightNode = nullptr;
//		Root = root;
//	}
//
//	~ObjectNode()
//	{
//		if (LeftNode != nullptr)
//			delete LeftNode;
//		if (RightNode != nullptr)
//			delete RightNode;
//	}
//};
//
