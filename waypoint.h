#ifndef WAYPOINT_INCLUDED
#define WAYPOINT_INCLUDED

#include "D3DX8MATH.h"
const float RAND_MAX_FLOAT=RAND_MAX;

float myrand()
{
	int i=rand();
	DWORD k=abs(i);
	float f=5*k/RAND_MAX_FLOAT;
	while(f>1)
		f=f-1.0f;
	return f;
}


typedef class CWAYPOINT
{
	private:
		float minx,maxx,miny,maxy,minz,maxz;
		DWORD mint,maxt;

		DWORD current_advance;// milliseconde
		DWORD current_duration;// milliseconde 
		D3DXVECTOR3 current_vectors[4];
		DWORD LastT;

		
	public:
		void GetVector(D3DXVECTOR3*V);
		void Advance();
		CWAYPOINT(float _minx,float _maxx,float _miny,float _maxy,float _minz,float _maxz,DWORD _mint,DWORD _maxt);
		~CWAYPOINT();

} CWAYPOINT,*PCWAYPOINT;

CWAYPOINT::CWAYPOINT(float _minx,float _maxx,float _miny,float _maxy,float _minz,float _maxz,DWORD _mint,DWORD _maxt)
{
	minx=_minx;
	maxx=_maxx;
	miny=_miny;
	maxy=_maxy;
	minz=_minz;
	maxz=_maxz;
	mint=_mint;
	maxt=_maxt;
	for(DWORD i=0;i<4;i++)
	{
		current_vectors[i].x=minx+(myrand())*(maxx-minx);
		current_vectors[i].y=miny+(myrand())*(maxy-miny);
		current_vectors[i].z=minz+(myrand())*(maxz-minz);
	}

	current_duration=mint+DWORD(myrand()*(maxt-mint));
	current_advance=0;
	LastT=timeGetTime();
}

CWAYPOINT::~CWAYPOINT()
{
}

void CWAYPOINT::GetVector(D3DXVECTOR3*V)
{
	D3DXVec3CatmullRom(V,&current_vectors[0]
		                ,&current_vectors[1]
						,&current_vectors[2]
						,&current_vectors[3]
						,float(current_advance)/float(current_duration)
						);
}

void CWAYPOINT::Advance()
{
	DWORD T=timeGetTime();
	DWORD DeltaT=T-LastT;

	current_advance=DeltaT;
	if (current_advance>=current_duration)
	{
		current_advance=0;
		current_vectors[0]=current_vectors[1];
		current_vectors[1]=current_vectors[2];
		current_vectors[2]=current_vectors[3];
		
		current_vectors[3].x=minx+(myrand())*(maxx-minx);
		current_vectors[3].y=miny+(myrand())*(maxy-miny);
		current_vectors[3].z=minz+(myrand())*(maxz-minz);

		current_duration=mint+DWORD(myrand()*(maxt-mint));
		LastT=timeGetTime();
	}
}


#endif