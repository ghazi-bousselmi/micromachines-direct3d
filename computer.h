#ifndef COMPUTER_INCLUDED
#define COMPUTER_INCLUDED

#include "MeshUtils.h"

typedef enum COM_STATE
{
	COM_STATE_RACING,
	COM_STATE_AVOIDING_OBSTACLE,
	COM_STATE_AVOIDING_CAR
} COM_STATE;


typedef struct
{
	COM_STATE com_state;
	DWORD avoided_index; // n° de la voiture ou de l'obstacle à éviter
	
	// si on est en train de suivre un chemin particulier 
	D3DVECTOR RallyPoints[10];
	DWORD nRallyPoints;
} Temp_State;


typedef class CCOMPUTER
{
	private:
		void TestCollisionCars(PMeshManager MONDE,PCarClass MyCar,DWORD EffectiveNumberOfPlayers);
		void TestCollisionMonde(PMeshManager MONDE,PCarClass MyCar);
		bool TestWorldLimit(PMeshManager MONDE,PCarClass MyCar,float *dist);
		void Avancer(PMeshManager MONDE,PCarClass MyCar,DWORD DeltaT,D3DVECTOR nextpos);
		void TestCollisionMondeWhileAvoiding(PMeshManager MONDE,PCarClass MyCar);

		int Intelligence;

	public:
		Temp_State state;
		
		void Init(PMeshManager MONDE,int _Intelligence);
		void Advance(DWORD DeltaT,PMeshManager MONDE,PCarClass MyCar,DWORD EffectiveNumberOfPlayers);
		
		CCOMPUTER();
		~CCOMPUTER();
} CCOMPUTER,*PCCOMPUTER;


CCOMPUTER::CCOMPUTER()
{
	//this->state
}

CCOMPUTER::~CCOMPUTER()
{

}


void CCOMPUTER::Init(PMeshManager MONDE,int _Intelligence)
{
	this->state.nRallyPoints=0;
	this->state.com_state=COM_STATE_RACING;
	Intelligence=abs(_Intelligence) % 4;
}



void CCOMPUTER::TestCollisionCars(PMeshManager MONDE,PCarClass MyCar,DWORD EffectiveNumberOfPlayers)
{
	PCarClass ACar;
	D3DVECTOR pos,nextpos,pos1;
	nextpos=MONDE->Balises[(MyCar->balise+1)% MONDE->NumBalises].pos;

	pos=D3DVEC(MyCar->Mesh->matrice(3,0),
			   MyCar->Mesh->matrice(3,1),
			   MyCar->Mesh->matrice(3,2)
			  );

	D3DVECTOR direction=nextpos-pos;
	direction=direction/~direction;

	bool found=false;
	int  index=0;
	float mindist=1e30f;

	
	for (DWORD i=0;i<EffectiveNumberOfPlayers;i++)
	{
		ACar=MONDE->Cars[i];
		if (ACar!=MyCar)
		{
			pos1=D3DVEC(MONDE->Cars[i]->Mesh->matrice(3,0),
						MONDE->Cars[i]->Mesh->matrice(3,1),
						MONDE->Cars[i]->Mesh->matrice(3,2)
						);


			D3DVECTOR tempvec=(pos1-pos);
			float dist=tempvec*direction;
			if (dist>=0)
			{
				
				float dist1=(float)sqrt(tempvec*tempvec-dist*dist);
				if (dist1<=(0.75*MyCar->Mesh->TransformedRadius+0.75*ACar->Mesh->TransformedRadius))
				{
					
					
					float tempf1,tempf2;
					tempf1=ACar->velocity*direction;
					tempf2=MyCar->velocity*direction;
					if (tempf1<tempf2)
					{
						// ca y est , là cette voiture peut nous géner, il faut la contourner
						if (!found)
						{
							found=true;
							mindist=dist;
							index=i;
						}
						else
						{
							if (mindist>=dist)
							{
								mindist=dist;
								index=i;
							}
						}

					} // tempf1<tempf2
				}//if (dist1<= ...
			}//if (dist>=0)
		}//if (ACar!=MyCar)
	} // for i;

	
	
	if (found)
	{
		this->state.avoided_index=index;
		this->state.com_state=COM_STATE_AVOIDING_CAR;
						
		pos1=D3DVEC(
					MONDE->Cars[index]->Mesh->matrice(3,0),
					MONDE->Cars[index]->Mesh->matrice(3,1),
					MONDE->Cars[index]->Mesh->matrice(3,2)
				   );

		D3DVECTOR tempvec,tv1,tv2;
		float dist=(pos1-pos)*direction;
		tempvec=(pos1-pos)-dist*direction;
		tv1=-1*(tempvec/~tempvec);
		tv2=pos+tv1*2*MyCar->Mesh->TransformedRadius+tempvec;				

		this->state.RallyPoints[1]=nextpos;
		this->state.RallyPoints[0]=tv2;
		this->state.nRallyPoints=2;
	}
}



