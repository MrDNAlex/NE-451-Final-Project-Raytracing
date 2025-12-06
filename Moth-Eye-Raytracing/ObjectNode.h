#pragma once
#include "Segment.h"
#include "ObjectBounds.h"
class ObjectNode
{
public:

	ObjectNode* LeftNode;

	ObjectNode* RightNode;

	std::vector<Segment*> Segments;

	ObjectBounds Bounds;

	ObjectNode()
	{
		Segments = std::vector<Segment*>();
		LeftNode = nullptr;
		RightNode = nullptr;
		Bounds = ObjectBounds();
	}

	~ObjectNode()
	{
		if (LeftNode != nullptr)
			delete LeftNode;
		if (RightNode != nullptr)
			delete RightNode;
	}
};