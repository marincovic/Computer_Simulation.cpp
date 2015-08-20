#include"dxerr.lib"
#include<dxerr.h>

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)
{
	HRESULT hr = (x);
	if (FAILED8hr))
		{
			DXTrace(_FILE, (DWARD)_LINE_, hr, L#x, true);
		}
}
#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif
