#pragma once

struct SurfacePhysicsParams_t {
	float friction;
	float elasticity;
	float density;
	float thickness;
	float dampening;
};

struct SurfaceAudioParams_t {
	float reflectivity; // like elasticity, but how much sound should be reflected by this surface
	float hardnessFactor; // like elasticity, but only affects impact sound choices
	float roughnessFactor; // like friction, but only affects scrape sound choices   
	float roughThreshold; // surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
	float hardThreshold; // surface hardness > this causes "hard" impacts, < this causes "soft" impacts
	float hardVelocityThreshold; // collision velocity > this causes "hard" impacts, < this causes "soft" impacts   
	float highPitchOcclusion;
	//a value betweeen 0 and 100 where 0 is not occluded at all and 100 is silent (except for any additional reflected sound)
	float midPitchOcclusion;
	float lowPitchOcclusion;
};

struct SurfaceSoundNames_t {
	unsigned short walkstepleft;
	unsigned short walkstepright;
	unsigned short runstepleft;
	unsigned short runstepright;
	unsigned short impactsoft;
	unsigned short impacthard;
	unsigned short scrapesmooth;
	unsigned short scraperough;
	unsigned short bulletimpact;
	unsigned short rolling;
	unsigned short breaksound;
	unsigned short strainsound;
};

struct SurfaceSoundHandles_t {
	short walkstepleft;
	short walkstepright;
	short runstepleft;
	short runstepright;
	short impactsoft;
	short impacthard;
	short scrapesmooth;
	short scraperough;
	short bulletimpact;
	short rolling;
	short breaksound;
	short strainsound;
};

struct SurfaceGameProps_t {
	float maxspeedfactor;
	float jumpfactor;
	float penetrationmodifier;
	float damagemodifier;
	uint16_t material;
	uint8_t climbable;
};

struct SurfaceData_t {
	SurfacePhysicsParams_t physics;
	SurfaceAudioParams_t audio;
	SurfaceSoundNames_t sounds;
	SurfaceGameProps_t game;
};

enum TextureCharacters {
	CHAR_TEX_ANTLION = 'A',
	CHAR_TEX_BLOODYFLESH = 'B',
	CHAR_TEX_CONCRETE = 'C',
	CHAR_TEX_DIRT = 'D',
	CHAR_TEX_EGGSHELL = 'E',
	CHAR_TEX_FLESH = 'F',
	CHAR_TEX_GRATE = 'G',
	CHAR_TEX_ALIENFLESH = 'H',
	CHAR_TEX_CLIP = 'I',
	CHAR_TEX_PLASTIC = 'L',
	CHAR_TEX_METAL = 'M',
	CHAR_TEX_SAND = 'N',
	CHAR_TEX_FOLIAGE = 'O',
	CHAR_TEX_COMPUTER = 'P',
	CHAR_TEX_SLOSH = 'S',
	CHAR_TEX_TILE = 'T',
	CHAR_TEX_CARDBOARD = 'U',
	CHAR_TEX_VENT = 'V',
	CHAR_TEX_WOOD = 'W',
	CHAR_TEX_GLASS = 'Y',
	CHAR_TEX_WARPSHIELD = 'Z',
};

class IPhysicsSurfaceProps
{
public:
	virtual ~IPhysicsSurfaceProps(void) {}
	virtual int ParseSurfaceData(const char *pFilename, const char *pTextfile) = 0;
	virtual int SurfacePropCount(void) const = 0;
	virtual int GetSurfaceIndex(const char *pSurfacePropName) const = 0;
	virtual void GetPhysicsProperties(int surfaceDataIndex, float *density, float thickness, float friction, float elasticity) const = 0;
	virtual SurfaceData_t* GetSurfaceData(int surfaceDataIndex) = 0;
	virtual const char GetString(unsigned short stringTableIndex) const = 0;
	virtual const char GetPropName(int surfaceDataIndex) const = 0;
	virtual void SetWorldMaterialIndexTable(int *pMapArray, int mapSize) = 0;
	virtual void GetPhysicsParameters(int surfaceDataIndex, SurfacePhysicsParams_t *pParamsOut) const = 0;
};