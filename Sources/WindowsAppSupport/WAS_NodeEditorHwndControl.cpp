#include "WAS_NodeEditorHwndControl.hpp"
#include "WAS_WindowsAppUtils.hpp"
#include "NE_Debug.hpp"

namespace WAS
{

class SetCaptureHandler
{
public:
	SetCaptureHandler () :
		counter (0)
	{
	
	}

	void HandleMouseDown (HWND hwnd)
	{
		if (counter == 0) {
			SetCapture (hwnd);
		}
		counter += 1;
	}

	void HandleMouseUp ()
	{
		counter -= 1;
		if (counter <= 0) { // sometimes down and up are not in pair
			ReleaseCapture ();
			counter = 0;
		}
	}

private:
	int counter;
};

static LRESULT CALLBACK NodeEditorStaticWindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static SetCaptureHandler setCaptureHandler;

	if (msg == WM_CREATE) {
		LPCREATESTRUCT createStruct = LPCREATESTRUCT (lParam);
		SetWindowLongPtr (hwnd, GWLP_USERDATA, (LONG_PTR) createStruct->lpCreateParams);
	} else if (msg == WM_DESTROY) {
		SetWindowLongPtr (hwnd, GWLP_USERDATA, NULL);
	}

	NodeEditorHwndControl* control = (NodeEditorHwndControl*) GetWindowLongPtr (hwnd, GWLP_USERDATA);
	if (control == nullptr) {
		return DefWindowProc (hwnd, msg, wParam, lParam);
	}

	NUIE::NodeEditor* nodeEditor = control->GetNodeEditor ();
	if (DBGERROR (nodeEditor == nullptr)) {
		return DefWindowProc (hwnd, msg, wParam, lParam);
	}

	switch (msg) {
		case WM_PAINT:
			control->Draw ();
			break;
		case WM_ERASEBKGND:
			return 0;
		case WM_LBUTTONDOWN:
			{
				setCaptureHandler.HandleMouseDown (hwnd);
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDown (GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Left, x, y);
			}
			break;
		case WM_MBUTTONDOWN:
			{
				setCaptureHandler.HandleMouseDown (hwnd);
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDown (GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Middle, x, y);
			}
			break;
		case WM_RBUTTONDOWN:
			{
				setCaptureHandler.HandleMouseDown (hwnd);
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDown (GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Right, x, y);
			}
			break;
		case WM_LBUTTONUP:
			{
				setCaptureHandler.HandleMouseUp ();
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseUp (GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Left, x, y);
			}
			break;
		case WM_MBUTTONUP:
			{
				setCaptureHandler.HandleMouseUp ();
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseUp (GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Middle, x, y);
			}
			break;
		case WM_RBUTTONUP:
			{
				setCaptureHandler.HandleMouseUp ();
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseUp (GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Right, x, y);
			}
			break;
		case WM_MOUSEMOVE:
			{
				SetFocus (hwnd); // before Windows 10 only the focused window catches the mouse wheel message
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseMove (GetModiferKeysFromEvent (wParam), x, y);
			}
			break;
		case WM_MOUSEWHEEL:
			{
				POINT mousePos;
				mousePos.x = GET_X_LPARAM (lParam);
				mousePos.y = GET_Y_LPARAM (lParam);
				ScreenToClient (hwnd, &mousePos);
				int delta = GET_WHEEL_DELTA_WPARAM (wParam);
				NUIE::MouseWheelRotation rotation = delta > 0 ? NUIE::MouseWheelRotation::Forward : NUIE::MouseWheelRotation::Backward;
				nodeEditor->OnMouseWheel (GetModiferKeysFromEvent (wParam), rotation, mousePos.x, mousePos.y);
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDoubleClick (GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Left, x, y);
			}
			break;
		case WM_MBUTTONDBLCLK:
			{
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDoubleClick (GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Middle, x, y);
			}
			break;
		case WM_RBUTTONDBLCLK:
			{
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDoubleClick (GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Right, x, y);
			}
			break;
		case WM_SIZE:
			{
				int newWidth = LOWORD (lParam);
				int newHeight = HIWORD (lParam);
				nodeEditor->OnResize (newWidth, newHeight);
			}
			break;
		case WM_KEYDOWN:
			{
				NUIE::Key pressedKey (NUIE::PressedKeyCode::Undefined);
				bool isControlPressed = (GetKeyState (VK_CONTROL) < 0);
				if (isControlPressed) {
					switch (wParam) {
						case 'A':
							pressedKey = NUIE::PressedKeyCode::SelectAll;
							break;
						case 'C':
							pressedKey = NUIE::PressedKeyCode::Copy;
							break;
						case 'V':
							pressedKey = NUIE::PressedKeyCode::Paste;
							break;
						case 'Z':
							pressedKey = NUIE::PressedKeyCode::Undo;
							break;
						case 'Y':
							pressedKey = NUIE::PressedKeyCode::Redo;
							break;
					}
				} else {
					switch (wParam) {
						case VK_ESCAPE:
							pressedKey = NUIE::PressedKeyCode::Escape;
							break;
						case VK_DELETE:
							pressedKey = NUIE::PressedKeyCode::Delete;
							break;
					}
				}
				if (pressedKey.IsValid ()) {
					nodeEditor->OnKeyPress (pressedKey);
				}
			}
			break;
	}

	return DefWindowProc (hwnd, msg, wParam, lParam);
}

static const short NodeListWidth = 200;

static LRESULT CALLBACK NodeEditorNodeListStaticWindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE) {
		LPCREATESTRUCT createStruct = LPCREATESTRUCT (lParam);
		SetWindowLongPtr (hwnd, GWLP_USERDATA, (LONG_PTR) createStruct->lpCreateParams);
	} else if (msg == WM_DESTROY) {
		SetWindowLongPtr (hwnd, GWLP_USERDATA, NULL);
	}

	NodeEditorNodeListHwndControl* control = (NodeEditorNodeListHwndControl*) GetWindowLongPtr (hwnd, GWLP_USERDATA);
	if (control == nullptr) {
		return DefWindowProc (hwnd, msg, wParam, lParam);
	}

	switch (msg) {
		case WM_LBUTTONUP:
			{
				ReleaseCapture ();
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				control->TreeViewEndDrag (x, y);
			}
			break;
		case WM_NOTIFY:
			{
				LPNMHDR header = (LPNMHDR) lParam;
				switch (header->code) {
				case TVN_BEGINDRAG:
					control->TreeViewBeginDrag ((LPNMTREEVIEW) lParam);
					break;
				case TVN_SELCHANGED:
					control->TreeViewSelectionChanged ((LPNMTREEVIEW) lParam);
					break;
				case NM_DBLCLK:
					control->TreeViewDoubleClick (header);
					break;
				}
			}
			break;
	}

	return DefWindowProc (hwnd, msg, wParam, lParam);
}


NodeTree::Item::Item (const std::wstring& name, const CreatorFunction& creator) :
	name (name),
	creator (creator)
{

}

NodeTree::Group::Group (const std::wstring& name) :
	name (name)
{

}

NodeTree::NodeTree ()
{

}

void NodeTree::AddItem (const std::wstring& groupName, const std::wstring& itemName, const CreatorFunction& creator)
{
	Group* group = nullptr;
	auto foundGroup = std::find_if (groups.begin (), groups.end (), [&] (const Group& group) {
		return group.name == groupName;
	});
	if (foundGroup != groups.end ()) {
		group = &*foundGroup;
	} else {
		groups.push_back (Group (groupName));
		group = &groups.back ();
	}
	group->items.push_back (Item (itemName, creator));
}

const std::vector<NodeTree::Group>& NodeTree::GetGroups () const
{
	return groups;
}