void CCOMPUTER::TestCollisionMonde(PMeshManager MONDE,PCarClass MyCar)
{
	// intelligence >= 1  ,  >= Easy
	D3DVECTOR pos,nextpos;
	nextpos=MONDE->Balises[(MyCar->balise+1)% MONDE->NumBalises].pos;

	pos=D3DVEC(MyCar->Mesh->matrice(3,0),
			   MyCar->Mesh->matrice(3,1),
			   MyCar->Mesh->matrice(3,2)
			  );

	D3DVECTOR direction=nextpos-pos;
	direction=direction/~direction;

	bool found=false;
	DWORD	*result=NULL;
	DWORD	nresult=0;
	DWORD   i;



	for (i=0;i<MONDE->NumMeshes;i++)
	{
		if (MONDE->Meshes[i]->flag==FLAG_OBSTACLE)
		{
			D3DVECTOR C=D3DVEC(MONDE->Meshes[i]->matrice(3,0),MONDE->Meshes[i]->matrice(3,1),MONDE->Meshes[i]->matrice(3,2));
			float dist=(C-pos)*direction;
			float	dist0=~(pos-nextpos);
			if ( (dist>0) && (dist<=10*MyCar->Mesh->TransformedRadius)
				&&(dist<1.3f*dist0)
				&&(sqrt((pos-C)*(pos-C)-dist*dist)<MONDE->Meshes[i]->TransformedRadius+MyCar->Mesh->TransformedRadius)
				)
			{
				if(!found)
				{
					result=(DWORD*)malloc(sizeof(DWORD));
					result[0]=i;
					nresult++;
				}
				else
				{
					result=(DWORD*)realloc(result,sizeof(DWORD)*(nresult+1));
					result[nresult]=i;
					nresult++;
				}

				found=true;
			}
		}
	}

	if(found)
	{
		float	mindist=1e30f;
		DWORD	minindex=0;
		for(i=0;i<nresult;i++)
		{
			D3DVECTOR C=D3DVEC(MONDE->Meshes[result[i]]->matrice(3,0),MONDE->Meshes[result[i]]->matrice(3,1),MONDE->Meshes[result[i]]->matrice(3,2));
			float dist=(C-pos)*direction;
			if((dist<mindist)&&(dist>0))
			{
				mindist=dist;
				minindex=result[i];
			}															

		}

		D3DVECTOR C0=D3DVEC(MONDE->Meshes[minindex]->matrice(3,0),MONDE->Meshes[minindex]->matrice(3,1),MONDE->Meshes[minindex]->matrice(3,2));
		D3DVECTOR C2;
		D3DVECTOR	K=D3DVEC(0,1,0)^(C0-pos);
		K=K/~K;

		
		D3DVECTOR	temp=(C0-pos)^direction;
		if(temp.y<0)
			K=-1*K;
		C2=C0+K*1.5f*(MONDE->Meshes[minindex]->TransformedRadius+MyCar->Mesh->TransformedRadius);				
		
		this->state.avoided_index=minindex;
		this->state.com_state=COM_STATE_AVOIDING_OBSTACLE;
	
		
		// intelligence>=1   >=Easy
		if (this->Intelligence==3) // good
		{
			this->state.RallyPoints[0]=C0+((pos+C2)/2-C0)*1.3f;
			this->state.RallyPoints[1]=C2;								
			this->state.RallyPoints[2]=C0+((nextpos+C2)/2-C0)*1.2f;;
			this->state.RallyPoints[2]=nextpos;		
			this->state.nRallyPoints=3;
		}
		else
		{		
			this->state.RallyPoints[0]=C2;								
			this->state.RallyPoints[1]=nextpos;		
			this->state.nRallyPoints=2;
		}		
		
		free(result);
	}
}


