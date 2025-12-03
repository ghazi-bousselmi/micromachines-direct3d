#ifndef Mesh_Utils_Included

#define Mesh_Utils_Included
#define PI 3.1415f

#include "stdio.h"
#include "3dutils.h"
#include "textutil.h"
#define D3DXOVERLOADS
#include "d3dx8.h"
#include "Direct3DDeviceUtility.h"

typedef D3DVECTOR TPosition;
typedef enum { FLAG_OBSTACLE=0,FLAG_CAR=1,FLAG_TERRAIN=2,FLAG_SPOT=3,FLAG_POINT=4,
FLAG_DIRECTIONAL=5,FLAG_BALISE=6,FLAG_NOTHING=7,FLAG_FORCE_DWORD = 0x7fffffff} FLAGS;

inline float sqr(float a) {return a*a;}

typedef struct
{
	D3DVECTOR points[3];

	//n : normal et d: distance des plans constitues des cotes du triangle
	D3DVECTOR n[3];
	float d[3];

	D3DVECTOR normal; // normalisé

} FaceInfo; // peut être calculé une fois pour toute pour les objets fixes, et ce pendant
            // le chargement du Mesh

typedef enum
{
	CAR_STATE_ALIFE,
	CAR_STATE_EXPLODING,
	CAR_STATE_WIN_ANIM,
	CAR_STATE_BLOCKED,
	CAR_STATE_FINISHED,	

} CAR_STATE;

typedef struct
{
		CAR_STATE car_state;
		DWORD sub_state,LastT;
		float s1,s2;
		D3DXMATRIX matrice;
} TempType;

typedef struct
{
	DWORD NumFaces;
	FaceInfo ** faceinfo;
	D3DVECTOR center;
} FaceInfo2;


typedef struct
{
	D3DVECTOR points[8];
	float minx,maxx,miny,maxy,minz,maxz;
} Box;

typedef class MeshStruct
{	

	public:
		char*filename;
		FLAGS flag; // pour le type du mesh
		char ** texturenames;
		LPD3DXMESH mesh;
		Box BoundingBox;
		float TransformedRadius;
		D3DXMATRIX matrice; // postionnement
		D3DMATERIAL8*           MeshMaterials; 
		LPDIRECT3DTEXTURE8*     MeshTextures ; 
		DWORD                   NumMaterials;
		DWORD                   NumFaces;
		FaceInfo * faceinfo; // pour CollisionDetection
		
		HRESULT OptimizeMesh(LPDIRECT3DDEVICE8 dev8,LPD3DXBUFFER ADJbuffer);
		
		// pour optimiser CollisionDetection
		struct
		{
			bool computed;
			D3DVECTOR Center; // transformé
			float RX,RY,RZ; // les rayons du mesh transformé
			// rayons et non pas longueurs

			DWORD NCubesX,NCubesY,NCubesZ;
			// résolutuion des cubes en X,Y,Z dans le sens positifs
			// il y a donc (NCubesX * 2) cubes dans la direction des X
			// ce ne sont pas des cubes, mais des parallélipipèdes

			// les rayons des cubes dans lequels le mesh est subdivisé
			// rayons et non pas longueurs
			float CubeRayon; // transformé

			FaceInfo2 * faceinfo2;
			
		} faceinfo2;

		//int GetCubeIndex(D3DVECTOR& pos);
		void GetCubeCoords(D3DVECTOR& pos,int*X,int*Y,int*Z);
		bool RayIntersectsCube(D3DVECTOR& pos1,D3DVECTOR& pos2,int X,int Y,int Z);
		void GetCube(D3DVECTOR& pos,FaceInfo2 * faceinfo2);
		void ComputeFaceInfo2(float Rayon);
		
		
		struct
		{
			bool Locked;
			float Factor;
		} Transparency;


		struct
		{
			bool Locked;
			D3DCOLORVALUE Color;
			float Power;
		} Specular;



		
		bool loaded;
		bool LoadFormFile(char * FileName,LPDIRECT3DDEVICE8 dev8);
		bool ReloadMesh(LPDIRECT3DDEVICE8 dev8);
		bool UnLoad();
		bool Render(LPDIRECT3DDEVICE8 dev8,CDirect3DDeviceUtility * pd3dDeviceUtility);
		bool ComputeFaceInfo();
		
		void LockTransparency(float Factor);
		void UnlockTransparency();
		void LockSpecular(float R,float G,float B,float Power);
		void UnlockSpecular();
		
		MeshStruct();
		~MeshStruct();
} MeshStruct,*PMeshStruct;


class MeshManager;

typedef class CarClass
{
private:
	struct
	{
		BYTE Up,Down,Left,Right;
	} Touches; // touches de direct input
public:	
	void Accelerer(DWORD DeltaT); // milli seconde;
	void Decelerer(DWORD DeltaT); // milli seconde;
	void TournerGauche(DWORD DeltaT); // milli seconde;
	void TournerDroite(DWORD DeltaT); // milli seconde;

	DWORD balise,tour,vie;
	D3DVECTOR velocity;
	D3DVECTOR gravity;
	PMeshStruct Mesh;	
	TempType State;
	float ContactTimer;

	bool contact; // si il y a contact avec le monde

	void CheckClavier(BYTE * diks,DWORD DeltaT); // tableau de 256 valeurs
	void UpdatePosition(MeshManager * MONDE,BYTE *state); // fait les collsitions
	                 
	void SetClavier(BYTE Up,BYTE Down,BYTE Left,BYTE Right); 
	void SetMesh(PMeshStruct Mesh_);

	bool ChekOutOfViewport(D3DXMATRIX view,D3DXMATRIX Projection,
		                   D3DVIEWPORT8 viewport); 

	CarClass();
	~CarClass();
} CarClass,*PCarClass;



typedef struct
{
	FLAGS flag;
	D3DVECTOR pos,dir;
	float range,umbra,penumbra,a0,a1,a2;
	DWORD color;
}light;

typedef struct
{
	DWORD num;
	D3DVECTOR pos;
	DWORD index_next;
	D3DVECTOR N,K; 
}BALISE;

typedef class MeshManager
{
	public:
		DWORD NumMeshes,NumLights,NumBalises,NumCars;
		// pour les balises
		float alpha,d,cos_alpha,d_2_tan_alpha;
		BALISE* Balises;
		light * Lights;
		PMeshStruct * Meshes;
		PCarClass * Cars;
		void Render(LPDIRECT3DDEVICE8 dev8,int EffectiveNumCars,CDirect3DDeviceUtility * pd3dDeviceUtility);
		bool AddMeshFromFile(char * FileName,LPDIRECT3DDEVICE8 dev8);
		
		void UnloadMesh(DWORD index);
		void UnloadAll();
		bool ReloadMesh(DWORD index,LPDIRECT3DDEVICE8 dev8);
		bool ReloadAll(LPDIRECT3DDEVICE8 dev8);
		void DeleteMesh(DWORD index);
		void DeleteAll();

		void ComputeAllFaceInfo();
		void ComputeAllFaceInfo2(float Rayon);

		void CreateLights(LPDIRECT3DDEVICE8 dev8,CDirect3DDeviceUtility * pd3dDeviceUtility);
		void LockTransparency(float Factor);
		void UnlockTransparency();
		void LockSpecular(float R,float G,float B,float Power);
		void UnlockSpecular();

		bool LoadFromSTGFile(char* FileName,LPDIRECT3DDEVICE8 dev8,CDirect3DDeviceUtility * pd3dDeviceUtility);
		void ComputeBaliseInfo(float _alpha,float _d);

		void Scale(float factor,LPDIRECT3DDEVICE8 dev8);

		// pour les voitures
		void CheckClavier(DWORD EffectiveNumCars,BYTE * diks,DWORD DeltaT); // tableau de 256 valeurs
		void UpdatePosition(DWORD EffectiveNumCars,BYTE *state,DWORD DeltaT); // fait les collsitions

		
		MeshManager ();
		~MeshManager();
} MeshManager,*PMeshManager;




////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//
//       Declarations des routines
//
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////
//    Des Utilitaires de collision
/////////////////////////////////////////////////////////////////////////


float intersectRayPlane(D3DVECTOR& rOrigin, D3DVECTOR& rVector, D3DVECTOR& pOrigin, D3DVECTOR& pNormal)
 {
  
  float d = - (pNormal*pOrigin);
 
  float numer = (pNormal*rOrigin) + d;
  float denom = (pNormal*rVector);
  
  if (denom == 0)  // normal is orthogonal to vector, cant intersect
   return (-1.0f);
   
  return -(numer / denom);	
}

inline bool CheckPointInTriangle_Using_FaceInfo2(D3DVECTOR * point, FaceInfo*faceinfo)
{
	float t=faceinfo->n[0] * *point  +faceinfo->d[0];
	if(t>0)return false;
	t=faceinfo->n[1] * *point  + faceinfo->d[1];
	if(t>0)return false;
	t=faceinfo->n[2] * *point  + faceinfo->d[2];
	if(t>0)return false;

	return true;
}


#define PLANE_BACKSIDE 0x000001
#define PLANE_FRONT    0x000010
#define ON_PLANE       0x000100

DWORD classifypoint(D3DVECTOR point, D3DVECTOR pO, D3DVECTOR pN) 
{

 D3DVECTOR dir = pO - point;
 float d = dir* pN;
 
 if (d<-0.001f)
  return PLANE_FRONT;	
 else
 if (d>0.001f)
  return PLANE_BACKSIDE;	

return ON_PLANE;	
}

int MIN(int x,int y) { if(x<y) return x;else return y;}
int MAX(int x,int y) { if(x>y) return x;else return y;}

bool Sphere_Triangle_Intersect(D3DVECTOR Center,float Ray,FaceInfo * face,float *Dist)
{
	//distance between sphere and the triangle's plane is greater than the radius => less good
	*Dist=(Center-face->points[0])*face->normal;
	if(fabs(*Dist)>Ray) return false;

	//one or more vertices r inside the sphere => good
	float Ray2=Ray*Ray;
	if ((face->points[0]-Center)*(face->points[0]-Center)<=Ray2) return true;
	if ((face->points[1]-Center)*(face->points[1]-Center)<=Ray2) return true;
	if ((face->points[2]-Center)*(face->points[2]-Center)<=Ray2) return true;

	//center is inside the edges' planes => good
	float t=face->n[0] * Center  +face->d[0];
	if(t>0)goto jump;
	t=face->n[1] * Center  + face->d[1];
	if(t>0)goto jump;
	t=face->n[2] * Center  + face->d[2];
	if(t>0)goto jump;
	return true;

	//an edge intersect the sphere => good too
jump:for(char i=0;i<3;i++)
	 {
		 char next_i=i+1;
		 if(next_i==3) next_i=0;
		 float a = (face->points[next_i]-face->points[i])*(face->points[next_i]-face->points[i]);
		 float b = 2*(face->points[next_i]-face->points[i])*(face->points[i]-Center);
		 float c = (face->points[i]-Center)*(face->points[i]-Center) - Ray2;
		 float delta=b*b-4*a*c;
		 if(delta<0)continue;
		 if(delta==0)
		 {
			 float t=-b/(2*a);
			 if(t>=0 && t<=1) return true;
		 }
		 float t1=(-b-(float)sqrt(delta))/(2*a);
		 if(t1>=0 && t1<=1) return true;
		 float t2=(-b+(float)sqrt(delta))/(2*a);
		 if(t2>=0 && t2<=1) return true;
	 }

	 //that's all (rien ne va plus)
	 return false;
}

