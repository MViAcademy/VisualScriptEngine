#include "NUIE_UIItemFinder.hpp"

namespace NUIE
{

static const double SlotSnappingDistanceInPixel = 20.0;

template <class SlotType>
static SlotType FindSlotByConnPosition (NodeUIManager& uiManager, NodeUIDrawingEnvironment& env, const Point& viewPosition)
{
	SlotType foundSlot = nullptr;
	double minDistance = INF;
	const ViewBox& viewBox = uiManager.GetViewBox ();
	uiManager.EnumerateUINodes ([&] (const UINodePtr& uiNode) -> bool {
		uiNode->EnumerateUISlots<SlotType> ([&] (const SlotType& currentSlot) -> bool {
			Point slotConnPosition = viewBox.ModelToView (uiNode->GetSlotConnPosition<SlotType> (env, currentSlot->GetId ()));
			double distance = viewPosition.DistanceTo (slotConnPosition);
			if (distance < minDistance) {
				foundSlot = currentSlot;
				minDistance = distance;
			}
			return true;
		});
		return true;
	});
	if (minDistance > SlotSnappingDistanceInPixel * viewBox.GetScale ()) {
		return nullptr;
	}
	return foundSlot;
}

template <class SlotType>
static SlotType FindSlotInNode (const UINodePtr& uiNode, NodeUIManager& uiManager, NodeUIDrawingEnvironment& env, const Point& viewPosition)
{
	SlotType foundSlot = nullptr;
	const ViewBox& viewBox = uiManager.GetViewBox ();
	uiNode->EnumerateUISlots<SlotType> ([&] (const SlotType& currentSlot) -> bool {
		if (uiNode->HasSlotRect<SlotType> (env, currentSlot->GetId ())) {
			Rect slotRect = viewBox.ModelToView (uiNode->GetSlotRect<SlotType> (env, currentSlot->GetId ()));
			if (slotRect.Contains (viewPosition)) {
				foundSlot = currentSlot;
				return false;
			}
		}
		return true;
	});
	return foundSlot;
}

UINodePtr FindNodeUnderPosition (NodeUIManager& uiManager, NodeUIDrawingEnvironment& env, const Point& viewPosition)
{
	const ViewBox& viewBox = uiManager.GetViewBox ();
	UINodePtr foundNode = nullptr;
	uiManager.EnumerateUINodes ([&] (const UINodePtr& uiNode) -> bool {
		Rect nodeRect = viewBox.ModelToView (uiNode->GetNodeRect (env));
		if (nodeRect.Contains (viewPosition)) {
			if (foundNode == nullptr || foundNode->GetId () < uiNode->GetId ()) {
				foundNode = uiNode;
			}
		}
		return true;
	});
	return foundNode;
}

UIInputSlotPtr FindInputSlotUnderPosition (NodeUIManager& uiManager, NodeUIDrawingEnvironment& env, const Point& viewPosition)
{
	UIInputSlotPtr foundSlot = nullptr;
	FindItemUnderPosition (uiManager, env, viewPosition, nullptr, nullptr, [&] (const UIInputSlotPtr& inputSlot) {
		foundSlot = inputSlot;
	});
	return foundSlot;
}

UIOutputSlotPtr FindOutputSlotUnderPosition (NodeUIManager& uiManager, NodeUIDrawingEnvironment& env, const Point& viewPosition)
{
	UIOutputSlotPtr foundSlot = nullptr;
	FindItemUnderPosition (uiManager, env, viewPosition, nullptr, [&] (const UIOutputSlotPtr& inputSlot) {
		foundSlot = inputSlot;
	}, nullptr);
	return foundSlot;
}

bool FindItemUnderPosition (NodeUIManager& uiManager, NodeUIDrawingEnvironment& env, const Point& viewPosition,
							const std::function<void (UINodePtr&)>& nodeFound,
							const std::function<void (UIOutputSlotPtr&)>& outputSlotFound,
							const std::function<void (UIInputSlotPtr&)>& inputSlotFound)
{
	UINodePtr foundNode = FindNodeUnderPosition (uiManager, env, viewPosition);
	UIOutputSlotPtr foundOutputSlot = nullptr;
	UIInputSlotPtr foundInputSlot = nullptr;

	bool findItemsInsideNode = !uiManager.IsPreviewMode ();
	if (findItemsInsideNode) {
		if (foundNode != nullptr) {
			foundOutputSlot = FindSlotInNode<UIOutputSlotPtr> (foundNode, uiManager, env, viewPosition);
			if (foundOutputSlot != nullptr) {
				foundNode = nullptr;
			}
		}
		if (foundNode != nullptr) {
			foundInputSlot = FindSlotInNode<UIInputSlotPtr> (foundNode, uiManager, env, viewPosition);
			if (foundInputSlot != nullptr) {
				foundNode = nullptr;
			}
		}
	}

	if (foundNode != nullptr && nodeFound != nullptr) {
		nodeFound (foundNode);
		return true;
	}

	if (findItemsInsideNode) {
		if (foundOutputSlot == nullptr) {
			foundOutputSlot = FindSlotByConnPosition<UIOutputSlotPtr> (uiManager, env, viewPosition);
		}

		if (foundInputSlot == nullptr) {
			foundInputSlot = FindSlotByConnPosition<UIInputSlotPtr> (uiManager, env, viewPosition);
		}

		if (foundOutputSlot != nullptr && outputSlotFound != nullptr) {
			outputSlotFound (foundOutputSlot);
			return true;
		}
	
		if (foundInputSlot != nullptr && inputSlotFound != nullptr) {
			inputSlotFound (foundInputSlot);
			return true;
		}
	}

	return false;
}

}