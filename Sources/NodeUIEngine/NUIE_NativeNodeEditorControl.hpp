#ifndef NUIE_NATIVENODEEDITORCONTROL_HPP
#define NUIE_NATIVENODEEDITORCONTROL_HPP

#include "NUIE_NodeEditor.hpp"

namespace NUIE
{

class NativeNodeEditorControl
{
public:
	NativeNodeEditorControl ();
	virtual ~NativeNodeEditorControl ();

	virtual bool					Init (NodeEditor* nodeEditorPtr, void* nativeParentHandle, int x, int y, int width, int height) = 0;
	virtual void*					GetEditorNativeHandle () const = 0;
	virtual bool					IsEditorFocused () const = 0;

	virtual void					Resize (int x, int y, int width, int height) = 0;
	virtual void					Invalidate () = 0;
	virtual NUIE::DrawingContext&	GetDrawingContext () = 0;
};

using NativeNodeEditorControlPtr = std::shared_ptr<NativeNodeEditorControl>;
using NativeNodeEditorControlConstPtr = std::shared_ptr<const NativeNodeEditorControl>;

}

#endif