void CollisionDetection_Using_Sphere_OBSTACLE(bool *test,DWORD *mesh,D3DVECTOR& pos,D3DVECTOR next_pos,float Radius,D3DVECTOR *normale,D3DVECTOR *PIP,
					   PMeshManager MONDE,float *dist)
{
	FaceInfo2 faceinfo2;
	int X,Y,Z,X1,Y1,Z1,X2,Y2,Z2,minX,maxX,minY,maxY,minZ,maxZ;
	D3DVECTOR normale1,I1;
	float dist1;
 
	for(DWORD mesh_id=0;mesh_id<MONDE->NumMeshes;mesh_id++)
	if(MONDE->Meshes[mesh_id]->flag==FLAG_OBSTACLE)
	{		
		MONDE->Meshes[mesh_id]->GetCubeCoords(pos,&X1,&Y1,&Z1);
		MONDE->Meshes[mesh_id]->GetCubeCoords(next_pos,&X2,&Y2,&Z2);		
		
		minX=MIN(X1,X2);
		minY=MIN(Y1,Y2);
		minZ=MIN(Z1,Z2);
		maxX=MAX(X1,X2);
		maxY=MAX(Y1,Y2);
		maxZ=MAX(Z1,Z2);

		for (X=minX;X<=maxX;X++)
		{
			for (Y=minY;Y<=maxY;Y++)
			{
				for (Z=minZ;Z<=maxZ;Z++)
				{
					if (MONDE->Meshes[mesh_id]->RayIntersectsCube(pos,next_pos,X,Y,Z))
					{
						DWORD index=(X<<2)*MONDE->Meshes[mesh_id]->faceinfo2.NCubesY*MONDE->Meshes[mesh_id]->faceinfo2.NCubesZ
								   +(Y<<1)*MONDE->Meshes[mesh_id]->faceinfo2.NCubesZ
								   +Z;

						faceinfo2=MONDE->Meshes[mesh_id]->faceinfo2.faceinfo2[index];
						if ((faceinfo2.NumFaces!=0)&&(faceinfo2.faceinfo!=NULL))
						{	
							for(DWORD i=0;i<faceinfo2.NumFaces;i++)
							{
								if(Sphere_Triangle_Intersect(next_pos,Radius,faceinfo2.faceinfo[i],&dist1))
								{ 		
									normale1=faceinfo2.faceinfo[i]->normal;
									I1=next_pos-dist1*normale1;
									if(!*test)
									{
										*normale=normale1;
										*PIP=I1;
										*mesh=mesh_id;
										*dist=dist1;
										*test=true;
									}
									else if(fabs(dist1) < ~(*PIP-next_pos))
									{
										*mesh=mesh_id;
										*normale=normale1;
										*dist=dist1;
										*PIP=I1;
									}
								}
							}
						}
					}
				}
			}
		}
	}	
}



void CollisionDetection_Using_Sphere_TERRAIN(bool *test,DWORD *mesh,D3DVECTOR& pos,D3DVECTOR next_pos,float Radius,D3DVECTOR *normale,D3DVECTOR *PIP,
					   PMeshManager MONDE,float *dist)
{
	FaceInfo2 faceinfo2;
	int X,Y,Z,X1,Y1,Z1,X2,Y2,Z2,minX,maxX,minY,maxY,minZ,maxZ;
	D3DVECTOR normale1,I1;
	float dist1;
 
	for(DWORD mesh_id=0;mesh_id<MONDE->NumMeshes;mesh_id++)
	if(MONDE->Meshes[mesh_id]->flag==FLAG_TERRAIN)
	{		
		MONDE->Meshes[mesh_id]->GetCubeCoords(pos,&X1,&Y1,&Z1);
		MONDE->Meshes[mesh_id]->GetCubeCoords(next_pos,&X2,&Y2,&Z2);		
		
		minX=MIN(X1,X2);
		minY=MIN(Y1,Y2);
		minZ=MIN(Z1,Z2);
		maxX=MAX(X1,X2);
		maxY=MAX(Y1,Y2);
		maxZ=MAX(Z1,Z2);

		for (X=minX;X<=maxX;X++)
		{
			for (Y=minY;Y<=maxY;Y++)
			{
				for (Z=minZ;Z<=maxZ;Z++)
				{
					if (MONDE->Meshes[mesh_id]->RayIntersectsCube(pos,next_pos,X,Y,Z))
					{
						DWORD index=(X<<2)*MONDE->Meshes[mesh_id]->faceinfo2.NCubesY*MONDE->Meshes[mesh_id]->faceinfo2.NCubesZ
								   +(Y<<1)*MONDE->Meshes[mesh_id]->faceinfo2.NCubesZ
								   +Z;

						faceinfo2=MONDE->Meshes[mesh_id]->faceinfo2.faceinfo2[index];
						if ((faceinfo2.NumFaces!=0)&&(faceinfo2.faceinfo!=NULL))
						{	
							for(DWORD i=0;i<faceinfo2.NumFaces;i++)
							{
								if(Sphere_Triangle_Intersect(next_pos,Radius,faceinfo2.faceinfo[i],&dist1))
								{ 		
									normale1=faceinfo2.faceinfo[i]->normal;
									I1=next_pos-dist1*normale1;
									if(!*test)
									{
										*normale=normale1;
										*PIP=I1;
										*mesh=mesh_id;
										*dist=dist1;
										*test=true;
									}
									else if(fabs(dist1) < ~(*PIP-next_pos))
									{
										*mesh=mesh_id;
										*normale=normale1;
										*dist=dist1;
										*PIP=I1;
									}
								}
							}
						}
					}
				}
			}
		}
	}	
}



// used pour ComputeFaceInfo2
inline bool CollisionDetection1faceinfo(D3DVECTOR & pos,D3DVECTOR & next_pos,FaceInfo & faceinfo)
{

	D3DVECTOR delta=next_pos-pos;
	if(classifypoint(pos,faceinfo.points[0],faceinfo.normal)!=classifypoint(next_pos,faceinfo.points[0],faceinfo.normal))
	{
		float a=intersectRayPlane(pos,delta,faceinfo.points[0],faceinfo.normal);
		D3DVECTOR I=pos+a*delta;
		
		//CheckPointInTriangle
		
		float t=faceinfo.n[0] * I  +faceinfo.d[0];
		if(t>0)return false;
		t=faceinfo.n[1] * I  + faceinfo.d[1];
		if(t>0)return false;
		t=faceinfo.n[2] * I  + faceinfo.d[2];
		if(t>0)return false;
		
		return true;					
	}
	
	return false;
}


D3DVECTOR XX=D3DVEC(1,0,0)
         ,YY=D3DVEC(0,1,0)
		 ,ZZ=D3DVEC(0,0,1)
		 ,_XX_=D3DVEC(-1,0,0)
		 ,_YY_=D3DVEC(0,-1,0)
		 ,_ZZ_=D3DVEC(0,0,-1);

inline bool CheckTriangleCote_Box(FaceInfo*faceinfo,D3DVECTOR& minPoint,D3DVECTOR& maxPoint,float radius)
{
	// teste si les cotés du triangle intersecte la BOX
	D3DVECTOR vecteurs[3];
	vecteurs[0]=faceinfo->points[1]-faceinfo->points[0];
	vecteurs[1]=faceinfo->points[2]-faceinfo->points[1];
	vecteurs[2]=faceinfo->points[0]-faceinfo->points[2];
	
	float _2radius=2*radius;
	float a,b,c;
	D3DVECTOR I;
	for(DWORD i=0;i<3;i++)
	{
		a=intersectRayPlane(faceinfo->points[i],vecteurs[i],minPoint,_ZZ_);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(faceinfo->points[i]+(a*vecteurs[i])-minPoint);
			b=I*XX;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*YY;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}


		a=intersectRayPlane(faceinfo->points[i],vecteurs[i],minPoint,_YY_);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(faceinfo->points[i]+(a*vecteurs[i])-minPoint);
			b=I*XX;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*ZZ;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}


		a=intersectRayPlane(faceinfo->points[i],vecteurs[i],minPoint,_XX_);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(faceinfo->points[i]+(a*vecteurs[i])-minPoint);
			b=I*YY;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*ZZ;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}


		a=intersectRayPlane(faceinfo->points[i],vecteurs[i],maxPoint,ZZ);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(faceinfo->points[i]+(a*vecteurs[i])-maxPoint);
			b=I*_XX_;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*_YY_;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}


		a=intersectRayPlane(faceinfo->points[i],vecteurs[i],maxPoint,YY);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(faceinfo->points[i]+(a*vecteurs[i])-maxPoint);
			b=I*_XX_;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*_ZZ_;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}


		a=intersectRayPlane(faceinfo->points[i],vecteurs[i],maxPoint,XX);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(faceinfo->points[i]+(a*vecteurs[i])-maxPoint);
			b=I*_YY_;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*_ZZ_;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}
	}	// for i=1..2

	return false;
}



          

#define OP000(kk) kk[0]
#define OP001(kk) kk[1]
#define OP010(kk) kk[2]
#define OP011(kk) kk[3]
#define OP100(kk) kk[4]
#define OP101(kk) kk[5]
#define OP110(kk) kk[6]
#define OP111(kk) kk[7]



inline bool CheckTriangleCollideBox(FaceInfo & faceinfo,D3DVECTOR&center,float radius)
{	
	D3DVECTOR pp[8];

	OP000(pp)=center+D3DVEC(-radius,-radius,-radius);
	OP001(pp)=center+D3DVEC(-radius,-radius, radius);
	OP010(pp)=center+D3DVEC(-radius, radius,-radius);
	OP011(pp)=center+D3DVEC(-radius, radius, radius);
	OP100(pp)=center+D3DVEC( radius,-radius,-radius);
	OP101(pp)=center+D3DVEC( radius,-radius, radius);
	OP110(pp)=center+D3DVEC( radius, radius,-radius);
	OP111(pp)=center+D3DVEC( radius, radius, radius);


	if (CollisionDetection1faceinfo(pp[0],OP001(pp),faceinfo)) return true;
	if (CollisionDetection1faceinfo(pp[0],OP010(pp),faceinfo)) return true;
	if (CollisionDetection1faceinfo(pp[0],OP100(pp),faceinfo)) return true;

	if (CollisionDetection1faceinfo(OP011(pp),OP111(pp),faceinfo)) return true;
	if (CollisionDetection1faceinfo(OP011(pp),OP001(pp),faceinfo)) return true;
	if (CollisionDetection1faceinfo(OP011(pp),OP010(pp),faceinfo)) return true;

	if (CollisionDetection1faceinfo(OP101(pp),OP001(pp),faceinfo)) return true;
	if (CollisionDetection1faceinfo(OP101(pp),OP111(pp),faceinfo)) return true;
	if (CollisionDetection1faceinfo(OP101(pp),OP100(pp),faceinfo)) return true;

	if (CollisionDetection1faceinfo(OP110(pp),OP010(pp),faceinfo)) return true;
	if (CollisionDetection1faceinfo(OP110(pp),OP100(pp),faceinfo)) return true;
	if (CollisionDetection1faceinfo(OP110(pp),OP111(pp),faceinfo)) return true;

  
	return CheckTriangleCote_Box(&faceinfo,OP000(pp),OP111(pp),radius);

	return false;
}

/////////////////////////////////////////////////////////////////////////
//    Celles de MeshStruct
/////////////////////////////////////////////////////////////////////////


MeshStruct:: MeshStruct()
{
	this->loaded=false;
	this->filename=NULL;
	this->flag=FLAG_OBSTACLE;
	this->texturenames=NULL;
	this->mesh=NULL;
	this->MeshMaterials=NULL;
	this->MeshTextures=NULL;
	this->faceinfo=NULL;
	this->Transparency.Locked=false;
	this->Specular.Locked=false;
	this->faceinfo2.computed=false;
}

MeshStruct::~MeshStruct()
{
	if (this==NULL) return;
	this->UnLoad();
	__try
	{
		//if (this->filename!=NULL) free(this->filename);		
	}
	__finally
	{
		this->filename=NULL;
	}
}


