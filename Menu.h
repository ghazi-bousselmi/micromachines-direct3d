#ifndef Menu_Included
#define Menu_Included

#include "stdio.h"
#include "3dutils.h"
#include "textutil.h"
#define D3DXOVERLOADS
#include "d3dx8.h"


class MENU;

// pour les bitmaps affichées
typedef struct
{
	char * filename;
	D3DVECTOR min,max;
} Bitmap;

typedef struct
{
	char* ItemText;
	D3DVECTOR Center;
	DWORD size; // taille du caractère          
	                                  // milli.sec.
	
	D3DXMATRIX GetCharMatrix(DWORD DeltaT, 
		                     DWORD CharIndex,
			    			 DWORD Size,
							 D3DVECTOR Center);
	
	MENU* submenu;
} ItemStruct;

D3DXMATRIX ItemStruct::GetCharMatrix(DWORD DeltaT, 
		                     DWORD CharIndex,
			    			 DWORD Size,
							 D3DVECTOR Center)
{
	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	return m;
}



typedef class MENU
{
	private:
		LPDIRECT3DDEVICE8 dev8;
		CDirect3DDeviceUtility *m_pd3dDeviceUtility;
	public:
		ItemStruct MenuTitle;
		bool active; // indique si le menu est activé ou non
		DWORD index_focused; // le item focused

		DWORD NumItems;
		DWORD NumBitmaps;
		ItemStruct*items;
		Bitmap*bitmaps;
		LPDIRECT3DTEXTURE8 * textures;
		
		
		void AddTextItem(ItemStruct item);

		// cree le menu
		MENU*AddMenuItem(ItemStruct item,ItemStruct SubMenuTitle);

		void AddBitmap(Bitmap bitmap);

		HRESULT InvalidateDeviceObjects();
		HRESULT RestoreDeviceObjects();

		
		HRESULT Render(DWORD DeltaT,CD3DFont*font); // milli secondes

		MENU*Parent;

		MENU(ItemStruct _MenuTitle,LPDIRECT3DDEVICE8 _dev8,CDirect3DDeviceUtility *pd3dDeviceUtility,MENU*_Parent=NULL);

		// detruit tous ses fils
		~MENU();

		void MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam );
} MENU;


void MENU::AddTextItem(ItemStruct item)
{
	if (this->NumItems==0)
	{
		this->NumItems++;
		this->items=(ItemStruct*)malloc(sizeof(ItemStruct));
		this->items[0]=item;
		this->items[0].submenu=NULL;
	}
	else
	{
		this->NumItems++;
		this->items=(ItemStruct*)realloc(this->items,this->NumItems*sizeof(ItemStruct));
		this->items[this->NumItems-1]=item;	
		this->items[this->NumItems-1].submenu=NULL;
	}
}

// cree le menu
MENU* MENU::AddMenuItem(ItemStruct item,ItemStruct SubMenuTitle)
{
	if (this->NumItems==0)
	{
		this->items=(ItemStruct*)malloc(sizeof(ItemStruct));
		this->items[0]=item;
		this->NumItems++;
		this->items[0].submenu=new MENU(SubMenuTitle,dev8,m_pd3dDeviceUtility,this);
		return this->items[0].submenu;
	}
	else
	{
		this->NumItems++;
		this->items=(ItemStruct*)realloc(this->items,this->NumItems*sizeof(ItemStruct));
		this->items[this->NumItems-1]=item;	
		this->items[this->NumItems-1].submenu=new MENU(SubMenuTitle,dev8,m_pd3dDeviceUtility,this);
		return this->items[this->NumItems-1].submenu;
	}
	return NULL;
}

void MENU::AddBitmap(Bitmap _bitmap)
{
	if (this->NumBitmaps==0)
	{
		this->NumBitmaps++;
		this->bitmaps=(Bitmap*)malloc(sizeof(Bitmap));
		this->bitmaps[0]=_bitmap;		
		this->textures=(LPDIRECT3DTEXTURE8*)malloc(sizeof(LPDIRECT3DTEXTURE8));
		HRESULT hr;
		hr=D3DXCreateTextureFromFileA(
			this->dev8,
			_bitmap.filename,
			&this->textures[0]
			);

		if (FAILED(hr))
			this->textures[0]=NULL;

	}
	else
	{
		this->NumBitmaps++;
		this->bitmaps=(Bitmap*)realloc(this->bitmaps,this->NumBitmaps*sizeof(Bitmap));
		this->bitmaps[this->NumBitmaps-1]=_bitmap;
		this->textures=(LPDIRECT3DTEXTURE8*)malloc(this->NumBitmaps*sizeof(LPDIRECT3DTEXTURE8));
		HRESULT hr;
		hr=D3DXCreateTextureFromFileA(
			this->dev8,
			_bitmap.filename,
			&this->textures[this->NumBitmaps-1]
			);

		if (FAILED(hr))
			this->textures[this->NumBitmaps-1]=NULL;
	}
}

HRESULT MENU::InvalidateDeviceObjects()
{
	for (DWORD i=0;i<this->NumBitmaps;i++)
	{
		if (this->textures[i]!=NULL)
			this->textures[i]->Release();

		this->textures[i]=NULL;
	}

	return S_OK;
}