const NodeTree EmptyNodeTree;

NodeEditorHwndBasedControl::NodeEditorHwndBasedControl ()
{

}

NodeEditorHwndBasedControl::~NodeEditorHwndBasedControl ()
{

}

NodeEditorHwndControl::NodeEditorHwndControl () :
	NodeEditorHwndBasedControl (),
	nodeEditor (nullptr),
	bitmapContext (),
	nodeTree (),
	control ()
{

}

NodeEditorHwndControl::~NodeEditorHwndControl ()
{

}

bool NodeEditorHwndControl::Init (NUIE::NodeEditor* nodeEditorPtr, const NodeTree& newNodeTree, HWND parentHandle, int x, int y, int width, int height)
{
	nodeEditor = nodeEditorPtr;
	DBGASSERT (nodeEditor != nullptr);

	bool controlInited = control.Init (parentHandle, NodeEditorStaticWindowProc, L"NodeEditorHwndControl", this);
	if (DBGERROR (!controlInited)) {
		return false;
	}

	HWND hwnd = control.GetWindowHandle ();
	if (DBGERROR (hwnd == NULL)) {
		return false;
	}

	bitmapContext.Init (hwnd);
	MoveWindow (hwnd, x, y, width, height, TRUE);

	nodeTree = newNodeTree;
	return true;
}

const NodeTree& NodeEditorHwndControl::GetNodeTree () const
{
	return nodeTree;
}

HWND NodeEditorHwndControl::GetEditorHandle () const
{
	return control.GetWindowHandle ();
}

void NodeEditorHwndControl::Resize (int x, int y, int width, int height)
{
	HWND hwnd = control.GetWindowHandle ();
	if (hwnd == NULL) {
		return;
	}
	MoveWindow (hwnd, x, y, width, height, TRUE);
	if (nodeEditor != nullptr) {
		nodeEditor->OnResize (width, height);
	}
}

void NodeEditorHwndControl::Invalidate ()
{
	HWND hwnd = control.GetWindowHandle ();
	if (hwnd == NULL) {
		return;
	}
	InvalidateRect (hwnd, NULL, FALSE);
}

NUIE::DrawingContext& NodeEditorHwndControl::GetDrawingContext ()
{
	return bitmapContext;
}

NUIE::NodeEditor* NodeEditorHwndControl::GetNodeEditor ()
{
	return nodeEditor;
}

void NodeEditorHwndControl::Draw ()
{
	HWND hwnd = control.GetWindowHandle ();
	if (hwnd == NULL) {
		return;
	}
	if (nodeEditor != nullptr) {
		nodeEditor->Draw ();
	}
	bitmapContext.BlitToWindow (hwnd);
}

NodeTreeView::NodeTreeView () :
	listHandle (NULL)
{

}

NodeTreeView::~NodeTreeView ()
{

}

bool NodeTreeView::Init (HWND parentHandle, int x, int y, int width, int height)
{
	listHandle = CreateWindowEx (
		0, WC_TREEVIEW, NULL, WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS, 
		x, y, width, height, parentHandle, NULL, NULL, NULL
	);
	if (DBGERROR (listHandle == NULL)) {
		return false;
	}
	return true;
}

void NodeTreeView::Resize (int x, int y, int width, int height)
{
	if (listHandle == NULL) {
		return;
	}
	MoveWindow (listHandle, x, y, width, height, TRUE);
}

bool NodeTreeView::HasGroup (const std::wstring& group) const
{
	return groups.find (group) != groups.end ();
}

