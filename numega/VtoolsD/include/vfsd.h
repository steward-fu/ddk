

typedef int PROVIDERID

class VFsd : public VDevice
{
};

class VLocalFsd : VFsd
{
	PROVIDERID Register();
};

class VRemoteFsd : VFsd
{
};

class VCharFsd : VFsd
{
};