HRESULT MeshStruct::OptimizeMesh(LPDIRECT3DDEVICE8 dev8,LPD3DXBUFFER ADJbuffer)
{
	if (!this->loaded) return S_OK;

	LPD3DXMESH pMeshTemp;
	DWORD *rgdwAdjacencyTemp=NULL;
	HRESULT hr;

    if (this->mesh->GetFVF() != (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1/*|D3DFVF_SPECULAR|D3DFVF_DIFFUSE*/))
    {
        hr = this->mesh->CloneMeshFVF( this->mesh->GetOptions(), D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1/*|D3DFVF_SPECULAR|D3DFVF_DIFFUSE*/, 
                                          dev8, &pMeshTemp );
        if( FAILED(hr) )
            goto End;

        this->mesh->Release();
        this->mesh = pMeshTemp;
    }

    D3DXComputeNormals( this->mesh,NULL );

    // allocate a second adjacency buffer to store post attribute sorted adjacency
    rgdwAdjacencyTemp = new DWORD[this->mesh->GetNumFaces() * 3];
    if (rgdwAdjacencyTemp == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto End;
    }

    // attribute sort - the un-optimized mesh option
    //          remember the adjacency for the vertex cache optimization
    hr = this->mesh->OptimizeInplace( D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT,
                                 (DWORD*)ADJbuffer->GetBufferPointer(),
                                 rgdwAdjacencyTemp, NULL, NULL);
    if( FAILED(hr) )
        goto End;

    // actually do the vertex cache optimization
    hr = this->mesh->OptimizeInplace( D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_VERTEXCACHE,
                                 rgdwAdjacencyTemp,
                                 NULL, NULL, NULL);
    if( FAILED(hr) )
        goto End;

    
End:
    if(rgdwAdjacencyTemp!=NULL)
		free(rgdwAdjacencyTemp);    
    return hr;	
}

		


/*int MeshStruct::GetCubeIndex(D3DVECTOR& pos) 
{ // -1 si il n'y a pas

	if (this==NULL) 
	{
		return -1;
	}

	if (this->faceinfo2.computed==false)
	{
		return -1;
	}

	int X,Y,Z;
	
	this->GetCubeCoords(pos,&X,&Y,&Z);

	if ( (X<0)|| (X>=int(2*this->faceinfo2.NCubesX)  ) ||
		 (Y<0)|| (Y>=int(2*this->faceinfo2.NCubesY)  ) ||
		 (Z<0)|| (Z>=int(2*this->faceinfo2.NCubesZ)  ) 
	   )
	{
		return-1;
	}

	DWORD index= X*4*this->faceinfo2.NCubesY*this->faceinfo2.NCubesZ
				+Y*2*this->faceinfo2.NCubesY	
				+Z;

	return int(index);	
}
*/


void MeshStruct::GetCubeCoords(D3DVECTOR& pos,int*X,int*Y,int*Z)
{
	D3DVECTOR K=pos-this->faceinfo2.Center;
	if (K.x>=0)
	{
		*X=(int)(K.x/faceinfo2.CubeRayon);*X+=faceinfo2.NCubesX;
	}
	else
	{
		*X=(int)(K.x/faceinfo2.CubeRayon)-1;
		*X+=faceinfo2.NCubesX;
	}

	if (K.y>=0)
	{
		*Y=(int)(K.y/faceinfo2.CubeRayon);*Y+=faceinfo2.NCubesY;
	}
	else
	{
		*Y=(int)(K.y/faceinfo2.CubeRayon)-1;
		*Y+=faceinfo2.NCubesY;
	}

	if (K.z>=0)
	{
		*Z=(int)(K.z/faceinfo2.CubeRayon);*Z+=faceinfo2.NCubesZ;
	}
	else
	{
		*Z=(int)(K.z/faceinfo2.CubeRayon)-1;
		*Z+=faceinfo2.NCubesZ;
	}
	
}



inline bool CheckPointInCube(D3DVECTOR&pos,D3DVECTOR&Center,float Radius)
{
	D3DVECTOR p=abs(Center-pos);
	if (p.x>Radius) return false;
	if (p.y>Radius) return false;
	if (p.z>Radius) return false;
	return true;
}

inline bool CheckRay_Box(D3DVECTOR&pos1,D3DVECTOR&pos2,D3DVECTOR& minPoint,D3DVECTOR& maxPoint,float radius)
{
	D3DVECTOR vecteurs=pos2-pos1;
	
	float _2radius=2*radius;
	float a,b,c;
	D3DVECTOR I;
	
		a=intersectRayPlane(pos1,vecteurs,minPoint,_ZZ_);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(pos1+(a*vecteurs)-minPoint);
			b=I*XX;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*YY;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}


		a=intersectRayPlane(pos1,vecteurs,minPoint,_YY_);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(pos1+(a*vecteurs)-minPoint);
			b=I*XX;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*ZZ;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}


		a=intersectRayPlane(pos1,vecteurs,minPoint,_XX_);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(pos1+(a*vecteurs)-minPoint);
			b=I*YY;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*ZZ;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}


		a=intersectRayPlane(pos1,vecteurs,maxPoint,ZZ);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(pos1+(a*vecteurs)-maxPoint);
			b=I*_XX_;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*_YY_;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}


		a=intersectRayPlane(pos1,vecteurs,maxPoint,YY);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(pos1+(a*vecteurs)-maxPoint);
			b=I*_XX_;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*_ZZ_;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}


		a=intersectRayPlane(pos1,vecteurs,maxPoint,XX);
		if ((a==0) || (a==1)) return true;
		if ((a>0) && (a<1))
		{
			I=(pos1+(a*vecteurs)-maxPoint);
			b=I*_YY_;
			if ((b>=0)&&(b<=_2radius))
			{
				c=I*_ZZ_;
				if ((c>=0)&&(c<=_2radius))
					return true;
			}			
		}
	
	return false;	
}




bool MeshStruct::RayIntersectsCube(D3DVECTOR& pos1,D3DVECTOR& pos2,int X,int Y,int Z)
{
	
	if (X<0) return false;
	if (Y<0) return false;
	if (Z<0) return false;
	if ((DWORD)X>=2*this->faceinfo2.NCubesX) return false;
	if ((DWORD)Y>=2*this->faceinfo2.NCubesY) return false;
	if ((DWORD)Z>=2*this->faceinfo2.NCubesZ) return false;
	
	DWORD index=X*4*this->faceinfo2.NCubesY*this->faceinfo2.NCubesZ
		       +Y*2*this->faceinfo2.NCubesZ
			   +Z;
	if (CheckPointInCube(pos1,this->faceinfo2.faceinfo2[index].center,this->faceinfo2.CubeRayon))
		return true;
	if (CheckPointInCube(pos2,this->faceinfo2.faceinfo2[index].center,this->faceinfo2.CubeRayon))
		return true;


	return CheckRay_Box(pos1,pos2,
		                D3DVEC(-this->faceinfo2.CubeRayon,-this->faceinfo2.CubeRayon,-this->faceinfo2.CubeRayon) 
						+this->faceinfo2.faceinfo2[index].center,
						D3DVEC(this->faceinfo2.CubeRayon,this->faceinfo2.CubeRayon,this->faceinfo2.CubeRayon) 
						+this->faceinfo2.faceinfo2[index].center,
						this->faceinfo2.CubeRayon
						);
}



void MeshStruct::GetCube(D3DVECTOR& pos,FaceInfo2 * _faceinfo2)
{
	if (this==NULL) 
	{
		_faceinfo2->faceinfo=NULL;
		_faceinfo2->NumFaces=0;
		return;
	}

	if (this->faceinfo2.computed==false)
	{
		_faceinfo2->faceinfo=NULL;
		_faceinfo2->NumFaces=0;
		return;
	}

	int X,Y,Z;
	this->GetCubeCoords(pos,&X,&Y,&Z);

	if ( (X<0)|| (X>=int(2*this->faceinfo2.NCubesX)) ||
		 (Y<0)|| (Y>=int(2*this->faceinfo2.NCubesY)) ||
		 (Z<0)|| (Z>=int(2*this->faceinfo2.NCubesZ)) 
	   )
	{
		_faceinfo2->faceinfo=NULL;
		_faceinfo2->NumFaces=0;
		return;
	}

	DWORD index= X*4*this->faceinfo2.NCubesY*this->faceinfo2.NCubesZ
				+Y*2*this->faceinfo2.NCubesY
				+Z;

	*_faceinfo2=this->faceinfo2.faceinfo2[index];
}

inline void AddFaceInfoToFaceInfo2(FaceInfo2*faceinfo2,FaceInfo*faceinfo)
{
	if (faceinfo2->NumFaces==0)
	{
		faceinfo2->faceinfo=(FaceInfo**)malloc(sizeof(FaceInfo*));
		faceinfo2->faceinfo[0]=faceinfo;
		faceinfo2->NumFaces=1;
	}
	else
	{
		faceinfo2->NumFaces++;
	    faceinfo2->faceinfo=(FaceInfo**)realloc(faceinfo2->faceinfo,faceinfo2->NumFaces*sizeof(FaceInfo*));
		faceinfo2->faceinfo[faceinfo2->NumFaces-1]=faceinfo;	
	}
}