HRESULT MENU::RestoreDeviceObjects()
{
	for (DWORD i=0;i<this->NumBitmaps;i++)
	{
		HRESULT hr;
		hr=D3DXCreateTextureFromFileA(
			this->dev8,
			this->bitmaps[i].filename,
			&this->textures[i]
			);

		if (FAILED(hr))
			this->textures[i]=NULL;
	}

	
	return S_OK;
}


HRESULT MENU::Render(DWORD DeltaT,CD3DFont*font) // milli secondes
{
	if (this->active)
	{
		D3DXMATRIX proj,view,world;		
		D3DVIEWPORT8 viewport;
		
		D3DXMatrixIdentity(&world);
		
		D3DXVECTOR3 lookat=D3DVEC(0,0,0);
		D3DXVECTOR3 eye=D3DVEC(0,0,-5);
		D3DXVECTOR3 up1=D3DXVECTOR3( 0, 1, 0 );


		D3DXMatrixLookAtLH( &view, &eye,
		    					  &lookat,
								  &up1);

		this->m_pd3dDeviceUtility->SetTransform( D3DTS_WORLD, &world );

		this->m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW, &view );

		this->m_pd3dDeviceUtility->GetTransform(D3DTS_PROJECTION, &proj);

		this->m_pd3dDeviceUtility->GetViewport(&viewport);

		for (DWORD i=0;i<this->NumItems;i++)
		{
			int l=Text_Length(this->items[i].ItemText);
			float x0=this->items[i].Center.x,y0=this->items[i].Center.y;

			this->items[i].Center.z=0;

			for (int j=0;j<l;j++)
			{
				D3DXVECTOR3 CC,CC0;
				CC0=this->items[i].Center;
				CC0.x+=(-l*this->items[i].size/2.0f+j*this->items[i].size);

				this->m_pd3dDeviceUtility->SetTransform( D3DTS_WORLD, &world );
				
				D3DXVec3Unproject(&CC0,&CC,&viewport,&proj,&view,&world);
				D3DXMATRIX mat1,mat2,mat3;
				D3DXMatrixTranslation(&mat1,-CC.x,-CC.y,-CC.z);
				if(this->items[i].GetCharMatrix!=NULL)
					mat2=this->items[i].GetCharMatrix(DeltaT,j,this->items[i].size,CC);
				else
					D3DXMatrixIdentity(&mat2);


				D3DXMatrixMultiply(&mat3,&mat2,&mat1);

				for (DWORD k=0;k<3;k++)
					for (DWORD MM=0;MM<3;MM++)
						mat3(k,MM)/=0.1f;

				this->m_pd3dDeviceUtility->SetTransform( D3DTS_WORLD, &mat3 );

				char s[2];
				s[1]=0;
				s[0]=this->items[i].ItemText[j];
				font->Render3DText(s,0);
								
			}
			
		}	

	}
	else
	{
		for (DWORD i=0;i<this->NumItems;i++)
		{
			if (this->items[i].submenu!=NULL)
			{
				this->items[i].submenu->Render(DeltaT,font);
			}
		}
	}

	return S_OK;
}




MENU::MENU(ItemStruct _MenuTitle,LPDIRECT3DDEVICE8 _dev8,MENU*_Parent)
{
	this->bitmaps=NULL;
	this->items=NULL;
	this->NumBitmaps=0;
	this->NumItems=0;
	this->Parent=_Parent;
	this->MenuTitle=_MenuTitle;
	this->dev8=_dev8;
	this->active=false;
}

MENU::~MENU()
{
	for(DWORD i=0;i<this->NumItems;i++)
	{
		if (this->items[i].submenu!=NULL)
			this->items[i].submenu->~MENU();
	}

	free(this->items);this->items=NULL;this->NumItems=0;

	for(i=0;i<this->NumBitmaps;i++)
	{
		if (this->textures[i]!=NULL)
			this->textures[i]->Release();
	}

	free(this->textures);
	free(this->bitmaps);this->bitmaps=NULL;this->textures=NULL;
	this->NumBitmaps=0;
}


int SIGN_EXTEND(WORD w)
{
	if ((w & 0x8000)==0x8000)
	{
		int k=w&0x7fff;
		return -k;
	}
	else
	{
		int k=w&0x7fff;
		return k;
	}
}

void MENU::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{	
	if (this->active)
	{
		switch( uMsg )
		{
		case WM_MOUSEMOVE:
			{
				int x=SIGN_EXTEND(LOWORD(lParam));
				int y=SIGN_EXTEND(HIWORD(lParam));
				DWORD i;
				for(i=0;i<this->NumItems;i++)
				{
					float x0=x-this->items[i].Center.x;
					float y0=y-this->items[i].Center.y;
					if (
						(x0>=-(this->items[i].size*Text_Length(this->items[i].ItemText)/2.0))
						&&
						(y0>=-(this->items[i].size/2.0))
						&&
						(x0<=this->items[i].size*Text_Length(this->items[i].ItemText)/2.0)
						&&
						(y0<=this->items[i].size/2.0)
					   )
					{
						this->index_focused=i;
					}
				}
			}
			break;
		
		}
	}
	else
	{
		for (DWORD i=0;i<this->NumItems;i++)
		{
			if (this->items[i].submenu!=NULL)
			{
				this->items[i].submenu->MsgProc(hWnd,uMsg,wParam,lParam);
			}
		}
	}

}




#endif
