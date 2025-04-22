#pragma once
enum eResourceUsage {
	LAG_USAGE_DEFAULT = 0,
	LAG_USAGE_IMMUTABLE = 1,
	LAG_USAGE_DYNAMIC = 2,
	LAG_USAGE_STAGING = 3
};
typedef struct LAG_TEXTURE2D_DESC {
	unsigned int             Width;
	unsigned int             Height;
	unsigned int             MipLevels;
	unsigned int             ArraySize;
	unsigned int			 Format; // this hooks into fwRenderContext::Format but we'll ignore that right now :)
	struct {
		unsigned int Count;
		unsigned int Quality;
	} SampleDesc;
	eResourceUsage			 Usage;
	unsigned int             BindFlags;
	unsigned int             CPUAccessFlags;
	unsigned int             MiscFlags;
}LAG_TEXTURE2D_DESC;