void NodeTreeView::AddGroup (const std::wstring& group)
{
	auto found = groups.find (group);
	if (DBGERROR (found != groups.end ())) {
		return;
	}

	TVINSERTSTRUCT tvInsertStruct;
	ZeroMemory (&tvInsertStruct, sizeof (TVINSERTSTRUCT));
	tvInsertStruct.hParent = NULL;
	tvInsertStruct.hInsertAfter = NULL;
	tvInsertStruct.item.mask = TVIF_TEXT | TVIF_PARAM;
	tvInsertStruct.item.pszText = (LPWSTR) group.c_str ();
	tvInsertStruct.item.cchTextMax = sizeof (tvInsertStruct.item.pszText) / sizeof (wchar_t);
	tvInsertStruct.item.lParam = (LPARAM) -1;

	HTREEITEM groupItem = (HTREEITEM) SendMessage (listHandle, TVM_INSERTITEM, 0, (LPARAM) &tvInsertStruct);
	groups.insert ({ group, groupItem });
}

void NodeTreeView::AddItem (const std::wstring& group, const std::wstring& text, LPARAM lParam)
{
	auto found = groups.find (group);
	if (DBGERROR (found == groups.end ())) {
		return;
	}

	TVINSERTSTRUCT tvInsertStruct;
	ZeroMemory (&tvInsertStruct, sizeof (TVINSERTSTRUCT));
	tvInsertStruct.hParent = found->second;
	tvInsertStruct.hInsertAfter = NULL;
	tvInsertStruct.item.mask = TVIF_TEXT | TVIF_PARAM;
	tvInsertStruct.item.pszText = (LPWSTR) text.c_str ();
	tvInsertStruct.item.cchTextMax = sizeof (tvInsertStruct.item.pszText) / sizeof (wchar_t);
	tvInsertStruct.item.lParam = lParam;
	SendMessage (listHandle, TVM_INSERTITEM, 0, (LPARAM) &tvInsertStruct);
}

void NodeTreeView::ExpandAll ()
{
	for (const auto& it : groups) {
		TreeView_Expand (listHandle, it.second, TVM_EXPAND);
	}
}

HWND NodeTreeView::GetListHandle ()
{
	return listHandle;
}

NodeEditorNodeListHwndControl::NodeEditorNodeListHwndControl () :
	NodeEditorHwndBasedControl (),
	nodeTreeView (),
	nodeEditorControl (),
	mainControl (),
	selectedNode (-1),
	draggedNode (-1)
{

}

NodeEditorNodeListHwndControl::~NodeEditorNodeListHwndControl ()
{

}

bool NodeEditorNodeListHwndControl::Init (NUIE::NodeEditor* nodeEditorPtr, const NodeTree& nodeTree, HWND parentHandle, int x, int y, int width, int height)
{
	bool controlInited = mainControl.Init (parentHandle, NodeEditorNodeListStaticWindowProc, L"NodeEditorNodeListHwndControl", this);
	if (DBGERROR (!controlInited)) {
		return false;
	}

	HWND mainHandle = mainControl.GetWindowHandle ();
	if (DBGERROR (mainHandle == NULL)) {
		return false;
	}

	MoveWindow (mainHandle, x, y, width, height, TRUE);
	nodeTreeView.Init (mainHandle, 0, 0, NodeListWidth, height);
	nodeEditorControl.Init (nodeEditorPtr, EmptyNodeTree, mainHandle, NodeListWidth, 0, width - NodeListWidth, height);

	LPARAM nextNodeId = 0;
	for (const NodeTree::Group& group : nodeTree.GetGroups ()) {
		nodeTreeView.AddGroup (group.name);
		for (const NodeTree::Item& item : group.items) {
			nodeTreeView.AddItem (group.name, item.name, nextNodeId);
			nodeIdToCreator.insert ({ nextNodeId, item.creator });
			nextNodeId++;
		}
	}
	nodeTreeView.ExpandAll ();

	return true;
}

const NodeTree& NodeEditorNodeListHwndControl::GetNodeTree () const
{
	return nodeEditorControl.GetNodeTree ();
}

HWND NodeEditorNodeListHwndControl::GetEditorHandle () const
{
	return nodeEditorControl.GetEditorHandle ();
}

void NodeEditorNodeListHwndControl::Resize (int x, int y, int width, int height)
{
	HWND mainHandle = mainControl.GetWindowHandle ();
	if (mainHandle == NULL) {
		return;
	}
	MoveWindow (mainHandle, x, y, width, height, TRUE);
	nodeTreeView.Resize (x, y, NodeListWidth, height);
	nodeEditorControl.Resize (x + NodeListWidth, y, width - NodeListWidth, height);
}

void NodeEditorNodeListHwndControl::Invalidate ()
{
	nodeEditorControl.Invalidate ();
}

NUIE::DrawingContext& NodeEditorNodeListHwndControl::GetDrawingContext ()
{
	return nodeEditorControl.GetDrawingContext ();
}

void NodeEditorNodeListHwndControl::TreeViewSelectionChanged (LPNMTREEVIEW lpnmtv)
{
	if (lpnmtv->hdr.hwndFrom != nodeTreeView.GetListHandle ()) {
		return;
	}

	selectedNode = lpnmtv->itemNew.lParam;
}

void NodeEditorNodeListHwndControl::TreeViewDoubleClick (LPNMHDR lpnmhdr)
{
	if (lpnmhdr->hwndFrom != nodeTreeView.GetListHandle ()) {
		return;
	}

	if (selectedNode == (LPARAM) -1) {
		return;
	}

	RECT clientRect;
	GetClientRect (nodeEditorControl.GetEditorHandle (), &clientRect);
	NUIE::Rect viewRect = NUIE::Rect::FromPositionAndSize (NUIE::Point (0.0, 0.0), NUIE::Size (clientRect.right - clientRect.left, clientRect.bottom - clientRect.top));
	NUIE::Point center = viewRect.GetCenter ();
	CreateNode (selectedNode, (int) center.GetX (), (int) center.GetY ());
}

void NodeEditorNodeListHwndControl::TreeViewBeginDrag (LPNMTREEVIEW lpnmtv)
{
	if (lpnmtv->hdr.hwndFrom != nodeTreeView.GetListHandle ()) {
		return;
	}

	if (lpnmtv->itemNew.lParam == (LPARAM) -1) {
		return;
	}

	HWND mainHandle = mainControl.GetWindowHandle ();

	HTREEITEM item = lpnmtv->itemNew.hItem;
	TreeView_SelectItem (lpnmtv->hdr.hwndFrom, item);
	SetCapture (mainHandle);
	SetCursor (LoadCursor (NULL, IDC_CROSS));

	draggedNode = lpnmtv->itemNew.lParam;
}

void NodeEditorNodeListHwndControl::TreeViewEndDrag (int x, int y)
{
	if (DBGERROR (draggedNode == (LPARAM) -1)) {
		return;
	}

	HWND mainHandle = mainControl.GetWindowHandle ();

	RECT editorRect;
	GetClientRect (nodeEditorControl.GetEditorHandle (), &editorRect);
	MapWindowPoints (nodeEditorControl.GetEditorHandle (), mainHandle, (LPPOINT) &editorRect, 2);

	if (x < editorRect.left || x > editorRect.right || y < editorRect.top || y > editorRect.bottom) {
		draggedNode = (LPARAM) -1;
		return;
	}

	CreateNode (draggedNode, x - editorRect.left, y - editorRect.top);
	draggedNode = (LPARAM) -1;
}

void NodeEditorNodeListHwndControl::CreateNode (LPARAM nodeId, int screenX, int screenY)
{
	auto found = nodeIdToCreator.find (nodeId);
	if (DBGERROR (found == nodeIdToCreator.end ())) {
		return;
	}

	NUIE::NodeEditor* nodeEditor = nodeEditorControl.GetNodeEditor ();
	NUIE::Point modelPosition = nodeEditor->ViewToModel (NUIE::Point (screenX, screenY));
	nodeEditor->AddNode (found->second (modelPosition));
}

}