void MeshStruct::ComputeFaceInfo2(float Rayon)
{ 
	////////////////////////////////////////////////////////////////
	//
	//  ComputeFaceInfo()  doit etre appelée avant cette procedure
	//
	////////////////////////////////////////////////////////////////
	

	if (this->faceinfo2.computed)
	{
		this->faceinfo2.computed=false;
		if(this->faceinfo2.faceinfo2!=NULL)
		{
			DWORD mmm=8*this->faceinfo2.NCubesX*this->faceinfo2.NCubesY*this->faceinfo2.NCubesZ;
			for(DWORD i=0;i<mmm;i++)
			{
				if(this->faceinfo2.faceinfo2[i].NumFaces>0)
					free(this->faceinfo2.faceinfo2[i].faceinfo);
			}
		}
		free(this->faceinfo2.faceinfo2);
	}




	float minx=1000000,maxx=-1000000,miny=1000000,maxy=-1000000,minz=1000000,maxz=-1000000;
	DWORD i;
	for(i=0;i<this->NumFaces;i++)
	{
		for(DWORD j=0;j<3;j++)
		{
			if (this->faceinfo[i].points[j].x<minx)minx=this->faceinfo[i].points[j].x;
			if (this->faceinfo[i].points[j].y<miny)miny=this->faceinfo[i].points[j].y;
			if (this->faceinfo[i].points[j].z<minz)minz=this->faceinfo[i].points[j].z;
			if (this->faceinfo[i].points[j].x>maxx)maxx=this->faceinfo[i].points[j].x;
			if (this->faceinfo[i].points[j].y>maxy)maxy=this->faceinfo[i].points[j].y;
			if (this->faceinfo[i].points[j].z>maxz)maxz=this->faceinfo[i].points[j].z;

		}
	}

	this->faceinfo2.RX=(maxx-minx)/2;
	this->faceinfo2.RY=(maxy-miny)/2;
	this->faceinfo2.RZ=(maxz-minz)/2;

	this->faceinfo2.Center=D3DVEC(
		                          (maxx+minx)/2,
		                          (maxy+miny)/2,
								  (maxz+minz)/2
								 );
	

	faceinfo2.NCubesX=DWORD(1+faceinfo2.RX/Rayon);
	faceinfo2.NCubesY=DWORD(1+faceinfo2.RY/Rayon);
	faceinfo2.NCubesZ=DWORD(1+faceinfo2.RZ/Rayon);
	faceinfo2.CubeRayon=Rayon;

	if (8*faceinfo2.NCubesX*faceinfo2.NCubesY*faceinfo2.NCubesX>
		2*this->NumFaces)
	{
		float Rayon2=8.0f*(1+faceinfo2.RX)*(1+faceinfo2.RX)*
			         (1+faceinfo2.RZ)
					 /(2.0f*this->NumFaces);
		Rayon2=float(exp(log(Rayon2)/3.0));
		Rayon=Rayon2;

		faceinfo2.NCubesX=DWORD(1+faceinfo2.RX/Rayon);
		faceinfo2.NCubesY=DWORD(1+faceinfo2.RY/Rayon);
		faceinfo2.NCubesZ=DWORD(1+faceinfo2.RZ/Rayon);
		faceinfo2.CubeRayon=Rayon;
	}


	faceinfo2.faceinfo2=(FaceInfo2*)malloc(8*faceinfo2.NCubesX*faceinfo2.NCubesY*faceinfo2.NCubesZ*sizeof(FaceInfo2));

	for (DWORD X=0;X<2*faceinfo2.NCubesX;X++)
		for (DWORD Y=0;Y<2*faceinfo2.NCubesY;Y++)
			for (DWORD Z=0;Z<2*faceinfo2.NCubesZ;Z++)
			{
				DWORD K= X*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
					    +Y*2*faceinfo2.NCubesZ
						+Z;
								   
				faceinfo2.faceinfo2[K].NumFaces=0;
				faceinfo2.faceinfo2[K].faceinfo=NULL;
				float fx=(float)X;fx=fx-faceinfo2.NCubesX;
				float fy=(float)Y;fy=fy-faceinfo2.NCubesY;
				float fz=(float)Z;fz=fz-faceinfo2.NCubesZ;
				faceinfo2.faceinfo2[K].center=D3DVEC( 
										 (fx+0.5f)*faceinfo2.CubeRayon,
					                     (fy+0.5f)*faceinfo2.CubeRayon,
										 (fz+0.5f)*faceinfo2.CubeRayon
										)+faceinfo2.Center;
				
			}

	int added=0,oldadded=0;
	int added2=0;

	for (i=0;i<this->NumFaces;i++)
	{
		int   minX=faceinfo2.NCubesX*2,maxX=0
			 ,minY=faceinfo2.NCubesY*2,maxY=0
			 ,minZ=faceinfo2.NCubesZ*2,maxZ=0;

		// 1er point
		int X,Y,Z;
		this->GetCubeCoords(faceinfo[i].points[0],&X,&Y,&Z);

		if (X<minX)minX=X;
		if (X>maxX)maxX=X;
		if (Y<minY)minY=Y;
		if (Y>maxY)maxY=Y;
		if (Z<minZ)minZ=Z;
		if (Z>maxZ)maxZ=Z;
		
		DWORD index;

		index=X*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
			 +Y*2*faceinfo2.NCubesZ 
			 +Z;
		AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
		added++;			
		
		

		
		// 2eme point
		this->GetCubeCoords(faceinfo[i].points[1],&X,&Y,&Z);

		if (X<minX)minX=X;
		if (X>maxX)maxX=X;
		if (Y<minY)minY=Y;
		if (Y>maxY)maxY=Y;
		if (Z<minZ)minZ=Z;
		if (Z>maxZ)maxZ=Z;
		

		
		index=X*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
			 +Y*2*faceinfo2.NCubesZ 
			 +Z;
		AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
		added++;
		
		

		// 3eme point
		this->GetCubeCoords(faceinfo[i].points[2],&X,&Y,&Z);


		if (X<minX)minX=X;
		if (X>maxX)maxX=X;
		if (Y<minY)minY=Y;
		if (Y>maxY)maxY=Y;
		if (Z<minZ)minZ=Z;
		if (Z>maxZ)maxZ=Z;
		

		
		index=X*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
			 +Y*2*faceinfo2.NCubesZ 
			 +Z;
		AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
		added++;			
		
		

		if (minX<0) 
			minX=0;
		if (minY<0) 
			minY=0;
		if (minZ<0)
			minZ=0;
		if (maxX>=int(2*faceinfo2.NCubesX))
			maxX=2*faceinfo2.NCubesX-1;
		if (maxY>=int(2*faceinfo2.NCubesY))
			maxY=2*faceinfo2.NCubesY-1;
		if (maxZ>=int(2*faceinfo2.NCubesZ))
			maxZ=2*faceinfo2.NCubesZ-1;

		
		if(minX>maxX) 
			break;
		if(minY>maxY) 
			break;
		if(minZ>maxZ) 
			break;
		
		
		
		if (minX==maxX)
		{
			if(minY==maxY)
			{
				if(minZ==maxZ)
				{
					index=minX*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
						  +minY*2*faceinfo2.NCubesZ 
						  +minZ;
					AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
					added++;
				}
				else
				{
					for(Z=minZ;Z<=maxZ;Z++)
					{
						index=minX*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
							  +minY*2*faceinfo2.NCubesZ 
							  +Z;
						AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
						added++;
					}
				}
			}
			else
			{
				if(minZ==maxZ)
				{
					for(Y=minY;Y<=maxY;Y++)
					{
						index= minX*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
							  +   Y*2*faceinfo2.NCubesZ 
							  +minZ;
						AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
						added++;
					}
				}
				else
				{
					for(Y=minY;Y<=maxY;Y++)
						for(Z=minZ;Z<=maxZ;Z++)
						{
							index=minX*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
								    +Y*2*faceinfo2.NCubesZ 
								    +Z;

							if (CheckTriangleCollideBox(faceinfo[i],faceinfo2.faceinfo2[index].center,Rayon))
							{
								AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
								added++;
							}
						}
				}
			}
		}
		else
		{
			if(minY==maxY)
			{
				if(minZ==maxZ)
				{
					for(X=minX;X<=maxX;X++)
					{
						index=    X*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
							  +minY*2*faceinfo2.NCubesZ 
							  +minZ;
						AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
						added++;
					}
				}
				else
				{
					for(X=minX;X<=maxX;X++)
						for(Z=minZ;Z<=maxZ;Z++)
						{
							index=    X*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
								  +minY*2*faceinfo2.NCubesZ 
								  +   Z;

							if (CheckTriangleCollideBox(faceinfo[i],faceinfo2.faceinfo2[index].center,Rayon))
							{
								AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
								added++;
							}
						}

				}
			}
			else
			{
				if(minZ==maxZ)
				{
					for(X=minX;X<=maxX;X++)
						for(Y=minY;Y<=maxY;Y++)
						{
							index=    X*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
								  +   Y*2*faceinfo2.NCubesZ 
								  +minZ;

							if (CheckTriangleCollideBox(faceinfo[i],faceinfo2.faceinfo2[index].center,Rayon))
							{
								AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
								added++;
							}
						}

				}
				else
				{
					for(X=minX;X<=maxX;X++)
						for(Y=minY;Y<=maxY;Y++)
							for(Z=minZ;Z<=maxZ;Z++)
							{
								index=X*4*faceinfo2.NCubesY*faceinfo2.NCubesZ
									 +Y*2*faceinfo2.NCubesZ 
									 +Z;

								if (CheckTriangleCollideBox(faceinfo[i],faceinfo2.faceinfo2[index].center,Rayon))
								{
									AddFaceInfoToFaceInfo2(&faceinfo2.faceinfo2[index],&faceinfo[i]);
									added++;
								}
							}

				}
			}
		}
		

		if (oldadded!=added)
		{
			oldadded=added;
			added2++;
		}

			
	} // for numfaces


	
	this->faceinfo2.computed=true;	
}



void MeshStruct::LockTransparency(float Factor)
{
	if (this==NULL) return;
	this->Transparency.Factor=Factor;
	this->Transparency.Locked=true;
}

void MeshStruct::UnlockTransparency()
{
	if (this==NULL) return;
	this->Transparency.Locked=false;
}


void MeshStruct::LockSpecular(float R,float G,float B,float Power)
{
	if (this==NULL) return;
	this->Specular.Color.r=R;
	this->Specular.Color.g=G;
	this->Specular.Color.b=B;
	this->Specular.Color.a=1.0f;
	this->Specular.Power=Power;

	this->Specular.Locked=true;
}

void MeshStruct::UnlockSpecular()
{
	if (this==NULL) return;
	this->Specular.Locked=false;
}




bool MeshStruct:: LoadFormFile(char * FileName,LPDIRECT3DDEVICE8 dev8)
{
	if (this==NULL) return false;
	
	this->filename=Text_Text(FileName);
	char * Path=GetPath(filename);

	if (this->loaded) this->UnLoad();

	LPD3DXBUFFER MaterialBuffer;
	LPD3DXBUFFER ADJbuffer;

	HRESULT hr;
    
	if (FAILED(hr=D3DXLoadMeshFromX(Text_Text(FileName),D3DXMESH_SYSTEMMEM,dev8,&ADJbuffer,
									&MaterialBuffer,&this->NumMaterials,
									&this->mesh)
				)
		)
	{
		MessageBox(0,"Fail to load X File","Error",MB_OK);
		return false;
	}

	

	this->loaded=true;

	D3DXMATERIAL  * d3dxMaterials = (D3DXMATERIAL*)MaterialBuffer->GetBufferPointer();
    MeshMaterials = new D3DMATERIAL8[this->NumMaterials  ];
    MeshTextures  = new LPDIRECT3DTEXTURE8[this->NumMaterials];

	this->texturenames=(char **) malloc(4*this->NumMaterials);
	

	for( DWORD i=0; i<NumMaterials; i++ )
    {
        // Copy the material
        MeshMaterials[i] = d3dxMaterials[i].MatD3D;

		MeshMaterials[i].Specular.a=0;
		MeshMaterials[i].Specular.r=0;
		MeshMaterials[i].Specular.g=0;
		MeshMaterials[i].Specular.b=0;
		MeshMaterials[i].Power=0;


		MeshMaterials[i].Emissive.a=0;
		MeshMaterials[i].Emissive.r=0;
		MeshMaterials[i].Emissive.g=0;
		MeshMaterials[i].Emissive.b=0;

		MeshMaterials[i].Ambient.a=0;
		MeshMaterials[i].Ambient.r=0;
		MeshMaterials[i].Ambient.g=0;
		MeshMaterials[i].Ambient.b=0;

		if ((flag==FLAG_TERRAIN)||(flag==FLAG_CAR))
		{
			MeshMaterials[i].Diffuse.a*=0.6f;
			MeshMaterials[i].Diffuse.r*=0.6f;
			MeshMaterials[i].Diffuse.g*=0.6f;
			MeshMaterials[i].Diffuse.b*=0.6f;
		}

	     
        // Create the texture
		if (NULL!=d3dxMaterials[i].pTextureFilename)
		{
			this->texturenames[i]=Text_Text(d3dxMaterials[i].pTextureFilename);
			this->texturenames[i]=Text_Concat(Path,this->texturenames[i]);
			if( FAILED( D3DXCreateTextureFromFile( dev8, 
												   this->texturenames[i], 
												   &MeshTextures[i] ) ) )
			{
				MeshTextures[i] = NULL;
				this->texturenames[i]=NULL;
			}
		}
		else
		{
			MeshTextures[i]=NULL;
			this->texturenames[i]=NULL;
		}
		
    }

    // Done with the material buffer
    MaterialBuffer->Release();
	MaterialBuffer=NULL;

	this->OptimizeMesh(dev8,ADJbuffer);
	ADJbuffer->Release();ADJbuffer=NULL;
	
	return true;
}


