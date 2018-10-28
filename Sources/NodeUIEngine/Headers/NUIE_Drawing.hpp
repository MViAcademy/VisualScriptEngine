#ifndef NUIE_DRAWING_HPP
#define NUIE_DRAWING_HPP

#include "NE_Serializable.hpp"
#include "NE_Checksum.hpp"
#include <string>

namespace NUIE
{

enum class HorizontalAnchor
{
	Left,
	Center,
	Right
};

enum class VerticalAnchor
{
	Top,
	Center,
	Bottom
};

class Color
{
public:
	Color ();
	Color (unsigned char r, unsigned char g, unsigned char b);

	void			Set (unsigned char newR, unsigned char newG, unsigned char newB);
	unsigned char	GetR () const;
	unsigned char	GetG () const;
	unsigned char	GetB () const;

	bool				operator== (const Color& rhs) const;
	bool				operator!= (const Color& rhs) const;

private:
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

class Pen
{
public:
	Pen (const Color& color, double thickness);

	const Color&	GetColor () const;
	double			GetThickness () const;

	bool	operator== (const Pen& rhs) const;
	bool	operator!= (const Pen& rhs) const;

private:
	Color	color;
	double	thickness;
};

class Font
{
public:
	Font (const std::wstring& family, double size);

	const std::wstring&		GetFamily () const;
	const double			GetSize () const;

	bool	operator== (const Font& rhs) const;
	bool	operator!= (const Font& rhs) const;

private:
	std::wstring	family;
	double			size;
};

void AddColorToChecksum (NE::Checksum& checksum, const Color& color);
void AddPenToChecksum (NE::Checksum& checksum, const Pen& pen);
void AddFontToChecksum (NE::Checksum& checksum, const Font& font);

NE::Stream::Status ReadColor (NE::InputStream& inputStream, Color& color);
NE::Stream::Status WriteColor (NE::OutputStream& outputStream, const Color& color);

}

#endif