void CCOMPUTER::TestCollisionMondeWhileAvoiding(PMeshManager MONDE,PCarClass MyCar)
{
	// intelligence >= 1  ,  >= Easy
	D3DVECTOR pos,nextpos;
	nextpos=MONDE->Balises[(MyCar->balise+1)% MONDE->NumBalises].pos;

	pos=D3DVEC(MyCar->Mesh->matrice(3,0),
			   MyCar->Mesh->matrice(3,1),
			   MyCar->Mesh->matrice(3,2)
			  );

	D3DVECTOR direction=nextpos-pos;
	direction=direction/~direction;

	bool found=false;
	DWORD	*result=NULL;
	DWORD	nresult=0;
	DWORD   i;



	for (i=0;i<MONDE->NumMeshes;i++)
	{
		if (i!=this->state.avoided_index)
		if (MONDE->Meshes[i]->flag==FLAG_OBSTACLE)
		{
			D3DVECTOR C=D3DVEC(MONDE->Meshes[i]->matrice(3,0),MONDE->Meshes[i]->matrice(3,1),MONDE->Meshes[i]->matrice(3,2));
			float dist=(C-pos)*direction;
			float	dist0=~(pos-nextpos);
			if ( (dist>0) && (dist<=10*MyCar->Mesh->TransformedRadius)
				&&(dist<1.3f*dist0)
				&&(sqrt((pos-C)*(pos-C)-dist*dist)<MONDE->Meshes[i]->TransformedRadius+MyCar->Mesh->TransformedRadius)
				)
			{
				if(!found)
				{
					result=(DWORD*)malloc(sizeof(DWORD));
					result[0]=i;
					nresult++;
				}
				else
				{
					result=(DWORD*)realloc(result,sizeof(DWORD)*(nresult+1));
					result[nresult]=i;
					nresult++;
				}

				found=true;
			}
		}
	}

	if(found)
	{
		float	mindist=1e30f;
		DWORD	minindex=0;
		for(i=0;i<nresult;i++)
		{
			D3DVECTOR C=D3DVEC(MONDE->Meshes[result[i]]->matrice(3,0),MONDE->Meshes[result[i]]->matrice(3,1),MONDE->Meshes[result[i]]->matrice(3,2));
			float dist=(C-pos)*direction;
			if((dist<mindist)&&(dist>0))
			{
				mindist=dist;
				minindex=result[i];
			}
		}

		D3DVECTOR KKK=D3DVEC(MONDE->Meshes[state.avoided_index]->matrice(3,0),MONDE->Meshes[state.avoided_index]->matrice(3,1),MONDE->Meshes[state.avoided_index]->matrice(3,2));
		/*
		if(~(kkk-pos)<mindist)
			return;
		*/


		D3DVECTOR C0=D3DVEC(MONDE->Meshes[minindex]->matrice(3,0),MONDE->Meshes[minindex]->matrice(3,1),MONDE->Meshes[minindex]->matrice(3,2));
		D3DVECTOR C2;
		D3DVECTOR	K=D3DVEC(0,1,0)^(C0-pos);
		K=K/~K;

		
		D3DVECTOR	temp=(C0-pos)^direction;
		if(temp.y<0)
			K=-1*K;
		C2=C0+K*1.5f*(MONDE->Meshes[minindex]->TransformedRadius+MyCar->Mesh->TransformedRadius);				
		
		this->state.avoided_index=minindex;
		this->state.com_state=COM_STATE_AVOIDING_OBSTACLE;
	
		
		// intelligence>=1   >=Easy
		if (this->Intelligence==3) // good
		{
			this->state.RallyPoints[0]=C0+((pos+C2)/2-C0)*1.3f;
			this->state.RallyPoints[1]=C2;								
			this->state.RallyPoints[2]=C0+((nextpos+C2)/2-C0)*1.3f;
			this->state.RallyPoints[3]=nextpos;		
			this->state.nRallyPoints=4;
		}
		else
		{		
			this->state.RallyPoints[0]=C2;								
			this->state.RallyPoints[1]=nextpos;		
			this->state.nRallyPoints=2;
		}		
		
		free(result);
	}
}





bool CCOMPUTER::TestWorldLimit(PMeshManager MONDE,PCarClass MyCar,float *dist)
{

	// intelligence >= 2 , medium
	D3DVECTOR pos;
	pos=D3DVEC(MyCar->Mesh->matrice(3,0),
			   MyCar->Mesh->matrice(3,1),
			   MyCar->Mesh->matrice(3,2)
			  );

	D3DVECTOR vel=MyCar->velocity;
	vel=vel/~vel;



	if(this->Intelligence==2) // medium
	{
		for(DWORD i=0;i<MONDE->NumMeshes;i++)
		{
			if(MONDE->Meshes[i]->flag==FLAG_TERRAIN)
			{
				int X,Y,Z;
				MONDE->Meshes[i]->GetCubeCoords(pos,&X,&Y,&Z);
				if(
					   (X<=1)
					||(X>=2*(int)MONDE->Meshes[i]->faceinfo2.NCubesX-2)
					||(Z<=1)
					||(Z>=2*(int)MONDE->Meshes[i]->faceinfo2.NCubesZ-2)
				   )
				{
					*dist=0;
					return true;
				}
			}
		}
	}


	if(this->Intelligence==3) // good
	{
		for(DWORD i=0;i<MONDE->NumMeshes;i++)
		{
			if(MONDE->Meshes[i]->flag==FLAG_TERRAIN)
			{
				int X,Y,Z;
				MONDE->Meshes[i]->GetCubeCoords(pos,&X,&Y,&Z);
			
				if(X<=3)
				{
					if(vel.x<-0.2f)
					{
						*dist=0;
						return true;
					}
				}

				if(X>=2*(int)MONDE->Meshes[i]->faceinfo2.NCubesX-4)
				{
					if(vel.x>0.2f)
					{
						*dist=0;
						return true;
					}
				}

				if(Z<=3)
				{
					if(vel.z<-0.2f)
					{
						*dist=0;
						return true;
					}
				}

				if(Z>=2*(int)MONDE->Meshes[i]->faceinfo2.NCubesZ-4)
				{
					if(MyCar->velocity.z>0.2)
					{
						*dist=0;
						return true;
					}
				}
			}
		}
	}

	return false;
}