bool MeshStruct::ReloadMesh(LPDIRECT3DDEVICE8 dev8)
{
	if (this==NULL) return false;
	
	if (this->loaded) this->UnLoad();
	char * Path=GetPath(filename);

	LPD3DXBUFFER MaterialBuffer;
	LPD3DXBUFFER ADJbuffer;


	HRESULT hr;
	if (FAILED(hr=D3DXLoadMeshFromX(this->filename,D3DXMESH_SYSTEMMEM,dev8,&ADJbuffer,
									&MaterialBuffer,&this->NumMaterials,
									&this->mesh)
				)
		)
	{
		MessageBox(0,"Fail to load X File","Error",MB_OK);
		return false;
	}

	

	this->loaded=true;

	D3DXMATERIAL  * d3dxMaterials = (D3DXMATERIAL*)MaterialBuffer->GetBufferPointer();
    MeshMaterials = new D3DMATERIAL8[this->NumMaterials  ];
    MeshTextures  = new LPDIRECT3DTEXTURE8[this->NumMaterials];

	this->texturenames=(char **) malloc(4*this->NumMaterials);

	for( DWORD i=0; i<NumMaterials; i++ )
    {
        // Copy the material
        MeshMaterials[i] = d3dxMaterials[i].MatD3D;


		MeshMaterials[i].Emissive.a=0;
		MeshMaterials[i].Emissive.r=0;
		MeshMaterials[i].Emissive.g=0;
		MeshMaterials[i].Emissive.b=0;
		
		MeshMaterials[i].Specular.a=0;
		MeshMaterials[i].Specular.r=0;
		MeshMaterials[i].Specular.g=0;
		MeshMaterials[i].Specular.b=0;
		MeshMaterials[i].Power=0;

		MeshMaterials[i].Ambient.a=0;
		MeshMaterials[i].Ambient.r=0;
		MeshMaterials[i].Ambient.g=0;
		MeshMaterials[i].Ambient.b=0;

		if ((flag==FLAG_TERRAIN)||(flag==FLAG_CAR))
		{
			MeshMaterials[i].Diffuse.a*=0.6f;
			MeshMaterials[i].Diffuse.r*=0.6f;
			MeshMaterials[i].Diffuse.g*=0.6f;
			MeshMaterials[i].Diffuse.b*=0.6f;
		}

        
		// Create the texture
		if (NULL!=d3dxMaterials[i].pTextureFilename)
		{
			this->texturenames[i]=Text_Text(d3dxMaterials[i].pTextureFilename);
			this->texturenames[i]=Text_Concat(Path,this->texturenames[i]);
			if( FAILED( D3DXCreateTextureFromFile( dev8, 
												   this->texturenames[i], 
												   &MeshTextures[i] ) ) )
			{
				MeshTextures[i] = NULL;
				this->texturenames[i]=NULL;
			}
		}
		else
		{
			MeshTextures[i]=NULL;
			this->texturenames[i]=NULL;
		}
		
    }

    // Done with the material buffer
    MaterialBuffer->Release();
	MaterialBuffer=NULL;

	this->OptimizeMesh(dev8,ADJbuffer);
	ADJbuffer->Release();ADJbuffer=NULL;
	
	return true;
}


bool MeshStruct::UnLoad()
{
	if (this==NULL) return false;
	
	if (!this->loaded) return true;

	this->loaded=false;

	if (this->mesh!=NULL) this->mesh->Release();this->mesh=NULL;
	
	DWORD i;
	for (i=0;i<this->NumMaterials;i++)
	{
		if (this->texturenames[i]!=NULL) free(this->texturenames[i]);
		if (this->MeshTextures[i]!=NULL) this->MeshTextures[i]->Release();
	}

	free(this->texturenames);   this->texturenames=NULL;
	free(this->MeshMaterials);  this->MeshMaterials=NULL;
	free(this->MeshTextures);   this->MeshTextures=NULL;

	if (this->faceinfo!=NULL) free(this->faceinfo);
	this->faceinfo=NULL;

	if (this->faceinfo2.computed)
	{
		this->faceinfo2.computed=false;
		if(this->faceinfo2.faceinfo2!=NULL)
		{
			DWORD mmm=8*this->faceinfo2.NCubesX*this->faceinfo2.NCubesY*this->faceinfo2.NCubesZ;
			for(DWORD i=0;i<mmm;i++)
			{
				if(this->faceinfo2.faceinfo2[i].NumFaces>0)
					free(this->faceinfo2.faceinfo2[i].faceinfo);
			}
		}
		free(this->faceinfo2.faceinfo2);
	}

	return true;
}


bool MeshStruct::Render(LPDIRECT3DDEVICE8 dev8,CDirect3DDeviceUtility * pd3dDeviceUtility)
{
	if (this==NULL) return false;
	
	if (dev8==NULL) return false;
	if (!this->loaded) return false;
	if (this->mesh==NULL)   return false;

    pd3dDeviceUtility->SetTransform( D3DTS_WORLD, &this->matrice );

	D3DMATERIAL8 mtrl;

	DWORD sSE,sAE,sSB,sDB;
	

	pd3dDeviceUtility->GetRenderState(D3DRS_SPECULARENABLE,&sSE);
	pd3dDeviceUtility->GetRenderState(D3DRS_ALPHABLENDENABLE,&sAE);
	pd3dDeviceUtility->GetRenderState(D3DRS_SRCBLEND,&sSB);
	pd3dDeviceUtility->GetRenderState(D3DRS_DESTBLEND,&sDB);

	pd3dDeviceUtility->SetRenderState(D3DRS_SPECULARENABLE,false);
	pd3dDeviceUtility->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	pd3dDeviceUtility->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pd3dDeviceUtility->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);


	for( DWORD i=0; i<this->NumMaterials; i++ )
    {
        // Set the material and texture for this subset

		mtrl=MeshMaterials[i];

		if (this->Specular.Locked==true)
		{
			mtrl.Specular=this->Specular.Color;
			mtrl.Power=this->Specular.Power;
		}

		if (this->Transparency.Locked==true)
		{
			mtrl.Diffuse.a*=this->Transparency.Factor;
			mtrl.Ambient.a*=this->Transparency.Factor;
		}

        dev8->SetMaterial( &mtrl );
        dev8->SetTexture ( 0, MeshTextures[i] );
        
        // Draw the mesh subset
        this->mesh->DrawSubset( i );
    }

	pd3dDeviceUtility->SetRenderState(D3DRS_SPECULARENABLE,sSE);
	pd3dDeviceUtility->SetRenderState(D3DRS_ALPHABLENDENABLE,sAE);
	pd3dDeviceUtility->SetRenderState(D3DRS_SRCBLEND,sSB);
	pd3dDeviceUtility->SetRenderState(D3DRS_DESTBLEND,sDB);

	return true;
}


bool MeshStruct::ComputeFaceInfo()
{
	if (this==NULL) return false;
	
	if (this->mesh==NULL) return false;
	this->NumFaces=this->mesh->GetNumFaces();
	if (this->NumFaces==0) return false;
	if (this->faceinfo!=NULL) free(this->faceinfo);
	
	
	this->faceinfo=(FaceInfo*)malloc(sizeof(FaceInfo)*this->NumFaces);
	
	LPDIRECT3DDEVICE8 dev8;
	this->mesh->GetDevice(&dev8);

	
	LPD3DXMESH mesh1;

	if (FAILED(this->mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM ,D3DFVF_XYZ ,dev8,&mesh1) ))
		return false;

	
	
	WORD *IB;
	D3DVECTOR * VB;
	

	mesh1->LockIndexBuffer(D3DLOCK_READONLY  ,(BYTE**) &IB);
	mesh1->LockVertexBuffer(D3DLOCK_READONLY ,(BYTE**) &VB);

	float minx=10000,maxx=-10000,miny=10000,maxy=-10000,minz=10000,maxz=-10000;
	
	DWORD i;
	for (i=0;i<this->NumFaces;i++)
	{
		D3DVECTOR A=VB[IB[i*3]],
				  B=VB[IB[i*3+1]],
				  C=VB[IB[i*3+2]];

		if(minx>A.x) minx=A.x;
		if(minx>B.x) minx=B.x;
		if(minx>C.x) minx=C.x;
		if(maxx<A.x) maxx=A.x;
		if(maxx<B.x) maxx=B.x;
		if(maxx<C.x) maxx=C.x;

		if(miny>A.y) miny=A.y;
		if(miny>B.y) miny=B.y;
		if(miny>C.y) miny=C.y;
		if(maxy<A.y) maxy=A.y;
		if(maxy<B.y) maxy=B.y;
		if(maxy<C.y) maxy=C.y;

		if(minz>A.z) minz=A.z;
		if(minz>B.z) minz=B.z;
		if(minz>C.z) minz=C.z;
		if(maxz<A.z) maxz=A.z;
		if(maxz<B.z) maxz=B.z;
		if(maxz<C.z) maxz=C.z;
				
		this->faceinfo[i].points[0]=A * this->matrice;
		this->faceinfo[i].points[1]=B * this->matrice;
		this->faceinfo[i].points[2]=C * this->matrice;

		D3DVECTOR AC,AB;
		AC=this->faceinfo[i].points[2]-this->faceinfo[i].points[0];
		AB=this->faceinfo[i].points[1]-this->faceinfo[i].points[0];

		this->faceinfo[i].normal=AB^AC;
		this->faceinfo[i].normal=this->faceinfo[i].normal/~this->faceinfo[i].normal;

		for (int j=0;j<3;j++)
		{
			int next_j=j+1;
			if(next_j==3) next_j=0;
			this->faceinfo[i].n[j]=(this->faceinfo[i].points[next_j]-this->faceinfo[i].points[j])^this->faceinfo[i].normal;
			this->faceinfo[i].n[j]=this->faceinfo[i].n[j]/~this->faceinfo[i].n[j];
			this->faceinfo[i].d[j]=-1*(this->faceinfo[i].n[j]*this->faceinfo[i].points[j]);

		}
	}
	
	mesh1->UnlockIndexBuffer();
	mesh1->UnlockVertexBuffer();

	for (int k=0;k<2;k++)
	for (int j=0;j<2;j++)
	for (i=0;i<2;i++)
	{
		this->BoundingBox.points[i+j*2+k*4].x=i*maxx+(1-i)*minx;
		this->BoundingBox.points[i+j*2+k*4].y=j*maxy+(1-j)*miny;
		this->BoundingBox.points[i+j*2+k*4].z=k*maxz+(1-k)*minz;
	}
	
	float Radius=0.5f*(float)sqrt(sqr(maxx-minx)+sqr(maxy-miny)+sqr(maxz-minz));
	this->BoundingBox.minx =minx ;
	this->BoundingBox.maxx =maxx ;
	this->BoundingBox.miny =miny ;
	this->BoundingBox.maxy =maxy ;
	this->BoundingBox.minz =minz ;
	this->BoundingBox.maxz =maxz ;

	D3DVECTOR OX=D3DVEC(Radius,0,0);
	OX=OX*this->matrice-D3DVEC(0,0,0)*this->matrice;

	this->TransformedRadius=~OX;
	
	return true;
}
		

/////////////////////////////////////////////////////////////////////////
//    Celles de CarClass
/////////////////////////////////////////////////////////////////////////

CarClass::CarClass()
{
	this->contact=false;
	this->gravity=D3DVEC(0,-0.01f,0);
	this->velocity=D3DVEC(0,0,0);
	this->balise=0;this->tour=0;
	this->vie=4;
	this->Mesh=NULL;
	this->State.car_state=CAR_STATE_ALIFE;
	this->State.sub_state=0;
	this->State.s1=0.0;
	this->State.s2=0.0;
	this->ContactTimer=0.0f;
}

CarClass::~CarClass()
{
	if (this==NULL) return;
	this->Mesh=NULL;
}

bool CarClass::ChekOutOfViewport(D3DXMATRIX view,D3DXMATRIX Projection,
		                         D3DVIEWPORT8 viewport)
{

	D3DXVECTOR3 V=D3DVEC(0,0,0);
	D3DXVec3Project(&V,&V,&viewport,&Projection,&view,&this->Mesh->matrice); 

	if (V.x<viewport.X) return true;
	if (V.y<viewport.Y) return true;
	if (V.x>viewport.X+viewport.Width) return true;
	if (V.y>viewport.Y+viewport.Height) return true;

	return false;
}




