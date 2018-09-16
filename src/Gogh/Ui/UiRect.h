#ifndef H_UIRECT
#define H_UIRECT

// TODO: remove from UI

struct Rect {
	int x, y, w, h;

	Rect() : x(0), y(0), w(0), h(0) {}
	Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}

	float xf() const { return static_cast<float>(x); }
	float yf() const { return static_cast<float>(y); }
	float wf() const { return static_cast<float>(w); }
	float hf() const { return static_cast<float>(h); }

	bool Contains(int _x, int _y) const {
		return _x >= x && _y >= y && _x < x + w && _y < y + h;
	}
	bool Contains(float _x, float _y) const {
		return _x >= xf() && _y >= yf() && _x < xf() + wf() && _y < yf() + hf();
	}
	bool IsNull() const {
		return x == 0 && y == 0 && w == 0 && h == 0;
	}
};

#endif // H_UIRECT