void CCOMPUTER::Avancer(PMeshManager MONDE,PCarClass MyCar,DWORD DeltaT,D3DVECTOR nextpos)
{
	D3DVECTOR pos;
	pos=D3DVEC(MyCar->Mesh->matrice(3,0),
			   MyCar->Mesh->matrice(3,1),
			   MyCar->Mesh->matrice(3,2)
			  );

	D3DVECTOR direction=nextpos-pos;
	direction=direction/~direction;

	
	float dist;

	MyCar->Accelerer(DeltaT);

	if(this->Intelligence>=2) // >= Medium
		if(this->TestWorldLimit(MONDE,MyCar,&dist))
			MyCar->Decelerer(DeltaT);

	D3DVECTOR ox=D3DVEC(MyCar->Mesh->matrice(0,0),MyCar->Mesh->matrice(0,1),MyCar->Mesh->matrice(0,2));
	ox=ox/~ox;
	
	D3DVECTOR V=ox^direction;
	if (V.y>0)
	{
		MyCar->TournerDroite(DeltaT);
	}
	else if (V.y<0)
	{
		MyCar->TournerGauche(DeltaT);
	}
}



void CCOMPUTER::Advance(DWORD DeltaT,PMeshManager MONDE,PCarClass MyCar,DWORD EffectiveNumberOfPlayers)
{
	if(Intelligence==0) // très stupide
	{
		D3DVECTOR pos=D3DVEC(MyCar->Mesh->matrice(3,0),
			   MyCar->Mesh->matrice(3,1),
			   MyCar->Mesh->matrice(3,2)
			  );
		D3DVECTOR nextpos=MONDE->Balises[(MyCar->balise+1)% MONDE->NumBalises].pos;

		D3DVECTOR direction=nextpos-pos;
		direction=direction/~direction;
	
		D3DVECTOR ox=D3DVEC(MyCar->Mesh->matrice(0,0),MyCar->Mesh->matrice(0,1),MyCar->Mesh->matrice(0,2));
		ox=ox/~ox;
		D3DVECTOR temp=ox^direction;

		if(temp.y>0) 
			MyCar->TournerDroite(DeltaT);
		else if(temp.y<0)
			MyCar->TournerGauche(DeltaT);

		MyCar->Accelerer(DeltaT);
	}
	else
	{
		switch(this->state.com_state)
		{
		case COM_STATE_RACING:
			{
				this->TestCollisionMonde(MONDE,MyCar);
				this->Avancer(MONDE,MyCar,DeltaT,MONDE->Balises[(MyCar->balise+1)% MONDE->NumBalises].pos);
				break;
			}	
		case COM_STATE_AVOIDING_OBSTACLE:
		case COM_STATE_AVOIDING_CAR:
			{	
				
				D3DVECTOR pos,nextpos;
				nextpos=this->state.RallyPoints[0];
				
				pos=D3DVEC(MyCar->Mesh->matrice(3,0),
						   MyCar->Mesh->matrice(3,1),
						   MyCar->Mesh->matrice(3,2)
						  );

				// if we are here, intelligence>=1 , >= easy
				this->Avancer(MONDE,MyCar,DeltaT,nextpos);
				
				pos.y=0;
				nextpos.y=0;
				if (~(pos-nextpos)<=5*MyCar->Mesh->TransformedRadius)
				{
					for (DWORD i=0;i<this->state.nRallyPoints-1;i++)
					{
						this->state.RallyPoints[i]=this->state.RallyPoints[i+1];
					}

					this->state.nRallyPoints--;
					if (this->state.nRallyPoints<=0)
					{
						this->state.com_state=COM_STATE_RACING;
					}
				}

				if (Intelligence==3) // good
					this->TestCollisionMondeWhileAvoiding(MONDE,MyCar);

				break;
			}	
		} // switch (state)
	} // else
}


#endif