void CarClass::Accelerer(DWORD DeltaT) // milli seconde
{
	if (this==NULL) return;
	if (this->Mesh==NULL) return;
	
	if (contact)
	{
		D3DVECTOR CarOxDirectionNormalizedVectorUsedForVelocity
			 =D3DVEC(this->Mesh->matrice(0,0),
					   this->Mesh->matrice(0,1),
					   this->Mesh->matrice(0,2)
						);

		CarOxDirectionNormalizedVectorUsedForVelocity=
			CarOxDirectionNormalizedVectorUsedForVelocity/
			~CarOxDirectionNormalizedVectorUsedForVelocity;

		this->velocity=this->velocity+
			(4*1.5f*DeltaT/10000.0f*this->Mesh->TransformedRadius)*
			CarOxDirectionNormalizedVectorUsedForVelocity;

		float	k=~this->velocity;
		if(k>=this->Mesh->TransformedRadius)
			this->velocity=this->Mesh->TransformedRadius/k*velocity;

	}
}

void CarClass::Decelerer(DWORD DeltaT) // milli seconde
{
	if (this==NULL) return;
	if (this->Mesh==NULL) return;

	if (contact)
	{
		D3DVECTOR CarOxDirectionNormalizedVectorUsedForVelocity
			 =D3DVEC(this->Mesh->matrice(0,0),
					   this->Mesh->matrice(0,1),
					   this->Mesh->matrice(0,2)
						);

		CarOxDirectionNormalizedVectorUsedForVelocity=
			CarOxDirectionNormalizedVectorUsedForVelocity/
			~CarOxDirectionNormalizedVectorUsedForVelocity;

		this->velocity=this->velocity-
			(4*0.75f*DeltaT/10000.0f*this->Mesh->TransformedRadius)*
			CarOxDirectionNormalizedVectorUsedForVelocity;

		float	k=~this->velocity;
		if(k>=this->Mesh->TransformedRadius)
			this->velocity=this->Mesh->TransformedRadius/k*velocity;
		
	}
}

void CarClass::TournerGauche(DWORD DeltaT) // milli seconde
{
	if (this==NULL) return;
	if (this->Mesh==NULL) return;

		D3DXMATRIX rot;
		D3DXVECTOR3 oy=D3DXVECTOR3(this->Mesh->matrice(1,0),this->Mesh->matrice(1,1),this->Mesh->matrice(1,2));
		oy=oy/~oy;
		D3DXMatrixRotationAxis(&rot,&oy,-PI*DeltaT/1000.0f);
		D3DVECTOR pos=D3DVEC(this->Mesh->matrice(3,0),this->Mesh->matrice(3,1),this->Mesh->matrice(3,2));
		D3DXMATRIX trans1,trans2;
		D3DXMatrixTranslation(&trans1,-pos.x,-pos.y,-pos.z);
		D3DXMatrixTranslation(&trans2,pos.x,pos.y,pos.z);
		this->Mesh->matrice*=trans1;
		this->Mesh->matrice*=rot;
		this->Mesh->matrice*=trans2;
}


void CarClass::TournerDroite(DWORD DeltaT) // milli seconde;
{
	if (this==NULL) return;
	if (this->Mesh==NULL) return;

		D3DXMATRIX rot;
		D3DXVECTOR3 oy=D3DXVECTOR3(this->Mesh->matrice(1,0),this->Mesh->matrice(1,1),this->Mesh->matrice(1,2));
		oy=oy/~oy;
		D3DXMatrixRotationAxis(&rot,&oy,PI*DeltaT/1000.0f);
		D3DVECTOR pos=D3DVEC(this->Mesh->matrice(3,0),this->Mesh->matrice(3,1),this->Mesh->matrice(3,2));
		D3DXMATRIX trans1,trans2;
		D3DXMatrixTranslation(&trans1,-pos.x,-pos.y,-pos.z);
		D3DXMatrixTranslation(&trans2,pos.x,pos.y,pos.z);
		this->Mesh->matrice*=trans1;
		this->Mesh->matrice*=rot;
		this->Mesh->matrice*=trans2;

}


void CarClass::UpdatePosition(PMeshManager MONDE,BYTE *state)
{
	if (this->State.car_state!=CAR_STATE_ALIFE)
		return;

	D3DVECTOR pos=D3DVEC(this->Mesh->matrice(3,0),this->Mesh->matrice(3,1),this->Mesh->matrice(3,2));

	if(MONDE->NumBalises)
	{	

			float MINDIST=1e30f;
			bool tour=false;

			for (DWORD b=this->balise;b<=this->balise+2;b++)
			{
				DWORD real_b= b % MONDE->NumBalises;
				D3DVECTOR KP=pos-MONDE->Balises[real_b].K;
				float ffff=~KP;
				if ( (ffff<MINDIST) && (ffff<(10*this->Mesh->TransformedRadius)) )
				{
					if(b>=MONDE->NumBalises)
						tour=true;
					else
						tour=false;
					this->balise=real_b;
					MINDIST=ffff;
				}									
			} // for b

			if (tour)
				this->tour++;
	}

	D3DVECTOR I,normale;

	//update position
	contact=false;
	DWORD mesh_id;
	float distance;
	CollisionDetection_Using_Sphere_OBSTACLE(&contact,&mesh_id,pos,pos+velocity+gravity,this->Mesh->TransformedRadius,&normale,&I,MONDE,&distance);

	if(contact) //gestion des collisions
	{
		if(distance<0)  //gestion des erreurs
		{
			D3DVECTOR newpos=pos+(this->Mesh->TransformedRadius-(pos-I)*normale)*normale;
			this->Mesh->matrice(3,0)=0.9f*pos.x+0.1f*newpos.x;
			this->Mesh->matrice(3,1)=0.9f*pos.y+0.1f*newpos.y;
			this->Mesh->matrice(3,2)=0.9f*pos.z+0.1f*newpos.z;
		}
		else
		{
			D3DVECTOR oy=D3DVEC(this->Mesh->matrice(1,0),this->Mesh->matrice(1,1),this->Mesh->matrice(1,2));
			oy=oy/~oy;
			float dot=oy*normale;

			*state=1; //bounce
			this->Mesh->matrice(3,0)+=0.01f*normale.x;
			this->Mesh->matrice(3,1)+=0.01f*normale.y;
			this->Mesh->matrice(3,2)+=0.01f*normale.z;
			velocity=-0.5f*velocity;

		}
	} 


	bool	contact1=false;
	CollisionDetection_Using_Sphere_TERRAIN(&contact1,&mesh_id,pos,pos+velocity+gravity,this->Mesh->TransformedRadius,&normale,&I,MONDE,&distance);

	if(contact1) //gestion des collisions
	{
		if(distance<0)  //gestion des erreurs
		{
			D3DVECTOR newpos=pos+(this->Mesh->TransformedRadius-(pos-I)*normale)*normale;
			this->Mesh->matrice(3,0)=0.9f*pos.x+0.1f*newpos.x;
			this->Mesh->matrice(3,1)=0.9f*pos.y+0.1f*newpos.y;
			this->Mesh->matrice(3,2)=0.9f*pos.z+0.1f*newpos.z;
		}
		else
		{
			D3DVECTOR oy=D3DVEC(this->Mesh->matrice(1,0),this->Mesh->matrice(1,1),this->Mesh->matrice(1,2));
			oy=oy/~oy;
			float dot=oy*normale;
			if(dot<0)
				dot=dot;

			if(dot>0.5f)
			{
				D3DVECTOR axe1=0.9f*oy+0.1f*normale;
				axe1=axe1/~axe1;
				D3DVECTOR axe2=2*axe1-oy;
				axe2=axe2/~axe2;

				if(dot<1.0f)
				{
					D3DXMATRIX trans1,trans2,rot1,rot2;
					D3DXMatrixTranslation(&trans1,-pos.x,-pos.y,-pos.z);
					D3DXMatrixRotationAxis(&rot1,&D3DXVECTOR3(axe1),3.1415f);
					D3DXMatrixRotationAxis(&rot2,&D3DXVECTOR3(axe2),3.1415f);
					D3DXMatrixTranslation(&trans2,pos.x,pos.y,pos.z);
					
					this->Mesh->matrice*=trans1;
					this->Mesh->matrice*=rot1;
					this->Mesh->matrice*=rot2;
					this->Mesh->matrice*=trans2;
				}
				
				velocity=velocity-(velocity*axe2)*axe2;
				
				D3DVECTOR ox=D3DVEC(this->Mesh->matrice(0,0),this->Mesh->matrice(0,1),this->Mesh->matrice(0,2));
				
				D3DVECTOR grav_proj=((gravity*ox)/(ox*ox))*ox;
				
				this->Mesh->matrice(3,0)+=velocity.x+0.2f*grav_proj.x;
				this->Mesh->matrice(3,1)+=velocity.y+0.2f*grav_proj.y;
				this->Mesh->matrice(3,2)+=velocity.z+0.2f*grav_proj.z;

				pos=D3DVEC(this->Mesh->matrice(3,0),this->Mesh->matrice(3,1),this->Mesh->matrice(3,2));
				D3DVECTOR projete=pos-((pos-I)*normale)*normale;
				D3DVECTOR oy1=(this->Mesh->BoundingBox.points[0]-this->Mesh->BoundingBox.points[2])/2.0f;
				oy1=oy1*this->Mesh->matrice-pos;
				float dist=(float)fabs((oy1*normale));
				if (~(pos-projete)<dist)
					pos=projete+dist*normale;
				
				this->Mesh->matrice(3,0)=pos.x;
				this->Mesh->matrice(3,1)=pos.y;
				this->Mesh->matrice(3,2)=pos.z;

			} else //un mur

			{
				*state=1; //bounce
				this->Mesh->matrice(3,0)+=0.01f*normale.x;
				this->Mesh->matrice(3,1)+=0.01f*normale.y;
				this->Mesh->matrice(3,2)+=0.01f*normale.z;
				velocity=-0.5f*velocity;

				//velocity=(velocity+2*(velocity*normale)*normale);

			}	
		}
	} 

	contact|=contact1;

	if(!contact) //no collision		
	{
		this->Mesh->matrice(3,0)+=velocity.x+gravity.x;
		this->Mesh->matrice(3,1)+=velocity.y+gravity.y;
		this->Mesh->matrice(3,2)+=velocity.z+gravity.z;
	}		

	//velocity=velocity*0.99f;
}	

void CarClass::CheckClavier(BYTE * diks,DWORD DeltaT) // tableau de 256 valeurs
{
	if (this==NULL) return;
	if (this->State.car_state!=CAR_STATE_ALIFE)
		return;
	
	if(diks[this->Touches.Up]&0x80)
		this->Accelerer(DeltaT);
	if(diks[this->Touches.Down]&0x80)
		this->Decelerer(DeltaT);
	if(diks[this->Touches.Right]&0x80)
		this->TournerDroite(DeltaT);
	if(diks[this->Touches.Left]&0x80)
		this->TournerGauche(DeltaT);
}


void CarClass::SetClavier(BYTE Up,BYTE Down,BYTE Left,BYTE Right)
{
	if (this==NULL) return;
	this->Touches.Up=Up;
	this->Touches.Down=Down;
	this->Touches.Left=Left;
	this->Touches.Right=Right;	
}

void CarClass::SetMesh(PMeshStruct Mesh_)
{
	if (this==NULL) return;
	this->Mesh=Mesh_;
}



/////////////////////////////////////////////////////////////////////////
//    Celles de MeshManager
/////////////////////////////////////////////////////////////////////////


MeshManager::MeshManager ()
{
	this->Meshes=NULL;
	this->Lights=NULL;
	this->Balises=NULL;
	this->Cars=NULL;
	this->NumCars=0;
	this->NumMeshes=0;
	this->NumBalises=0;
	this->NumLights=0;

}

MeshManager::~MeshManager()
{
	if (this==NULL) return;
	this->DeleteAll();
}


void MeshManager::LockTransparency(float Factor)
{
	if (this==NULL) return;

	for(DWORD i=0;i<this->NumMeshes;i++)
		this->Meshes[i]->LockTransparency(Factor);

}

void MeshManager::UnlockTransparency()
{
	if (this==NULL) return;

	for(DWORD  i=0;i<this->NumMeshes;i++)
		this->Meshes[i]->UnlockTransparency();

}


void MeshManager::LockSpecular(float R,float G,float B,float Power)
{
	if (this==NULL) return;
		
	for(DWORD  i=0;i<this->NumMeshes;i++)
		this->Meshes[i]->LockSpecular(R,G,B,Power);

}

void MeshManager::UnlockSpecular()
{
	if (this==NULL) return;

	for(DWORD i=0;i<this->NumMeshes;i++)
		this->Meshes[i]->UnlockSpecular();
}




void MeshManager::Render(LPDIRECT3DDEVICE8 dev8,int EffectiveNumCars,CDirect3DDeviceUtility * pd3dDeviceUtility)
{
	if (this==NULL) return;
	if (this->Meshes==NULL) return;
	if (this->NumMeshes==0) return;
	int nc=0;
	for (DWORD i=0;i<this->NumMeshes;i++)
	{
		if(this->Meshes[i]->flag!=FLAG_CAR)
			this->Meshes[i]->Render(dev8,pd3dDeviceUtility);
		else
		{
			nc++;
			if (nc<=EffectiveNumCars)
			{
				switch(this->Cars[nc-1]->State.car_state)
				{
				case CAR_STATE_ALIFE:
					this->Meshes[i]->Render(dev8,pd3dDeviceUtility);
					break;
				case CAR_STATE_EXPLODING:
					
					break;
				case CAR_STATE_WIN_ANIM:
					
					break;
				}
			}
		}
	}
}



bool MeshManager::AddMeshFromFile(char * FileName,LPDIRECT3DDEVICE8 dev8)
{
	DWORD index;
	if (this==NULL) return false;
	if (this->NumMeshes==0) 
	{
		this->Meshes=(PMeshStruct*)malloc(sizeof(PMeshStruct));
		index=0;
		this->NumMeshes=1;
	}
	else
	{
		this->Meshes=(PMeshStruct*)realloc(this->Meshes,(this->NumMeshes+1)*sizeof(PMeshStruct));
		index=this->NumMeshes;
		this->NumMeshes++;
	}

	this->Meshes[index]=new MeshStruct();
	this->Meshes[index]->LoadFormFile(FileName,dev8);
	this->Meshes[index]->flag=FLAG_OBSTACLE;

	return true;
}

void MeshManager::DeleteMesh(DWORD index)
{
	if (index>=this->NumMeshes) return;
	
	PMeshStruct kk;

	kk=this->Meshes[this->NumMeshes-1];
	if (this->Meshes[index]) this->Meshes[index]->~MeshStruct();
	this->Meshes[index]=kk;


	if (this->NumMeshes>1)
	{
		this->Meshes=(PMeshStruct*)realloc(this->Meshes,(this->NumMeshes-1)*sizeof(PMeshStruct));
		this->NumMeshes--;
	}
	else
	{		
		free(this->Meshes);
		this->Meshes=NULL;
		this->NumMeshes=0;
	}	
}


void MeshManager::DeleteAll()
{
	DWORD i;
	for( i=0;i<this->NumMeshes;i++)
	{
		if(!this->Meshes[i]->UnLoad())
		{
			int a=0;
		}
		this->Meshes[i]->~MeshStruct();
		this->Meshes[i]=NULL;
	}

	free(this->Meshes);
	free(this->Lights);
	free(this->Balises);

	this->Meshes=NULL;
	this->Lights=NULL;
	this->Balises=NULL;
	this->NumMeshes=0;
	this->NumLights=0;
	this->NumBalises=0;

	for (i=0;i<this->NumCars;i++)
	{
		this->Cars[i]->~CarClass();
	}
	free(this->Cars);
	this->Cars=NULL;
	this->NumCars=0;
}


void MeshManager::UnloadMesh(DWORD index)
{
	if (index>=this->NumMeshes) return;
	if (this->Meshes==NULL) return;
	this->Meshes[index]->UnLoad();
}

void MeshManager::UnloadAll()
{
	if (this->Meshes==NULL) return;
	DWORD i;
	for ( i=0;i<this->NumMeshes;i++)
		this->Meshes[i]->UnLoad();

	for (i=0;i<this->NumCars;i++)
	{
		this->Cars[i]->~CarClass();
	}
	free(this->Cars);
	this->Cars=NULL;
	this->NumCars=0;
}

bool MeshManager::ReloadMesh(DWORD index,LPDIRECT3DDEVICE8 dev8)
{
	if (index>=this->NumMeshes) return false;
	if (this->Meshes==NULL) return false;
	return this->Meshes[index]->ReloadMesh(dev8);
}

bool MeshManager::ReloadAll(LPDIRECT3DDEVICE8 dev8)
{
	DWORD i;
	for (i=0;i<this->NumCars;i++)
	{
		this->Cars[i]->~CarClass();
	}
	free(this->Cars);
	this->Cars=NULL;
	this->NumCars=0;

	if (this->Meshes==NULL) return false;
	bool r=true;
	for ( i=0;i<this->NumMeshes;i++)
	{
		r&=this->Meshes[i]->ReloadMesh(dev8);
		if (this->Meshes[i]->flag==FLAG_CAR)
		{
			if (this->NumCars==0)
				this->Cars=(PCarClass*)malloc(sizeof(PCarClass));
			else
				this->Cars=(PCarClass*)realloc(this->Cars,(this->NumCars+1)*sizeof(PCarClass));

			this->NumCars++;
			this->Cars[this->NumCars-1]=new CarClass();
			this->Cars[this->NumCars-1]->SetMesh(this->Meshes[i]);
		}
	}

	return r;
}
		

void MeshManager::ComputeAllFaceInfo()
{
	if (this->Meshes==NULL) return;
	for(DWORD i=0;i<this->NumMeshes;i++)
	{
		this->Meshes[i]->ComputeFaceInfo();
		if(this->Meshes[i]->TransformedRadius<=0)
		{
			i=i;
		}
	}
}


void MeshManager::ComputeAllFaceInfo2(float Rayon)
{
	if (this->Meshes==NULL) return;
	for(DWORD i=0;i<this->NumMeshes;i++)
	{
		if(this->Meshes[i]->flag!=FLAG_CAR)
			this->Meshes[i]->ComputeFaceInfo2(Rayon);
	}
}



void MeshManager::CreateLights(LPDIRECT3DDEVICE8 dev8,CDirect3DDeviceUtility * pd3dDeviceUtility)
{
	for(DWORD i=0;i<NumLights;i++)
	{
		if(Lights[i].flag==3) //spot
				{
					D3DLIGHT8 l;
					l.Type=D3DLIGHT_SPOT;
					float A,R,G,B;
					A=(float)(((Lights[i].color & 0xff000000)>>24)/255.0);
					R=(float)(((Lights[i].color & 0x00ff0000)>>16)/255.0);
					G=(float)(((Lights[i].color & 0x0000ff00)>>8)/255.0);
					B=(float)((Lights[i].color & 0x000000ff)/255.0);
					l.Diffuse.r=R;l.Diffuse.g=G;l.Diffuse.b=B;l.Diffuse.a=A;
					l.Ambient=l.Diffuse;
					l.Specular=l.Diffuse;
					l.Direction=Lights[i].dir;
					l.Position=Lights[i].pos;
					l.Range=Lights[i].range;
					l.Attenuation0=Lights[i].a0;l.Attenuation1=Lights[i].a1;l.Attenuation2=Lights[i].a2;
					l.Theta=Lights[i].umbra;
					l.Phi=Lights[i].penumbra;

					l.Falloff=1.0;

					pd3dDeviceUtility->SetLight(i,&l);
					pd3dDeviceUtility->LightEnable(i,true);
				}	
				else if(Lights[i].flag==4) //point
				{
					D3DLIGHT8 l;
					l.Type=D3DLIGHT_POINT;
					float A,R,G,B;
					A=(float)(((Lights[i].color & 0xff000000)>>24)/255.0);
					R=(float)(((Lights[i].color & 0x00ff0000)>>16)/255.0);
					G=(float)(((Lights[i].color & 0x0000ff00)>>8)/255.0);
					B=(float)((Lights[i].color & 0x000000ff)/255.0);
					l.Diffuse.r=R;l.Diffuse.g=G;l.Diffuse.b=B;l.Diffuse.a=A;
					l.Ambient=l.Diffuse;
					l.Specular=l.Diffuse;
					l.Position=Lights[i].pos;
					l.Range=Lights[i].range;
					l.Attenuation0=Lights[i].a0;l.Attenuation1=Lights[i].a1;l.Attenuation2=Lights[i].a2;

					pd3dDeviceUtility->SetLight(i,&l);
					pd3dDeviceUtility->LightEnable(i,true);
				}
				else if(Lights[i].flag==5) //directional
				{
					D3DLIGHT8 l;
					l.Type=D3DLIGHT_DIRECTIONAL;
					float A,R,G,B;
					A=(float)(((Lights[i].color & 0xff000000)>>24)/255.0);
					R=(float)(((Lights[i].color & 0x00ff0000)>>16)/255.0);
					G=(float)(((Lights[i].color & 0x0000ff00)>>8)/255.0);
					B=(float)((Lights[i].color & 0x000000ff)/255.0);
					l.Diffuse.r=R;l.Diffuse.g=G;l.Diffuse.b=B;l.Diffuse.a=A;
					l.Ambient=l.Diffuse;
					l.Specular=l.Diffuse;
					l.Direction=Lights[i].dir;


					pd3dDeviceUtility->SetLight(i,&l);
					pd3dDeviceUtility->LightEnable(i,true);
				}
	}
}

void CreateNewCar(PCarClass * CC)
{
	*CC=new CarClass();
}

bool MeshManager::LoadFromSTGFile(char* FileName,LPDIRECT3DDEVICE8 dev8,CDirect3DDeviceUtility * pd3dDeviceUtility )
{
	FILE*f=NULL;
	char * Path=GetPath(FileName);
	__try
	{
		f=fopen(FileName,"rb");
		if (f==NULL) return false;

		this->DeleteAll();

		DWORD num_Obj;

		fread(&num_Obj,4,1,f);

		DWORD i;
		for ( i=0;i<num_Obj;i++)
		{
			DWORD j;
			if(i==99)
				i=99;

			fread(&j,4,1,f);
			char * nomfic;
			nomfic=(char*)malloc(j+1);
			fread(nomfic,j+1,1,f);
			nomfic=Text_Concat(Path,nomfic);

			D3DXMATRIX mat;
			for(DWORD m=0;m<4;m++)
			for(DWORD k=0;k<4;k++)
			{
				fread(&mat(k,m),sizeof(mat(k,m)),1,f);
			}
			FLAGS flag;
			fread(&flag,4,1,f);

			if((flag<3) || (flag==FLAG_NOTHING)) //mesh
			{
				if (this->AddMeshFromFile(nomfic,dev8)==false)
				{
					fclose(f);
					this->DeleteAll();
					return false;
				}
				this->Meshes[this->NumMeshes-1]->flag=flag;
				for(DWORD m=0;m<4;m++)
					for(DWORD k=0;k<4;k++)
					{
						this->Meshes[this->NumMeshes-1]->matrice(k,m)=mat(k,m);
					}

				if (flag==FLAG_CAR)
				{
					if (this->NumCars==0)
						this->Cars=(PCarClass*)malloc(sizeof(PCarClass));
					else
						this->Cars=(PCarClass*)realloc(this->Cars,(this->NumCars+1)*sizeof(PCarClass));

					this->NumCars++;
					CreateNewCar(&this->Cars[this->NumCars-1]);
					this->Cars[this->NumCars-1]->SetMesh(this->Meshes[this->NumMeshes-1]);
				}

			}
			else
			if(flag==6) //balise
			{
				if (this->NumBalises==0) 
				{
					this->Balises=(BALISE*)malloc(sizeof(BALISE));
					this->NumBalises=1;
				}
				else
				{
					this->Balises=(BALISE*)realloc(this->Balises,(this->NumBalises+1)*sizeof(BALISE));
					this->NumBalises++;
				}

				Balises[NumBalises-1].pos=D3DVEC(mat(3,0),mat(3,1),mat(3,2));
				fread(&Balises[NumBalises-1].num,sizeof(Balises[NumBalises-1].num),1,f);

			}
			else //light
			{
				if (this->NumLights==0) 
				{
					this->Lights=(light*)malloc(sizeof(light));
					this->NumLights=1;
				}
				else
				{
					this->Lights=(light*)realloc(this->Lights,(this->NumLights+1)*sizeof(light));
					this->NumLights++;
				}
				Lights[NumLights-1].pos=D3DVEC(mat(3,0),mat(3,1),mat(3,2));
				Lights[NumLights-1].dir=D3DVEC(mat(2,0),mat(2,1),mat(2,2));
				Lights[NumLights-1].flag=flag;
				fread(&Lights[NumLights-1].color,sizeof(Lights[NumLights-1].color),1,f);
				fread(&Lights[NumLights-1].range,sizeof(Lights[NumLights-1].range),1,f);
				fread(&Lights[NumLights-1].umbra,sizeof(Lights[NumLights-1].umbra),1,f);
				fread(&Lights[NumLights-1].penumbra,sizeof(Lights[NumLights-1].penumbra),1,f);
				fread(&Lights[NumLights-1].a0,sizeof(Lights[NumLights-1].a0),1,f);
				fread(&Lights[NumLights-1].a1,sizeof(Lights[NumLights-1].a1),1,f);
				fread(&Lights[NumLights-1].a2,sizeof(Lights[NumLights-1].a2),1,f);
	
			}


		}

		//sort the "balises"
		if(NumBalises)
		{
			for(DWORD j=0;j<this->NumBalises-1;j++)
				for(i=j+1;i<this->NumBalises;i++)
				{
					if(this->Balises[i].num<this->Balises[j].num)
					{
						BALISE temp;
						temp=Balises[i];
						Balises[i]=Balises[j];
						Balises[j]=temp;
					}
				}
		}
		//create the lights
		CreateLights(dev8,pd3dDeviceUtility);


		fclose(f);f=NULL;
	}
	__except (1) // always (i.e.  toujours) exécuter le Handler ci dessous
	{
		if (f!=NULL) fclose(f);f=NULL;
		this->DeleteAll();
		return false;
	}
		
	return true;
}


void MeshManager::ComputeBaliseInfo(float _alpha,float _d)
{
	this->alpha=_alpha;
	this->d=_d;
	this->cos_alpha=(float)cos(_alpha);
	this->d_2_tan_alpha=d/(2.0f*float(tan(_alpha)));

	for(DWORD i=0;i<this->NumBalises;i++)
	{
		this->Balises[i].index_next=(i+1) % this->NumBalises;
		this->Balises[i].N=this->Balises[Balises[i].index_next].pos
			              -this->Balises[i].pos;
		this->Balises[i].N=this->Balises[i].N/~this->Balises[i].N;
		this->Balises[i].K=this->Balises[i].pos-this->d_2_tan_alpha*this->Balises[i].N;
	}	
}


void MeshManager::Scale(float factor,LPDIRECT3DDEVICE8 dev8)
{
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale,factor,factor,factor);
	
	DWORD i;
	for( i=0;i<this->NumMeshes;i++)
	{
		this->Meshes[i]->matrice*=scale;
	}

	for(i=0;i<this->NumBalises;i++)
		this->Balises[i].pos=factor*this->Balises[i].pos;

	for(i=0;i<this->NumLights;i++)
		this->Lights[i].pos=factor*this->Lights[i].pos;

}

// pour les voitures
void MeshManager::CheckClavier(DWORD EffectiveNumCars,BYTE * diks,DWORD DeltaT) // tableau de 256 valeurs
{
	for(DWORD i=0;i<EffectiveNumCars;i++)
		this->Cars[i]->CheckClavier(diks,DeltaT);
}


void MeshManager::UpdatePosition(DWORD EffectiveNumCars,BYTE *state,DWORD DeltaT) // fait les collisions entre les voitures
{
	*state=0;
	DWORD i,j;
	for(i=0;(i<EffectiveNumCars-1)&&(i<NumCars);i++)
	{
		if (Cars[i]->State.car_state!=CAR_STATE_ALIFE) break;
 		D3DVECTOR pos1=D3DVEC(Cars[i]->Mesh->matrice(3,0),Cars[i]->Mesh->matrice(3,1),Cars[i]->Mesh->matrice(3,2));

		D3DVECTOR ox=(Cars[i]->Mesh->BoundingBox.points[0]-Cars[i]->Mesh->BoundingBox.points[1])/2.0f;
		ox=ox*Cars[i]->Mesh->matrice-pos1;
		D3DVECTOR oy=(Cars[i]->Mesh->BoundingBox.points[0]-Cars[i]->Mesh->BoundingBox.points[2])/2.0f;
		oy=oy*Cars[i]->Mesh->matrice-pos1;
		D3DVECTOR oz=(Cars[i]->Mesh->BoundingBox.points[0]-Cars[i]->Mesh->BoundingBox.points[4])/2.0f;
		oz=oz*Cars[i]->Mesh->matrice-pos1;
			
		for(j=i+1;j<EffectiveNumCars;j++)
		{
			if (Cars[j]->State.car_state!=CAR_STATE_ALIFE) break;
			D3DVECTOR pos2=D3DVEC(Cars[j]->Mesh->matrice(3,0),Cars[j]->Mesh->matrice(3,1),Cars[j]->Mesh->matrice(3,2));
			D3DVECTOR vec=pos2-pos1;
			float dist=~vec;
			vec=vec/dist;			

			D3DVECTOR ox1=(Cars[j]->Mesh->BoundingBox.points[0]-Cars[j]->Mesh->BoundingBox.points[1])/2.0f;
			ox1=ox1*Cars[j]->Mesh->matrice-pos2;
			D3DVECTOR oy1=(Cars[j]->Mesh->BoundingBox.points[0]-Cars[j]->Mesh->BoundingBox.points[2])/2.0f;
			oy1=oy1*Cars[j]->Mesh->matrice-pos2;
			D3DVECTOR oz1=(Cars[j]->Mesh->BoundingBox.points[0]-Cars[j]->Mesh->BoundingBox.points[4])/2.0f;
			oz1=oz1*Cars[j]->Mesh->matrice-pos2;
			
			float dist1=(float)sqrt(sqr(ox*vec)+sqr(oy*vec)+sqr(oz*vec));
			float dist2=(float)sqrt(sqr(ox1*vec)+sqr(oy1*vec)+sqr(oz1*vec));

			if(dist<=dist1+dist2)
			{
				
				D3DVECTOR ppp=pos2-pos1;
				ppp=ppp/dist;
				
				Cars[i]->velocity=Cars[i]->velocity-2*(Cars[i]->velocity*ppp)*ppp;
				Cars[j]->velocity=Cars[j]->velocity-2*(Cars[j]->velocity*ppp)*ppp;

				
				D3DVECTOR vel1=Cars[i]->velocity;
				D3DVECTOR vel2=Cars[j]->velocity;

				Cars[i]->velocity=-0.8f*vel2+0.2f*vel1;
				Cars[j]->velocity=-0.8f*vel1+0.2f*vel2;


				if(Cars[i]->velocity*ppp>0)
					Cars[i]->velocity=-1*Cars[i]->velocity;

				if(Cars[j]->velocity*ppp<0)
					Cars[j]->velocity=-1*Cars[j]->velocity;


				Cars[i]->Mesh->matrice(3,0)-=ppp.x*Cars[i]->Mesh->TransformedRadius*0.01f;
				Cars[i]->Mesh->matrice(3,1)-=ppp.y*Cars[i]->Mesh->TransformedRadius*0.01f;
				Cars[i]->Mesh->matrice(3,2)-=ppp.z*Cars[i]->Mesh->TransformedRadius*0.01f;
				
				Cars[j]->Mesh->matrice(3,0)+=ppp.x*Cars[j]->Mesh->TransformedRadius*0.01f;
				Cars[j]->Mesh->matrice(3,1)+=ppp.y*Cars[j]->Mesh->TransformedRadius*0.01f;
				Cars[j]->Mesh->matrice(3,2)+=ppp.z*Cars[j]->Mesh->TransformedRadius*0.01f;		
				*state=2;
			}
		}
	}

	for(i=0;(i<EffectiveNumCars)&&(i<NumCars);i++)
	{
		if((this->Cars[i]->ContactTimer>1000.0f) && (this->Cars[i]->State.s2==0.0f))
		{
			this->Cars[i]->ContactTimer=0;
			this->Cars[i]->State.car_state=CAR_STATE_EXPLODING;
			this->Cars[i]->State.sub_state=0;
			this->Cars[i]->State.s1=0;
			this->Cars[i]->State.s2=1.0;
			return;
		}
		else if((this->Cars[i]->State.s2==1.0f) && (this->Cars[i]->State.car_state!=CAR_STATE_EXPLODING))
		{
			Cars[i]->Mesh->matrice(3,0)=
				(Balises[Cars[i]->balise].pos.x+2.5f*(i%2)*Cars[i]->Mesh->TransformedRadius);
			Cars[i]->Mesh->matrice(3,1)=
				(Balises[Cars[i]->balise].pos.y+2*Cars[i]->Mesh->TransformedRadius);
			Cars[i]->Mesh->matrice(3,2)=
				(Balises[Cars[i]->balise].pos.z+2.5f*(i/2)*Cars[i]->Mesh->TransformedRadius);

			this->Cars[i]->velocity=D3DVEC(0.0f,0.0f,0.0f);
			D3DVECTOR orientation=this->Balises[this->Balises[this->Cars[i]->balise].index_next].pos-this->Balises[this->Cars[i]->balise].pos;
			orientation=orientation/~orientation;

			//orientation selon les balises
			D3DVECTOR ox=D3DVEC(this->Cars[i]->Mesh->matrice(0,0),this->Cars[i]->Mesh->matrice(0,1),this->Cars[i]->Mesh->matrice(0,2));
			D3DVECTOR oy=D3DVEC(this->Cars[i]->Mesh->matrice(1,0),this->Cars[i]->Mesh->matrice(1,1),this->Cars[i]->Mesh->matrice(1,2));
			D3DVECTOR oz=D3DVEC(this->Cars[i]->Mesh->matrice(2,0),this->Cars[i]->Mesh->matrice(2,1),this->Cars[i]->Mesh->matrice(2,2));
			ox=~ox*orientation;
			D3DVECTOR oy1=(ox^oy)^ox;
			oy=(~oy/~oy1)*oy1;
			D3DVECTOR oz1=ox^oy;
			oz=(~oz/~oz1)*oz1;

			this->Cars[i]->Mesh->matrice(0,0)=ox.x;
			this->Cars[i]->Mesh->matrice(0,1)=ox.y;
			this->Cars[i]->Mesh->matrice(0,2)=ox.z;
			this->Cars[i]->Mesh->matrice(1,0)=oy.x;
			this->Cars[i]->Mesh->matrice(1,1)=oy.y;
			this->Cars[i]->Mesh->matrice(1,2)=oy.z;
			this->Cars[i]->Mesh->matrice(2,0)=oz.x;
			this->Cars[i]->Mesh->matrice(2,1)=oz.y;
			this->Cars[i]->Mesh->matrice(2,2)=oz.z;
			this->Cars[i]->State.s2=0.0;
			this->Cars[i]->ContactTimer=0;
		}				

		if (this->Cars[i]->State.car_state==CAR_STATE_ALIFE)
		{
			this->Cars[i]->UpdatePosition(this,state);

			if(this->Cars[i]->contact) 
				this->Cars[i]->ContactTimer=0;
			 else
				this->Cars[i]->ContactTimer+=DeltaT;
		}
	}
}

#endif