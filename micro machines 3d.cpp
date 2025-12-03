#define STRICT
#define _WIN32_WINDOWS 0x0410
#include <windows.h>
#include <commdlg.h>
#include <math.h>
#include <tchar.h>
#include <stdio.h>
#include <D3DX8.h>
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "resource.h"
#include <dinput.h>

#include "MeshUtils.h"
#include "gamemenu.h"
#include "KeyBoard.h"
#include "dmutil.h"
#include	"waypoint.h"
#include	"computer.h"



static bool EscPressed = false;
typedef enum
{
	APP_STATE_PLAYING,
	APP_STATE_MENU,
	APP_STATE_QUITMENU,
	APP_STATE_QUITGAMEMENU
} APP_STATE;

typedef enum
{
	APP_SUBSTATE_PLAYING,
	APP_SUBSTATE_INTROANIM,
	APP_SUBSTATE_OUTROANIM,
	APP_SUBSTATE_WINANIM
} APP_SUBSTATE;



//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	bool		  GameStarted;
    CD3DFont*     m_pFont;
	CD3DFont*     m_pRankingFont;
    CD3DFont*     m_pStatsFont;
    TCHAR         m_strFont[100];
    DWORD         m_dwFontSize;
    BYTE    diks[256];
	LPDIRECTINPUT8       g_pDI;
	LPDIRECTINPUTDEVICE8 g_pKeyboard;
	HBITMAP credits_hbmp;
	bool MetallicaPlaying;
	bool MetropolisPlaying,bounce,beep;

    D3DXMATRIX m_matBillboard;

	PMeshManager MONDE;
	PMeshStruct  Sphere;
	PMeshStruct  bgPLANE;

	// pour l'arrière plan
	PMeshStruct   HyperPlane; 

	// Menu objects
	CMenuItem*           g_pMainMenu;    // Menu class for in-game menus
	CMenuItem*           g_pQuitMenu;
	CMenuItem*           g_pQuitGameMenu;
	CMenuItem*           g_pCurrentMenu;
	CMenuItem*			 g_pStageMenu;
	CMenuItem*           g_pGlobalSaveMenu;
	CMenuItem*           g_pIntelligenceMenu;

	// DirectMusic objects
	CMusicManager*       g_pMusicManager;  // Class to manage DMusic objects
	CMusicSegment*       g_pSphereExplodeSound;
	CMusicSegment*       g_pExplodeSound;
	CMusicSegment*       g_pBounceSound;
	CMusicSegment*       g_pLOLSound;
	CMusicSegment*       g_pMetallicaSound;
	CMusicSegment*       g_pMetropolisSound;
	CMusicSegment*       g_pBeepSound;
	
	struct
	{
		BYTE UP,DOWN,LEFT,RIGHT;
	} Touches[3];

	APP_STATE    app_state;
	APP_SUBSTATE app_substate;
	int ss1; // sub sub state
	         // utilisé pour le APP_SUBSTATE_WINANIM
	// pour mémoriser la voiture gagnante et perdante
	DWORD winnerIndex;
	DWORD looserIndex;


	DWORD	Greatest_lap;

	// also used for animation
	struct
	{
		DWORD LastT;
		D3DVECTOR initial,to;
	} AnimCam;


	DWORD nPlayers;
	DWORD nCPU;
	DWORD LastT;
	DWORD	DeltaT;
	int Intelligence;

	PCWAYPOINT	WayPoints[4];
	PCCOMPUTER	computers[3];

	D3DXVECTOR3 lookat;
	D3DXVECTOR3 eye;
	D3DXVECTOR3 up1;
	float FOV;
	D3DXMATRIX matView;

protected:
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove();
    HRESULT FinalCleanup();
	HRESULT OnCreateDevice( HWND hDlg );
	HRESULT ReadImmediateData();
	VOID    FreeDirectInput();
	VOID    ConstructMenus();
	VOID    UpdateMenus();
	// (pos,rx,ry) % viewport 
	void DrawBillBoard(D3DVECTOR pos, float rx,float ry,
					   LPDIRECT3DTEXTURE8 tex,D3DMATERIAL8* mat);
	HRESULT CreateSoundObjects( HWND hWnd );
	VOID DestroySoundObjects();
	inline VOID PlaySound( CMusicSegment* pSound );
	inline VOID StopSound( CMusicSegment* pSound );
	void ArtificialIntelligence(DWORD DeltaT);

public:
    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
	HRESULT CreateHyperPlanes(char*FileName);
};


CMyD3DApplication d3dApp;



//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{    
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_strWindowTitle    = _T("Micro Machines 3D");
    m_bUseDepthBuffer   = true;

    // Create fonts
    lstrcpy( m_strFont, _T("Time New Roman") );
    m_dwFontSize = 18;
    m_pFont      = new CD3DFont( _T("Impact"), 48, 0L );
    m_pRankingFont= new CD3DFont( _T("Impact"), 36, 0L );
    m_pStatsFont = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );

	MONDE=NULL;
	Sphere=NULL;
	bgPLANE=NULL;

	g_pDI = NULL;
	g_pKeyboard = NULL;
	g_pQuitMenu       = NULL;
	g_pMainMenu       = NULL;
	g_pCurrentMenu       = NULL;
	app_state=APP_STATE_MENU;
	LastT=0;
	g_pMusicManager        = NULL;  // Class to manage DMusic objects
	g_pSphereExplodeSound  = NULL;
	g_pBounceSound         = NULL;
	g_pExplodeSound         = NULL;
	g_pMetropolisSound			=NULL;
	g_pLOLSound         = NULL;
	HyperPlane             = NULL;
	g_pMetallicaSound=NULL;
	g_pBeepSound=0;
	MetallicaPlaying=false;
	MetropolisPlaying=false;
	GameStarted=false;
	bounce=false;
	beep=false;
}

void CMyD3DApplication::ArtificialIntelligence(DWORD DeltaT)
{
	if(this->nCPU==0) return;
	for(DWORD i=this->nPlayers;i<this->nPlayers+this->nCPU;i++)
	{
		if(MONDE->Cars[i]->State.car_state==CAR_STATE_ALIFE)
		{
			computers[i-this->nPlayers]->Advance(DeltaT,MONDE,MONDE->Cars[i],this->nPlayers+this->nCPU);
		}
	}
}

HRESULT CMyD3DApplication::CreateHyperPlanes(char*FileName)
{

	if (this->HyperPlane)
		delete HyperPlane;
	HyperPlane=NULL;

	this->HyperPlane=new MeshStruct();
	D3DXMatrixScaling(&HyperPlane->matrice,0.0014f,0.002f,0.0014f);
	HyperPlane->matrice(3,2)=0.5f;
	if (!HyperPlane->LoadFormFile( FileName,m_pd3dDevice))
		return E_FAIL;
	else 
		return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateSoundObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateSoundObjects( HWND hWnd )
{
    // Create the music manager class, used to create the sounds
    g_pMusicManager = new CMusicManager();
    if( FAILED( g_pMusicManager->Initialize( hWnd ) ) )
        return E_FAIL;
	 
    // Instruct the music manager where to find the files
	g_pMusicManager->CreateSegmentFromFile(&g_pSphereExplodeSound,"waves/navbar.wav");
	g_pMusicManager->CreateSegmentFromFile(&g_pBounceSound,"waves/bounce.wav");
	g_pMusicManager->CreateSegmentFromFile(&g_pExplodeSound,"waves/explode.wav");
	g_pMusicManager->CreateSegmentFromFile(&g_pLOLSound,"waves/ChatLOL.wav");
	g_pMusicManager->CreateSegmentFromFile(&g_pBeepSound,"waves/ChatBEEP.wav");
	g_pMusicManager->CreateSegmentFromFile(&g_pMetallicaSound,"waves/Mercyfull fate.mid",TRUE,TRUE);
	g_pMusicManager->CreateSegmentFromFile(&g_pMetropolisSound,"waves/metropolis part 1.mid",TRUE,TRUE);
	g_pMetropolisSound->SetRepeats(0xFFFFFFFF);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DestroySoundObjects()
// Desc:
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::DestroySoundObjects()
{
	SAFE_DELETE( g_pLOLSound );
	SAFE_DELETE( g_pBounceSound );
	SAFE_DELETE( g_pExplodeSound );
    SAFE_DELETE( g_pSphereExplodeSound );
	SAFE_DELETE( g_pMetallicaSound );
	SAFE_DELETE( g_pBeepSound );
	SAFE_DELETE( g_pMetropolisSound );
    SAFE_DELETE( g_pMusicManager );
}

// Convenient macros for playing sounds
inline VOID CMyD3DApplication::PlaySound( CMusicSegment* pSound )
{
        if( pSound ) pSound->Play( DMUS_SEGF_SECONDARY );
}

inline VOID CMyD3DApplication::StopSound( CMusicSegment* pSound )
{
        if( pSound ) pSound->Stop();
}

#define MENU_MAIN           1
#define MENU_SOUND          2
#define MENU_2CARS          3
#define MENU_3CARS          4
#define MENU_4CARS          5
#define MENU_ONEPLAYER      6
#define MENU_TWOPLAYERS     7
#define MENU_THREEPLAYERS   8
#define MENU_PLAY           9
#define MENU_CREDITS       10
#define MENU_BACK          11
#define MENU_SOUNDON       12
#define MENU_SOUNDOFF      13
#define MENU_QUIT          14
#define MENU_KEYBOARD      15
#define MENU_P1C           16
#define MENU_P2C           17 
#define MENU_P3C           18
#define MENU_OPTION		   19
#define MENU_UP            20
#define MENU_DOWN          21 
#define MENU_LEFT          22
#define MENU_RIGHT         23
#define MENU_EXITPROGRAM   24
#define MENU_BACKMAIN      25
#define MENU_STAGE		   26
#define MENU_S1			   27
#define MENU_S2            28
#define MENU_S3            29
#define MENU_QUITGAME      30
#define MENU_BACKTOGAME    31
#define MENU_INTELLIGENCE  32
#define MENU_INT_STUPID    33
#define MENU_INT_EASY      34
#define MENU_INT_MEDIUM    35
#define MENU_INT_GOOD      36




VOID CMyD3DApplication::ConstructMenus()
{
	
    g_pIntelligenceMenu = new CMenuItem( _T("CPU Intelligence"), MENU_INTELLIGENCE );
    g_pIntelligenceMenu->Add( new CMenuItem( _T("Stupid"),       MENU_INT_STUPID   ) );
    g_pIntelligenceMenu->Add( new CMenuItem( _T("Easy"),         MENU_INT_EASY     ) );
    g_pIntelligenceMenu->Add( new CMenuItem( _T("Medium"),       MENU_INT_MEDIUM   ) );
    g_pIntelligenceMenu->Add( new CMenuItem( _T("Good"),         MENU_INT_GOOD     ) );
    g_pIntelligenceMenu->Add( new CMenuItem( _T("Back"),         MENU_BACK         ) );


    g_pStageMenu = new CMenuItem( _T("Map Select"), MENU_STAGE  );
    g_pStageMenu->Add( new CMenuItem( _T("Map Cuisine"),    MENU_S1   ) );
    //g_pStageMenu->Add( new CMenuItem( _T("Map NULL"),    MENU_S2   ) );
    g_pStageMenu->Add( new CMenuItem( _T("Map Nature"),    MENU_S3   ) );
    g_pStageMenu->Add( new CMenuItem( _T("Back"),     MENU_BACK ) );

    CMenuItem* pOnePlayerSubMenu = new CMenuItem( _T("One Player"), MENU_ONEPLAYER );
    pOnePlayerSubMenu->Add( new CMenuItem( _T("+1 CPU"),  MENU_2CARS ) );
    pOnePlayerSubMenu->Add( new CMenuItem( _T("+2 CPU"),  MENU_3CARS ) );
    pOnePlayerSubMenu->Add( new CMenuItem( _T("+3 CPU"),  MENU_4CARS ) );
    pOnePlayerSubMenu->Add( new CMenuItem( _T("Back"),         MENU_BACK  ) );

	CMenuItem* pTwoPlayerSubMenu = new CMenuItem( _T("Two Players"), MENU_TWOPLAYERS );
    pTwoPlayerSubMenu->Add( new CMenuItem( _T("No CPU"),  MENU_2CARS ) );
    pTwoPlayerSubMenu->Add( new CMenuItem( _T("+1 CPU"),  MENU_3CARS ) );
    pTwoPlayerSubMenu->Add( new CMenuItem( _T("+2 CPU"),  MENU_4CARS ) );
    pTwoPlayerSubMenu->Add( new CMenuItem( _T("Back"),         MENU_BACK  ) );

	CMenuItem* pThreePlayerSubMenu = new CMenuItem( _T("Three Players"), MENU_THREEPLAYERS );
    pThreePlayerSubMenu->Add( new CMenuItem( _T("No CPU"),  MENU_3CARS ) );
    pThreePlayerSubMenu->Add( new CMenuItem( _T("+1 CPU"),  MENU_4CARS ) );
    pThreePlayerSubMenu->Add( new CMenuItem( _T("Back"),         MENU_BACK  ) );


    // Build video sub menu
    CMenuItem* pPlaySubMenu = new CMenuItem( _T("Play"), MENU_PLAY );
    pPlaySubMenu->Add( pOnePlayerSubMenu );
    pPlaySubMenu->Add( pTwoPlayerSubMenu );
    pPlaySubMenu->Add( pThreePlayerSubMenu );
    pPlaySubMenu->Add( new CMenuItem( _T("Back"),     MENU_BACK  ) );


    // Build sound menu
    CMenuItem* pSoundSubMenu = new CMenuItem( _T("Sound"), MENU_SOUND );
    pSoundSubMenu->Add( new CMenuItem( _T("Sound On"),  MENU_SOUNDON ) );
    pSoundSubMenu->Add( new CMenuItem( _T("Sound Off"), MENU_SOUNDOFF ) );
    pSoundSubMenu->Add( new CMenuItem( _T("Back"),      MENU_BACK ) );

	
	// Build Keyboard menu
	CMenuItem *g_pKeyboardMenu = new CMenuItem( _T("Keyboard Menu"),  MENU_KEYBOARD );
    g_pKeyboardMenu->Add( new CMenuItem( _T("Player One Controls"),      MENU_P1C ) );
    g_pKeyboardMenu->Add( new CMenuItem( _T("Player Two Controls"),      MENU_P2C ) );
    g_pKeyboardMenu->Add( new CMenuItem( _T("Player Three Controls"),    MENU_P3C ) );
    g_pKeyboardMenu->Add( new CMenuItem( _T("Back"), MENU_BACK ) );

	//Build options menu
    CMenuItem *g_pOptionMenu = new CMenuItem( _T("Options Menu"),  MENU_OPTION );
    g_pOptionMenu->Add( g_pKeyboardMenu );
    g_pOptionMenu->Add( new CMenuItem( _T("Back"), MENU_BACK ) );

    // Build main menu
    g_pMainMenu = new CMenuItem( _T("Main Menu"),  MENU_MAIN );
    g_pMainMenu->Add( pPlaySubMenu );
	g_pMainMenu->Add( g_pOptionMenu );
    g_pMainMenu->Add( pSoundSubMenu );
    g_pMainMenu->Add( new CMenuItem( _T("Credits"),   MENU_CREDITS     ) );
    g_pMainMenu->Add( new CMenuItem( _T("Exit Game"), MENU_EXITPROGRAM ) );

    // Build "quit game?" menu
    g_pQuitGameMenu = new CMenuItem( _T("Quit Game ?"), MENU_MAIN );
    g_pQuitGameMenu->Add( new CMenuItem( _T("Yes"),     MENU_QUITGAME   ) );
    g_pQuitGameMenu->Add( new CMenuItem( _T("No"),      MENU_BACKTOGAME ) );

	// Build "quit game?" menu
    g_pQuitMenu = new CMenuItem( _T("Back to Windows ?"),  MENU_MAIN );
    g_pQuitMenu->Add( new CMenuItem( _T("Yes"),     MENU_QUIT     ) );
    g_pQuitMenu->Add( new CMenuItem( _T("No"),      MENU_BACKMAIN ) );

    return;
}



	// utilisée pour gérer l'état des menus
	static bool EnterPressed = false;	
	static bool UpPressed = false;	
	static bool DownPressed = false;	
	static DWORD player;
	static bool CurrentIsKeyboardEntry=false;
	static bool ReadingEntry=false;
	static bool AllKeysReleased=false;
	static CMenuItem* OldMenu;



//-----------------------------------------------------------------------------
// Name: UpdateMenus()
// Desc:
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::UpdateMenus()
{
    if( g_pCurrentMenu == NULL )
        return;

    // Keep track of current selected menu, to check later for changes
    DWORD dwCurrentSelectedMenu = g_pCurrentMenu->dwSelectedMenu;

    

	if ((diks[0x1c]&0x80) !=0x80) EnterPressed=false;
	if ((diks[0x1]&0x80) !=0x80)  EscPressed=false;

	if ((diks[0xC8]&0x80) !=0x80)  UpPressed=false;
	if ((diks[0xD0]&0x80) !=0x80)  DownPressed=false;

	if(!CurrentIsKeyboardEntry)
	{
		if(diks[0x1]&0x80) 
			if(!EscPressed)
			{
				PlaySound( g_pSphereExplodeSound );
				EscPressed=true;
				g_pCurrentMenu = g_pCurrentMenu->pParent;

				if(g_pCurrentMenu==NULL)
				{
					for(DWORD k=0;(k<3)&&(k<this->nPlayers+this->nCPU)
						          &&(k<this->MONDE->NumCars);k++)
					{
						this->MONDE->Cars[k]->SetClavier(
							this->Touches[k].UP,
							this->Touches[k].DOWN,
							this->Touches[k].LEFT,
							this->Touches[k].RIGHT
							);
					}

					if (this->app_state==APP_STATE_QUITGAMEMENU)
					{
						g_pCurrentMenu=g_pQuitMenu;
						this->app_state=APP_STATE_PLAYING;
					}
					else
					{
						g_pCurrentMenu=g_pQuitMenu;
						this->app_state=APP_STATE_QUITMENU;
					}
					return;
				}
			}

		// Check for menu up/down input
		if( ((diks[0xC8]&0x80)==0x80) && !UpPressed )
		{
			UpPressed=true;
			if( g_pCurrentMenu->dwSelectedMenu > 0 )
				g_pCurrentMenu->dwSelectedMenu--;
		}

		if( ((diks[0xD0]&0x80)==0x80) && !DownPressed )
		{
			DownPressed=true;
			if( (g_pCurrentMenu->dwSelectedMenu+1) < g_pCurrentMenu->dwNumChildren )
				g_pCurrentMenu->dwSelectedMenu++;
		}

		// The the current menu changed, play a sound
		if( dwCurrentSelectedMenu != g_pCurrentMenu->dwSelectedMenu )
			PlaySound( g_pSphereExplodeSound );
		if((diks[0x1c]&0x80) && (!EnterPressed))
		{
			PlaySound( g_pSphereExplodeSound );
			EnterPressed=true;

			DWORD dwID = g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->dwID;

			switch( dwID )
			{
				case MENU_BACK:
					g_pCurrentMenu = g_pCurrentMenu->pParent;
					
					if(g_pCurrentMenu==NULL)
					{
						for(DWORD k=0;(k<3)&&(k<this->nPlayers+this->nCPU);k++)
						{
							this->MONDE->Cars[k]->SetClavier(
								this->Touches[k].UP,
								this->Touches[k].DOWN,
								this->Touches[k].LEFT,
								this->Touches[k].RIGHT
								);
						}
						
						g_pCurrentMenu=g_pQuitMenu;
						this->app_state=APP_STATE_QUITMENU;						
					}

					break;

				case MENU_CREDITS:
						BITMAP bmp;
						RECT rc;
						GetClientRect( this->m_hWnd, &rc );

						// Display the splash bitmap in the window
						HDC hDCWindow;
						hDCWindow= GetDC( this->m_hWnd );
						HDC hDCImage;
						hDCImage= CreateCompatibleDC( NULL );
						SelectObject( hDCImage, credits_hbmp );
						GetObject( credits_hbmp, sizeof(bmp), &bmp );
						StretchBlt( hDCWindow, 0, 0, rc.right, rc.bottom,
									hDCImage, 0, 0,
									bmp.bmWidth, bmp.bmHeight, SRCCOPY );
						DeleteDC( hDCImage );
						ReleaseDC( m_hWnd, hDCWindow );

						while(diks[0x1c]&0x80)
							ReadImmediateData();
						bool test;
						test=true;
						while(test)
						{
							ReadImmediateData();
							for(DWORD i=0;i<255;i++)
								if(diks[i]&0x80) test=false;
						}
						break;

				case MENU_EXITPROGRAM:
					g_pCurrentMenu=g_pQuitMenu;
					this->app_state=APP_STATE_QUITMENU;
					break;

				case MENU_BACKMAIN:
					g_pCurrentMenu=g_pMainMenu;
					this->app_state=APP_STATE_MENU;
					break;

				case MENU_2CARS:
					this->nCPU=2-this->nPlayers;
					if(nCPU>0)
					{
						g_pIntelligenceMenu->pParent=g_pCurrentMenu;
						g_pCurrentMenu=g_pIntelligenceMenu;
					}
					else
					{
						g_pStageMenu->pParent=g_pCurrentMenu;
						g_pCurrentMenu=g_pStageMenu;					
					}
					break;
				
				case MENU_3CARS:
					this->nCPU=3-this->nPlayers;
					if(nCPU>0)
					{
						g_pIntelligenceMenu->pParent=g_pCurrentMenu;
						g_pCurrentMenu=g_pIntelligenceMenu;
					}
					else
					{
						g_pStageMenu->pParent=g_pCurrentMenu;
						g_pCurrentMenu=g_pStageMenu;
					}
					break;
				
				case MENU_4CARS:
					this->nCPU=4-this->nPlayers;
					if(nCPU>0)
					{
						g_pIntelligenceMenu->pParent=g_pCurrentMenu;
						g_pCurrentMenu=g_pIntelligenceMenu;
					}
					else
					{
						g_pStageMenu->pParent=g_pCurrentMenu;
						g_pCurrentMenu=g_pStageMenu;					
					}
					break;
				case MENU_INT_STUPID:
					Intelligence=0;
					g_pStageMenu->pParent=g_pCurrentMenu;
					g_pCurrentMenu=g_pStageMenu;					
					break;
				case MENU_INT_EASY:
					Intelligence=1;
					g_pStageMenu->pParent=g_pCurrentMenu;
					g_pCurrentMenu=g_pStageMenu;					
					break;
				case MENU_INT_MEDIUM:
					Intelligence=2;
					g_pStageMenu->pParent=g_pCurrentMenu;
					g_pCurrentMenu=g_pStageMenu;					
					break;
				case MENU_INT_GOOD:
					Intelligence=3;
					g_pStageMenu->pParent=g_pCurrentMenu;
					g_pCurrentMenu=g_pStageMenu;					
					break;
					
				case MENU_S1:
				case MENU_S2:
				case MENU_S3:	
					GameStarted=false;
					this->app_substate=APP_SUBSTATE_INTROANIM;
					this->app_state=APP_STATE_PLAYING;
					this->MONDE->UnloadAll();
					
					MONDE->DeleteAll();		
					if(dwID==MENU_S1)
						MONDE->LoadFromSTGFile("stage cuisine/cuisine.stg",m_pd3dDevice,m_pd3dDeviceUtility);
					else if(dwID==MENU_S2)
						MONDE->LoadFromSTGFile("stage metal/metal.stg",m_pd3dDevice,m_pd3dDeviceUtility);
					else if(dwID==MENU_S3)
						MONDE->LoadFromSTGFile("stage Nature/Nature.stg",m_pd3dDevice,m_pd3dDeviceUtility);

					DWORD i;
					for (i=0;i<this->nPlayers+this->nCPU;i++)
					{
						this->MONDE->Cars[i]->SetClavier(this->Touches[i].UP,
														 this->Touches[i].DOWN,
														 this->Touches[i].LEFT,
														 this->Touches[i].RIGHT);
					}
					MONDE->Scale(0.15f,m_pd3dDevice);
					MONDE->ComputeAllFaceInfo();
					MONDE->Cars[0]->Mesh->ComputeFaceInfo();

					if (MONDE->NumCars>0)
					{
						MONDE->ComputeAllFaceInfo2(3*MONDE->Cars[0]->Mesh->TransformedRadius);
						MONDE->ComputeBaliseInfo(PI/4.0f,6*MONDE->Cars[0]->Mesh->TransformedRadius);
					}
					else
					{
						MONDE->ComputeBaliseInfo(PI/4.0f,1.0);
					}					
					
					for(i=0;i<this->nPlayers+this->nCPU;i++)
					{
						this->MONDE->Cars[i]->vie=4;
						this->MONDE->Cars[i]->tour=0;
						this->MONDE->Cars[i]->balise=0;
						this->MONDE->Cars[i]->State.car_state=CAR_STATE_ALIFE;
						this->MONDE->Cars[i]->State.sub_state=0;
						this->MONDE->Cars[i]->State.s1=0;
						this->MONDE->Cars[i]->State.s2=0;
					}


					this->computers[0]->Init(this->MONDE,this->Intelligence);
					this->computers[1]->Init(this->MONDE,this->Intelligence);
					this->computers[2]->Init(this->MONDE,this->Intelligence);


					break;


				case MENU_BACKTOGAME:
					this->app_state=APP_STATE_PLAYING;
					this->g_pCurrentMenu=this->g_pGlobalSaveMenu;
					break;

				case MENU_QUITGAME:
					this->app_state=APP_STATE_MENU;
					this->g_pCurrentMenu=this->g_pMainMenu;
					//this->MONDE->DeleteAll();
					this->nCPU=0;
					this->nPlayers=0;
					break;

				case MENU_ONEPLAYER:
					this->nPlayers=1;
					g_pCurrentMenu = g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu];
					break;
				case MENU_TWOPLAYERS:
					this->nPlayers=2;
					g_pCurrentMenu = g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu];
					break;
				case MENU_THREEPLAYERS:
					this->nPlayers=3;
					g_pCurrentMenu = g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu];
					break;

				case MENU_OPTION:
				case MENU_KEYBOARD:
				case MENU_SOUND:
				case MENU_PLAY:
					g_pCurrentMenu = g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu];
					break;

				case MENU_P1C:
					player=dwID;
						// Build video sub menu
					CMenuItem* pMenu;
					pMenu=new CMenuItem( _T("Keyboard Entry 1"), MENU_PLAY );					
					pMenu->Add( new CMenuItem( Text_Concat("UP :   ",GetKeyName(this->Touches[0].UP)),  MENU_UP ) );
					pMenu->Add( new CMenuItem( Text_Concat("DOWN :   ",GetKeyName(this->Touches[0].DOWN)),  MENU_DOWN ) );
					pMenu->Add( new CMenuItem( Text_Concat("LEFT :   ",GetKeyName(this->Touches[0].LEFT)),  MENU_LEFT ) );
					pMenu->Add( new CMenuItem( Text_Concat("RIGHT :   ",GetKeyName(this->Touches[0].RIGHT)),  MENU_RIGHT ) );
					pMenu->Add( new CMenuItem( _T("Back"),     MENU_BACK ) );
					OldMenu=g_pCurrentMenu;
					g_pCurrentMenu=pMenu;
					CurrentIsKeyboardEntry=true;
					ReadingEntry=false;
					break;

				case MENU_P2C:
					player=dwID;
						// Build video sub menu
					pMenu=new CMenuItem( _T("Keyboard Entry 2"), MENU_PLAY );					
					pMenu->Add( new CMenuItem( Text_Concat("UP :   ",GetKeyName(this->Touches[1].UP)),  MENU_UP ) );
					pMenu->Add( new CMenuItem( Text_Concat("DOWN :   ",GetKeyName(this->Touches[1].DOWN)),  MENU_DOWN ) );
					pMenu->Add( new CMenuItem( Text_Concat("LEFT :   ",GetKeyName(this->Touches[1].LEFT)),  MENU_LEFT ) );
					pMenu->Add( new CMenuItem( Text_Concat("RIGHT :   ",GetKeyName(this->Touches[1].RIGHT)),  MENU_RIGHT ) );
					pMenu->Add( new CMenuItem( _T("Back"),     MENU_BACK ) );
					OldMenu=g_pCurrentMenu;
					g_pCurrentMenu=pMenu;
					CurrentIsKeyboardEntry=true;
					ReadingEntry=false;
					break;

				case MENU_P3C:
					player=dwID;
						// Build video sub menu
					pMenu=new CMenuItem( _T("Keyboard Entry 3"), MENU_PLAY );					
					pMenu->Add( new CMenuItem( Text_Concat("UP :   ",GetKeyName(this->Touches[2].UP)),  MENU_UP ) );
					pMenu->Add( new CMenuItem( Text_Concat("DOWN :   ",GetKeyName(this->Touches[2].DOWN)),  MENU_DOWN ) );
					pMenu->Add( new CMenuItem( Text_Concat("LEFT :   ",GetKeyName(this->Touches[2].LEFT)),  MENU_LEFT ) );
					pMenu->Add( new CMenuItem( Text_Concat("RIGHT :   ",GetKeyName(this->Touches[2].RIGHT)),  MENU_RIGHT ) );
					pMenu->Add( new CMenuItem( _T("Back"),     MENU_BACK ) );
					OldMenu=g_pCurrentMenu;
					g_pCurrentMenu=pMenu;
					CurrentIsKeyboardEntry=true;
					ReadingEntry=false;
					break;

				 case MENU_SOUNDON:
					if( g_pMusicManager == NULL )
					{
						CreateSoundObjects( m_hWnd );
     					if(!MetallicaPlaying)
						{
							MetallicaPlaying=true;
							PlaySound(g_pMetallicaSound);
						}
					}
					g_pCurrentMenu= g_pCurrentMenu->pParent;
					break;

				case MENU_SOUNDOFF:
					if( g_pMusicManager )
					{
						DestroySoundObjects();
						MetallicaPlaying=false;
						MetropolisPlaying=false;
					}
					 g_pCurrentMenu= g_pCurrentMenu->pParent;
					break;
				

				case MENU_QUIT:
					PostMessage( this->m_hWnd, WM_CLOSE, 0, 0 );
					g_pCurrentMenu = NULL;
					break;
			}
		}
	}
	else
	{
		if (!ReadingEntry)
		{
			if((diks[0x1]&0x80) && (!EscPressed))
			{
				PlaySound( g_pSphereExplodeSound );
				EscPressed=true;
				g_pCurrentMenu->~CMenuItem();
				g_pCurrentMenu = OldMenu;
				CurrentIsKeyboardEntry=false;
				return;
			}
	
			
			// Check for menu up/down input
			if( ((diks[0xC8]&0x80)==0x80) && !UpPressed )
			{
				PlaySound( g_pSphereExplodeSound );
				UpPressed=true;
				if( g_pCurrentMenu->dwSelectedMenu > 0 )
					g_pCurrentMenu->dwSelectedMenu--;
			}

			if( ((diks[0xD0]&0x80)==0x80) && !DownPressed )
			{
				PlaySound( g_pSphereExplodeSound );
				DownPressed=true;
				if( (g_pCurrentMenu->dwSelectedMenu+1) < g_pCurrentMenu->dwNumChildren )
					g_pCurrentMenu->dwSelectedMenu++;
			}
			
			
			// entrée
			if(  (diks[0x1c]&0x80) && (!EnterPressed))
			{
				EnterPressed=true;
				PlaySound( g_pSphereExplodeSound );

				DWORD dwID = g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->dwID;
				switch( dwID )
				{
					case MENU_BACK:
						g_pCurrentMenu->~CMenuItem();
						g_pCurrentMenu = OldMenu;						
						CurrentIsKeyboardEntry=false;
						break;
					default:
						g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->~CMenuItem();
						switch(dwID)
						{
							case MENU_UP:
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]=
									new CMenuItem("UP   ???",MENU_UP);
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->pParent=g_pCurrentMenu;
								break;
							case MENU_DOWN:
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]=
									new CMenuItem("DOWN   ???",MENU_DOWN);
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->pParent=g_pCurrentMenu;
								break;
							case MENU_LEFT:
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]=
									new CMenuItem("LEFT   ???",MENU_LEFT);
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->pParent=g_pCurrentMenu;
								break;
							case MENU_RIGHT:
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]=
									new CMenuItem("RIGHT   ???",MENU_RIGHT);
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->pParent=g_pCurrentMenu;
								break;
						}	
						ReadingEntry=true;
						AllKeysReleased=false;
						break;			
				}
			}

		}
		else // ReadingEntry
		{
			if (!AllKeysReleased)
			{
				bool b=true;			
				for (DWORD i=0;i<255;i++)
				{
					b&=((diks[i]&0x80)!=0x80);
				}

				if (b) AllKeysReleased=true;
			}
			else
			{
				for (BYTE i=0;i<255;i++)
				{
					DWORD dwID = g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->dwID;

					if ((diks[i]&0x80))
					{
						PlaySound( g_pSphereExplodeSound );
						switch(player)
						{
						case MENU_P1C:
							switch(dwID)
							{
							case MENU_UP:
								this->Touches[0].UP=i;
								break;
							case MENU_DOWN:
								this->Touches[0].DOWN=i;
								break;
							case MENU_LEFT:
								this->Touches[0].LEFT=i;
								break;
							case MENU_RIGHT:
								this->Touches[0].RIGHT=i;
								break;
							}							
							break;
						case MENU_P2C:
							switch(dwID)
							{
							case MENU_UP:
								this->Touches[1].UP=i;
								break;
							case MENU_DOWN:
								this->Touches[1].DOWN=i;
								break;
							case MENU_LEFT:
								this->Touches[1].LEFT=i;
								break;
							case MENU_RIGHT:
								this->Touches[1].RIGHT=i;
								break;
							}							
							break;
						case MENU_P3C:
							switch(dwID)
							{
							case MENU_UP:
								this->Touches[2].UP=i;
								break;
							case MENU_DOWN:
								this->Touches[2].DOWN=i;
								break;
							case MENU_LEFT:
								this->Touches[2].LEFT=i;
								break;
							case MENU_RIGHT:
								this->Touches[2].RIGHT=i;
								break;
							}							
							break;
						} // switch
					
						g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->~CMenuItem();
						switch(dwID)
						{
							case MENU_UP:
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]=
									new CMenuItem(Text_Concat("UP :   ",GetKeyName(i)),MENU_UP);
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->pParent=g_pCurrentMenu;
								break;
							case MENU_DOWN:
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]=
									new CMenuItem(Text_Concat("DOWN :   ",GetKeyName(i)),MENU_DOWN);
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->pParent=g_pCurrentMenu;
								break;
							case MENU_LEFT:
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]=
									new CMenuItem(Text_Concat("LEFT :   ",GetKeyName(i)),MENU_LEFT);
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->pParent=g_pCurrentMenu;
								break;
							case MENU_RIGHT:
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]=
									new CMenuItem(Text_Concat("RIGHT :   ",GetKeyName(i)),MENU_RIGHT);
								g_pCurrentMenu->pChild[g_pCurrentMenu->dwSelectedMenu]->pParent=g_pCurrentMenu;
								break;
						}						
						
						ReadingEntry=false;
						return;
					}
				}

			}



		}


	}


}

//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------

MeshManager*CreateMeshManager()
{
	return new MeshManager();	
}

MeshStruct*CreateMeshStruct()
{
	return new MeshStruct();	
}

CWAYPOINT*CreateWAYPOINT(float _minx,float _maxx,float _miny,float _maxy,float _minz,float _maxz,DWORD _mint,DWORD _maxt)
{
	return new CWAYPOINT(_minx,_maxx,_miny,_maxy,_minz,_maxz,_mint,_maxt);
}

CCOMPUTER*CreateComputer()
{
	return new CCOMPUTER();	
}


HRESULT CMyD3DApplication::OneTimeSceneInit()
{

	for (DWORD k=0;k<4;k++)
		this->WayPoints[k]=CreateWAYPOINT(-0.5f,0.5f,-0.5f,0.5f,0,0.5f,1000,1100);

	this->computers[0]=CreateComputer();
	this->computers[1]=CreateComputer();
	this->computers[2]=CreateComputer();

	CreateSoundObjects(this->m_hWnd);
	credits_hbmp = (HBITMAP)LoadImage( GetModuleHandle( NULL ),
                                      TEXT("credits"), IMAGE_BITMAP,
                                      0, 0, LR_CREATEDIBSECTION );

	ConstructMenus();
	this->g_pCurrentMenu=this->g_pMainMenu;

	this->MONDE=CreateMeshManager();
	this->Sphere=CreateMeshStruct();
	this->bgPLANE=CreateMeshStruct();

	
	__try
	{
		FILE*f=fopen("keyboard.cfg","rb");
		if(f!=NULL)
		{
			fread(&this->Touches[0],sizeof(this->Touches),1,f);
			fclose(f);
		}
		else
		{
			this->Touches[0].UP=0xc8;
			this->Touches[0].DOWN=0xd0;
			this->Touches[0].LEFT=0xcb;
			this->Touches[0].RIGHT=0xcd;
			
			this->Touches[1].UP=0x11;
			this->Touches[1].DOWN=0x1f;
			this->Touches[1].LEFT=0x1e;
			this->Touches[1].RIGHT=0x20;

			this->Touches[2].UP=0x17;
			this->Touches[2].DOWN=0x25;
			this->Touches[2].LEFT=0x24;
			this->Touches[2].RIGHT=0x26;

			FILE*f=fopen("keyboard.cfg","wb");
			if(f!=NULL)
			{
				fwrite(&this->Touches[0],sizeof(this->Touches),1,f);
				fclose(f);
			}
		}
	}
	__except(1)
	{
		this->Touches[0].UP=0xc8;
		this->Touches[0].DOWN=0xd0;
		this->Touches[0].LEFT=0xcb;
		this->Touches[0].RIGHT=0xcd;
		
		this->Touches[1].UP=0x11;
		this->Touches[1].DOWN=0x1f;
		this->Touches[1].LEFT=0x1e;
		this->Touches[1].RIGHT=0x20;

		this->Touches[2].UP=0x17;
		this->Touches[2].DOWN=0x25;
		this->Touches[2].LEFT=0x24;
		this->Touches[2].RIGHT=0x26;

		FILE*f=fopen("keyboard.cfg","wb");
		if(f!=NULL)
		{
			fwrite(&this->Touches[0],sizeof(this->Touches),1,f);
			fclose(f);
		}
	}
	return S_OK;
}

DWORD current[4];
BYTE classification[4];
//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{	
	
	if (LastT==0)
	{
		DeltaT=0;
		LastT=timeGetTime();
	}
	else
	{
		DWORD T=timeGetTime();
		DeltaT=T-LastT;
		LastT=T;		
	}

	if (this->app_state==APP_STATE_PLAYING)
	{
		if((this->nPlayers+this->nCPU)>0)
		{
			if(app_substate==APP_SUBSTATE_PLAYING)
			{
				BYTE state;
				this->MONDE->UpdatePosition(this->nPlayers+this->nCPU,&state,DeltaT);

				
				Greatest_lap=0;
				bool	finished=true;
	
				for(int	i=0;i<(int)(this->nCPU+this->nPlayers);i++)
				{
					if(Greatest_lap<MONDE->Cars[i]->tour)
						Greatest_lap=MONDE->Cars[i]->tour;
	
					if(MONDE->Cars[i]->tour>=3)
					{
						MONDE->Cars[i]->State.car_state=CAR_STATE_FINISHED;
					}
					else
						finished=false;
				}

				if(finished)
				{
					this->app_substate=APP_SUBSTATE_OUTROANIM;
					return	S_OK;
				}

				
				
				if (g_pMusicManager)
				{
					if(!g_pBounceSound->IsPlaying())
						bounce=false;
					if((state==1) && (!bounce))
					{
						bounce=true;
						PlaySound(g_pBounceSound);
					}

					if(!g_pBeepSound->IsPlaying())
						beep=false;
					if((state==2)&&(!beep))
					{
						beep=true;
						PlaySound(g_pBeepSound);
					}
				}
			}

			float maxy=-1.0e30f;

			D3DXVECTOR3 lookat1=D3DXVECTOR3(0,0,0);
			for(DWORD i=0;i<this->nPlayers+this->nCPU;i++)
			{
				lookat1=lookat1+D3DXVECTOR3(MONDE->Cars[i]->Mesh->matrice(3,0),MONDE->Cars[i]->Mesh->matrice(3,1),MONDE->Cars[i]->Mesh->matrice(3,2));
				if (MONDE->Cars[i]->Mesh->matrice(3,1)>maxy)
					maxy=MONDE->Cars[i]->Mesh->matrice(3,1);
			}
			lookat1=lookat1/(float)(nPlayers+nCPU);
			lookat1.y=maxy;

			if (GameStarted)
				lookat=0.8f*lookat+0.2f*lookat1;
			else
			{
				GameStarted=true;
				lookat=lookat1;
			}

			eye=lookat+D3DXVECTOR3(0,5,0);
			up1=D3DVEC(1,0,0)^D3DVECTOR(lookat-eye);
			up1=up1/~up1;
		}

		D3DXMatrixLookAtLH( &matView, &eye,
		    				  &lookat,
							  &up1);
		DWORD x = m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW, &matView );
		D3DXMatrixInverse( &m_matBillboard, NULL, &matView );
		m_matBillboard._41 = 0.0f;
		m_matBillboard._42 = 0.0f;
		m_matBillboard._43 = 0.0f;

		
		switch(this->app_substate)
		{
		case APP_SUBSTATE_PLAYING:
			{
				D3DXMATRIX mv,mp;
				memset(mv,0,sizeof(mv));
				memset(mv,0,sizeof(mp));
				DWORD i;

				x=m_pd3dDeviceUtility->GetTransform(D3DTS_VIEW, &mv);
				x=m_pd3dDeviceUtility->GetTransform(D3DTS_PROJECTION, &mp);
				D3DVIEWPORT8 viewport;
				i=m_pd3dDeviceUtility->GetViewport(&viewport);
				
				
				for( i=0;i<this->nPlayers+this->nCPU;i++)
				{
					if(this->MONDE->Cars[i]->State.car_state==CAR_STATE_BLOCKED)
					{
						this->MONDE->Cars[i]->State.car_state=CAR_STATE_ALIFE;
						if((this->nCPU>0)&&(i>=this->nCPU))
							this->computers[i-this->nCPU]->state.com_state=COM_STATE_RACING;
					}
				}
				

				for(i=0;i<this->nPlayers+this->nCPU;i++)
				{
					if(this->MONDE->Cars[i]->ChekOutOfViewport(mv,mp,viewport))
					{

						//BYTE classification[4];
						for(BYTE e=0;e<4;e++)
							classification[e]=e;
						looserIndex=0;
						winnerIndex=this->nPlayers+this->nCPU-1;
						if(MONDE->NumBalises)
						{	
							//DWORD current[4];
							float currentdist[4];
							DWORD p;
					
							for ( p=0;p<this->nPlayers+this->nCPU;p++)
							{
								D3DVECTOR pos=D3DVEC(MONDE->Cars[p]->Mesh->matrice(3,0),
													 MONDE->Cars[p]->Mesh->matrice(3,1),
													 MONDE->Cars[p]->Mesh->matrice(3,2));
								DWORD MMIN=0;
								float MINDIST=1e30f;
								for (DWORD b=0;b<MONDE->NumBalises;b++)
								{
									D3DVECTOR KP=pos-MONDE->Balises[b].K;
									float ffff=~KP;
									if ((ffff<MINDIST) && (ffff<(10*MONDE->Cars[p]->Mesh->TransformedRadius)))
									{
										MMIN=b;
										MINDIST=ffff;
									}									
								} // for b

								current[p]=MMIN;
								currentdist[p]=MINDIST;
							} // for p
						
							for (p=0;p<this->nPlayers+this->nCPU-1;p++)
							{
								for (DWORD q=p+1;q<this->nPlayers+this->nCPU;q++)
								{
									if  ((MONDE->Cars[classification[p]]->tour>MONDE->Cars[classification[q]]->tour)
										||
										((MONDE->Cars[classification[p]]->tour==MONDE->Cars[classification[q]]->tour) && (current[classification[p]]>current[classification[q]]) && (current[classification[p]]<=MONDE->Cars[classification[p]]->balise) && (current[classification[q]]<=MONDE->Cars[classification[q]]->balise))
										||
										((MONDE->Cars[classification[p]]->tour==MONDE->Cars[classification[q]]->tour) && (current[classification[p]]>current[classification[q]]) && (current[classification[p]]>MONDE->Cars[classification[p]]->balise) && (current[classification[q]]>MONDE->Cars[classification[q]]->balise))
										||
										((MONDE->Cars[classification[p]]->tour==MONDE->Cars[classification[q]]->tour) && (current[classification[p]]<=MONDE->Cars[classification[p]]->balise) && (current[classification[q]]>MONDE->Cars[classification[q]]->balise))
										||
										((MONDE->Cars[classification[p]]->tour==MONDE->Cars[classification[q]]->tour) && (current[classification[p]]==current[classification[q]]) && (currentdist[classification[p]]>currentdist[classification[q]])))
									{
										BYTE temp=classification[p];
										classification[p]=classification[q];
										classification[q]=temp;
									}
									
								} // for q
							} // for p

							looserIndex=classification[0];
							winnerIndex=classification[this->nPlayers+this->nCPU-1];

						} // if (numbalises)

						

						for (DWORD j=0;j<this->nPlayers+this->nCPU;j++)
							this->MONDE->Cars[j]->State.car_state=CAR_STATE_BLOCKED;

						this->ss1=10;
						this->app_substate=APP_SUBSTATE_WINANIM;
						this->AnimCam.initial=lookat;						
						this->AnimCam.to=D3DVEC(MONDE->Cars[looserIndex]->Mesh->matrice(3,0),
													MONDE->Cars[looserIndex]->Mesh->matrice(3,1),
													MONDE->Cars[looserIndex]->Mesh->matrice(3,2));
						this->AnimCam.LastT=timeGetTime();
						break;
					}									
				}
			}
			break;
		case APP_SUBSTATE_WINANIM:
			{
				switch(this->ss1)
				{
				case 10:
					{
						DWORD T=timeGetTime();
						float f=(T-this->AnimCam.LastT)/1000.0f;
						if (f>1) f=1;
						this->lookat=this->AnimCam.to*f+
							         (1-f)*this->AnimCam.initial;
						eye=lookat+D3DXVECTOR3(0,5,0);

						D3DXMatrixLookAtLH( &matView, &eye,
		    						  &lookat,
									  &up1);
						m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW, &matView );
						D3DXMatrixInverse( &m_matBillboard, NULL, &matView );
						m_matBillboard._41 = 0.0f;
						m_matBillboard._42 = 0.0f;
						m_matBillboard._43 = 0.0f;

						
						if (f>=1)
						{
							MONDE->Cars[looserIndex]->State.car_state=CAR_STATE_EXPLODING;
							MONDE->Cars[looserIndex]->State.sub_state=0;
							MONDE->Cars[looserIndex]->State.s1=0;
							this->ss1=0;
						}							
					}
					break;
				case 0:
					{
						this->lookat=this->AnimCam.to;							     
						eye=lookat+D3DXVECTOR3(0,5,0);

						D3DXMatrixLookAtLH( &matView, &eye,
		    						  &lookat,
									  &up1);
						m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW, &matView );
						D3DXMatrixInverse( &m_matBillboard, NULL, &matView );
						m_matBillboard._41 = 0.0f;
						m_matBillboard._42 = 0.0f;
						m_matBillboard._43 = 0.0f;

						if (MONDE->Cars[looserIndex]->State.car_state==CAR_STATE_BLOCKED)
						{
							this->AnimCam.initial=this->AnimCam.to;
							this->AnimCam.to=D3DVEC(MONDE->Cars[winnerIndex]->Mesh->matrice(3,0),
													MONDE->Cars[winnerIndex]->Mesh->matrice(3,1),
													MONDE->Cars[winnerIndex]->Mesh->matrice(3,2));
							this->AnimCam.LastT=timeGetTime();
							this->ss1=1;
						}
						break;
					}
					break;
				case 1:
					{
						DWORD T=timeGetTime();
						float f=(T-this->AnimCam.LastT)/1000.0f;
						if (f>1) f=1;
						this->lookat=this->AnimCam.to*f+
							         (1-f)*this->AnimCam.initial;
						eye=lookat+D3DXVECTOR3(0,5,0)+D3DXVECTOR3(0,0,5)*f;
						up1=D3DVEC(0,1,0)^D3DVECTOR(lookat-eye);
						up1=D3DVECTOR(lookat-eye)^up1;    
						if (up1.x==0 && up1.y==0 && up1.z==0)
							up1=D3DVEC(1,0,0)^D3DVECTOR(lookat-eye);
						up1=up1/~up1;
				
						D3DXMatrixLookAtLH( &matView, &eye,
		    				  &lookat,
							  &up1);
						m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW, &matView );
						D3DXMatrixInverse( &m_matBillboard, NULL, &matView );
						m_matBillboard._41 = 0.0f;
						m_matBillboard._42 = 0.0f;
						m_matBillboard._43 = 0.0f;

						if (f>=1)
						{
							PlaySound(g_pLOLSound);
							this->AnimCam.LastT=T;
							this->ss1=2;
							MONDE->Cars[winnerIndex]->State.car_state=CAR_STATE_WIN_ANIM;
							MONDE->Cars[winnerIndex]->State.sub_state=0;
						}
						break;
					}
					break;
				case 2:
					{
						DWORD T=timeGetTime();
						float f=(T-this->AnimCam.LastT)/1000.0f;
						this->lookat=this->AnimCam.to;							         
						
						if (f>=6) f=6;				
						
						eye=lookat+D3DXVECTOR3(0,5,0)+D3DXVECTOR3(float(5*sin(2*PI*f/3)),0,float(5*cos(2*PI*f/3)) );
						up1=D3DVEC(0,1,0)^D3DVECTOR(lookat-eye);
						up1=D3DVECTOR(lookat-eye)^up1;    
						if (up1.x==0 && up1.y==0 && up1.z==0)
							up1=D3DVEC(1,0,0)^D3DVECTOR(lookat-eye);
						up1=up1/~up1;
				
						D3DXMatrixLookAtLH( &matView, &eye,
		    				  &lookat,
							  &up1);
						m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW, &matView );
						D3DXMatrixInverse( &m_matBillboard, NULL, &matView );
						m_matBillboard._41 = 0.0f;
						m_matBillboard._42 = 0.0f;
						m_matBillboard._43 = 0.0f;

						if ((f>=6)&&(MONDE->Cars[winnerIndex]->State.car_state==CAR_STATE_BLOCKED))
						{
							this->AnimCam.LastT=T;
							this->ss1=3;
							this->AnimCam.initial=this->AnimCam.to;
							this->AnimCam.to=AnimCam.initial;
						}
						break;
					}
					break;	
				case 3:
					{
						DWORD T=timeGetTime();
						float f=(T-this->AnimCam.LastT)/1000.0f;
						if (f>=0.5f) f=0.5f;						
						this->lookat=this->AnimCam.to*f*2+2*(0.5f-f)*this->AnimCam.initial;
						
						
						eye=lookat+D3DXVECTOR3(0,5,0)+D3DXVECTOR3(0,0,(0.5f-f)*5*2 );
						up1=D3DVEC(0,1,0)^D3DVECTOR(lookat-eye);
						up1=D3DVECTOR(lookat-eye)^up1;    
						if (up1.x==0 && up1.y==0 && up1.z==0)
							up1=D3DVEC(1,0,0)^D3DVECTOR(lookat-eye);
						up1=up1/~up1;
						

						D3DXMatrixLookAtLH( &matView, &eye,
		    				  &lookat,
							  &up1);
						m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW, &matView );
						D3DXMatrixInverse( &m_matBillboard, NULL, &matView );
						m_matBillboard._41 = 0.0f;
						m_matBillboard._42 = 0.0f;
						m_matBillboard._43 = 0.0f;

						bool c=true;
						DWORD i;
						for ( i=0;i<this->nPlayers+this->nCPU;i++)
						{
							if (this->MONDE->Cars[i]->State.car_state!=CAR_STATE_BLOCKED)
							{
								c=false;
							}
						}

						if (c)
						{
							for (i=0;i<this->nPlayers+this->nCPU;i++)
							{
								this->MONDE->Cars[i]->State.car_state=CAR_STATE_BLOCKED;
								this->MONDE->Cars[i]->velocity=D3DVEC(0,0,0);
							}

							this->ss1=4;
							this->AnimCam.LastT=timeGetTime();
							this->AnimCam.initial=AnimCam.to;
							

							if (MONDE->NumBalises)
							{
								MONDE->Cars[winnerIndex]->Mesh->matrice(3,0)=
									MONDE->Balises[MONDE->Cars[winnerIndex]->balise].pos.x;
								MONDE->Cars[winnerIndex]->Mesh->matrice(3,1)=
									MONDE->Balises[MONDE->Cars[winnerIndex]->balise].pos.y;
								MONDE->Cars[winnerIndex]->Mesh->matrice(3,2)=
									MONDE->Balises[MONDE->Cars[winnerIndex]->balise].pos.z;

								AnimCam.to=MONDE->Balises[MONDE->Cars[winnerIndex]->balise].pos;
							}


							D3DVECTOR orientation=MONDE->Balises[MONDE->Balises[MONDE->Cars[winnerIndex]->balise].index_next].pos-MONDE->Balises[MONDE->Cars[winnerIndex]->balise].pos;
							orientation=orientation/~orientation;

							D3DVECTOR V=D3DVEC(0,0,0);
							float maxy=-1e20f;
							for (DWORD i=0;i<this->nCPU+this->nPlayers;i++)
							{
								MONDE->Cars[i]->State.car_state=CAR_STATE_EXPLODING;
								MONDE->Cars[i]->State.sub_state=0;
								MONDE->Cars[i]->State.s1=0;
								MONDE->Cars[i]->Mesh->matrice(3,0)=
									(this->AnimCam.to.x+2.5f*(i/2)*MONDE->Cars[i]->Mesh->TransformedRadius);
								MONDE->Cars[i]->Mesh->matrice(3,1)=
									(this->AnimCam.to.y+0.2f*MONDE->Cars[i]->Mesh->TransformedRadius);
								MONDE->Cars[i]->Mesh->matrice(3,2)=
									(this->AnimCam.to.z+2.5f*(i%2)*MONDE->Cars[i]->Mesh->TransformedRadius);

								V.x+=MONDE->Cars[i]->Mesh->matrice(3,0);
								V.y+=MONDE->Cars[i]->Mesh->matrice(3,1);
								V.z+=MONDE->Cars[i]->Mesh->matrice(3,2);
								if (MONDE->Cars[i]->Mesh->matrice(3,1)>maxy)
									maxy=MONDE->Cars[i]->Mesh->matrice(3,1);

								//orientation selon les balises
								D3DVECTOR ox=D3DVEC(MONDE->Cars[i]->Mesh->matrice(0,0),MONDE->Cars[i]->Mesh->matrice(0,1),MONDE->Cars[i]->Mesh->matrice(0,2));
								D3DVECTOR oy=D3DVEC(MONDE->Cars[i]->Mesh->matrice(1,0),MONDE->Cars[i]->Mesh->matrice(1,1),MONDE->Cars[i]->Mesh->matrice(1,2));
								D3DVECTOR oz=D3DVEC(MONDE->Cars[i]->Mesh->matrice(2,0),MONDE->Cars[i]->Mesh->matrice(2,1),MONDE->Cars[i]->Mesh->matrice(2,2));
								ox=~ox*orientation;
								D3DVECTOR oy1=(ox^oy)^ox;
								oy=(~oy/~oy1)*oy1;
								D3DVECTOR oz1=ox^oy;
								oz=(~oz/~oz1)*oz1;

								MONDE->Cars[i]->Mesh->matrice(0,0)=ox.x;
								MONDE->Cars[i]->Mesh->matrice(0,1)=ox.y;
								MONDE->Cars[i]->Mesh->matrice(0,2)=ox.z;
								MONDE->Cars[i]->Mesh->matrice(1,0)=oy.x;
								MONDE->Cars[i]->Mesh->matrice(1,1)=oy.y;
								MONDE->Cars[i]->Mesh->matrice(1,2)=oy.z;
								MONDE->Cars[i]->Mesh->matrice(2,0)=oz.x;
								MONDE->Cars[i]->Mesh->matrice(2,1)=oz.y;
								MONDE->Cars[i]->Mesh->matrice(2,2)=oz.z;


							}
							V=V/float(this->nCPU+this->nPlayers);
							V.y=maxy;
						}
						break;
					}
					break;
				case 4:
					{
						
						DWORD T=timeGetTime();
						float f=(T-this->AnimCam.LastT)/1000.0f;
						if (f>=1) f=1;						
						this->lookat=this->AnimCam.to*f+(1-f)*this->AnimCam.initial;
						
						
						eye=lookat+D3DXVECTOR3(0,5,0);
						up1=D3DVEC(1,0,0)^D3DVECTOR(lookat-eye);
						up1=up1/~up1;
						

						D3DXMatrixLookAtLH( &matView, &eye,
		    				  &lookat,
							  &up1);
						m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW, &matView );
						D3DXMatrixInverse( &m_matBillboard, NULL, &matView );
						m_matBillboard._41 = 0.0f;
						m_matBillboard._42 = 0.0f;
						m_matBillboard._43 = 0.0f;
						
						if (f>=1)
						{
							for (DWORD i=0;i<this->nPlayers+this->nCPU;i++)
							{
								this->MONDE->Cars[i]->State.car_state=CAR_STATE_ALIFE;
								this->MONDE->Cars[i]->velocity=D3DVEC(0,0,0);
								this->MONDE->Cars[i]->balise=MONDE->Cars[winnerIndex]->balise;
								this->MONDE->Cars[i]->tour=MONDE->Cars[winnerIndex]->tour;
								if(i>=this->nPlayers)
									this->computers[i-this->nPlayers]->Init(MONDE,Intelligence);
							}
							this->app_substate=APP_SUBSTATE_PLAYING;
							MONDE->Cars[winnerIndex]->vie++;
							MONDE->Cars[looserIndex]->vie--;
							
							if (MONDE->Cars[winnerIndex]->vie==8)
							{
								this->app_substate=APP_SUBSTATE_OUTROANIM;								
							}

							if (MONDE->Cars[looserIndex]->vie==0)
							{
								this->app_substate=APP_SUBSTATE_OUTROANIM;								
							}



						}					
					}
					break;
				} // switch

			}
			break;
		case APP_SUBSTATE_INTROANIM:
			{
				if ((diks[0x1c]&0x80) !=0x80) EnterPressed=false;
				if ((diks[0x1c]&0x80) && (!EnterPressed)) 
				{
					EnterPressed=true;
					this->app_substate=APP_SUBSTATE_PLAYING;
				}
				break;
			}
		case APP_SUBSTATE_OUTROANIM:
			{
				// to be continued ...
				if ((diks[0x1c]&0x80) !=0x80) EnterPressed=false;
				if ((diks[0x1c]&0x80) && (!EnterPressed)) 
				{
					EnterPressed=true;
					this->app_state=APP_STATE_MENU;
					this->g_pCurrentMenu=this->g_pMainMenu;
				}
				break;
			}

		} // switch

	}

	// Check for game menu condition
	if( this->app_state!=APP_STATE_PLAYING )
	{
		if((!MetallicaPlaying) && (g_pMetallicaSound))
		{
			MetallicaPlaying=true;
			PlaySound(g_pMetallicaSound);
		}

		if(MetropolisPlaying)
		{
			MetropolisPlaying=false;
			StopSound(g_pMetropolisSound);
		}
		UpdateMenus();
	}
	else
	{
		if(MetallicaPlaying)
		{
			MetallicaPlaying=false;
			StopSound(g_pMetallicaSound);
		}

		if((!MetropolisPlaying) && (g_pMetropolisSound))
		{
			MetropolisPlaying=true;
			PlaySound(g_pMetropolisSound);
		}	

		if(this->app_substate==APP_SUBSTATE_PLAYING)
		{
			for(DWORD	i=0;i<4;i++)
				MONDE->Cars[i]->velocity=MONDE->Cars[i]->velocity*0.95f;

			ArtificialIntelligence(DeltaT);
			MONDE->CheckClavier(this->nPlayers,diks,DeltaT);
		}

	}


	return S_OK;
}

 // (pos,rx,ry) % viewport 
void CMyD3DApplication::DrawBillBoard(D3DVECTOR pos, float rx,float ry,
									  LPDIRECT3DTEXTURE8 tex,
									  D3DMATERIAL8* mat)
{
		D3DXMATRIX m_matBillboardMatrix,m1,m4;
		D3DXVECTOR3 vDir = lookat - eye;
		if( vDir.y > 0.0f )
			D3DXMatrixRotationZ( &m_matBillboardMatrix, -atanf(vDir.x/vDir.y)+D3DX_PI/2 );
		else
			D3DXMatrixRotationZ( &m_matBillboardMatrix, -atanf(vDir.x/vDir.y)-D3DX_PI/2 );


		D3DXMatrixScaling(&m4,rx,ry,1.0);
		D3DXMatrixRotationZ(&m1,-3.1415f/2.0);
		m_matBillboardMatrix=m4*m1*m_matBillboardMatrix;

		m_matBillboardMatrix(3,0)=pos.x;
		m_matBillboardMatrix(3,1)=pos.y;
		m_matBillboardMatrix(3,2)=pos.z;


		m_pd3dDeviceUtility->SetTransform(D3DTS_WORLD,&m_matBillboardMatrix);

		m_pd3dDevice->SetMaterial( mat );				
		m_pd3dDevice->SetTexture ( 0, tex );

		D3DLIGHT8 l8,l81;
		BOOL bb;

		l8.Type=D3DLIGHT_POINT;
		l8.Diffuse=(*mat).Diffuse;
		l8.Ambient=(*mat).Diffuse;
		l8.Specular=(*mat).Specular;
		l8.Position=pos;
		l8.Range=10;
		l8.Falloff=1;
		l8.Attenuation0=1;
		l8.Attenuation1=1;
		l8.Attenuation2=1;

		l8.Phi=0;
		l8.Theta=0;

		m_pd3dDeviceUtility->GetLight(0,&l81);
		m_pd3dDeviceUtility->GetLightEnable(0,&bb);	
		m_pd3dDeviceUtility->SetLight(0,&l8);
		m_pd3dDeviceUtility->LightEnable(0,true);	

		
		DWORD oldAlpha;
		DWORD scrb,destb;
		
		m_pd3dDeviceUtility->GetRenderState(D3DRS_ALPHABLENDENABLE,&oldAlpha);
		m_pd3dDeviceUtility->GetRenderState(D3DRS_SRCBLEND ,&scrb);
		m_pd3dDeviceUtility->GetRenderState(D3DRS_DESTBLEND,&destb);
		
		m_pd3dDeviceUtility->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		m_pd3dDeviceUtility->SetRenderState(D3DRS_SRCBLEND ,D3DBLEND_SRCCOLOR );
		m_pd3dDeviceUtility->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR );

		bgPLANE->mesh->DrawSubset(0);

		m_pd3dDeviceUtility->SetLight(0,&l81);
		m_pd3dDeviceUtility->LightEnable(0,bb);	

		m_pd3dDeviceUtility->SetRenderState(D3DRS_ALPHABLENDENABLE,oldAlpha);
		m_pd3dDeviceUtility->SetRenderState(D3DRS_SRCBLEND ,scrb);
		m_pd3dDeviceUtility->SetRenderState(D3DRS_DESTBLEND,destb);
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
   
    // Begin the scene 
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// Clear the viewport
	    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x00, 1.0f, 0L );
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0x00303080, 1.0f, 0L );

		D3DVIEWPORT8 vp;
		m_pd3dDeviceUtility->GetViewport(&vp);
		m_pd3dDeviceUtility->SetViewport(&vp);


		// set up view and projection matrices
		// yeah , cool
		FOV=PI/4;
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,FOV,1.0f,1.0f,100.0f);
		
	
		m_pd3dDeviceUtility->SetTransform(D3DTS_PROJECTION,&proj);

		m_pd3dDeviceUtility->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

		// Turn on the zbuffer
	    m_pd3dDeviceUtility->SetRenderState( D3DRS_ZENABLE, true );

		// Turn on ambient lighting 
		m_pd3dDeviceUtility->SetRenderState( D3DRS_AMBIENT, 0x50101010 );

		
		// Quelques States
		m_pd3dDeviceUtility->SetRenderState(D3DRS_ZENABLE,true);
		m_pd3dDeviceUtility->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		m_pd3dDeviceUtility->SetRenderState(D3DRS_ZWRITEENABLE,true);

		m_pd3dDeviceUtility->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		
		m_pd3dDeviceUtility->SetRenderState(D3DRS_ALPHATESTENABLE,false);
		m_pd3dDeviceUtility->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		
		m_pd3dDeviceUtility->SetRenderState(D3DRS_SRCBLEND ,D3DBLEND_SRCALPHA);
		m_pd3dDeviceUtility->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA );

		m_pd3dDeviceUtility->SetRenderState(D3DRS_DITHERENABLE,true);
		m_pd3dDeviceUtility->SetRenderState(D3DRS_SPECULARENABLE,true);
		
		

	    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0x00303080, 1.0f, 0L );


		//////// this->is->the->render->function
		//////// but it contains some moving functionalities
		//             /\ nécessité oblige /\ 

		if(this->app_state==APP_STATE_PLAYING)
		{
			D3DXMATRIX mw,mp,mv;

			m_pd3dDeviceUtility->GetTransform(D3DTS_VIEW,&mv);
			D3DXMatrixIdentity(&mw);
			m_pd3dDeviceUtility->GetTransform(D3DTS_PROJECTION,&mp);
			D3DVIEWPORT8 viewport;
			m_pd3dDeviceUtility->GetViewport(&viewport);
			


			int EffectiveNumCars=nPlayers+nCPU;

			int nc=0;
			for (DWORD i=0;i<MONDE->NumMeshes;i++)
			{
				if(MONDE->Meshes[i]->flag!=FLAG_CAR)
					MONDE->Meshes[i]->Render(m_pd3dDevice,m_pd3dDeviceUtility);
				else
				{
					nc++;
					if (nc<=EffectiveNumCars)
					{
						if(nc>2)
							nc=nc;
						switch(MONDE->Cars[nc-1]->State.car_state)
						{
						case CAR_STATE_ALIFE:
						case CAR_STATE_BLOCKED:
						case CAR_STATE_FINISHED:
							MONDE->Meshes[i]->Render(m_pd3dDevice,m_pd3dDeviceUtility);
							break;
						case CAR_STATE_EXPLODING:
						{
							D3DXVECTOR3 pos=D3DVEC(MONDE->Meshes[i]->matrice(3,0),MONDE->Meshes[i]->matrice(3,1),MONDE->Meshes[i]->matrice(3,2));
							D3DXVECTOR3 pos1=D3DVEC(MONDE->Meshes[i]->matrice(3,0)+MONDE->Meshes[i]->TransformedRadius,MONDE->Meshes[i]->matrice(3,1),MONDE->Meshes[i]->matrice(3,2));

							D3DXVECTOR3 posp;
							D3DXVECTOR3 pos1p;

							D3DXVec3Project(&posp,&pos,&viewport,&mp,&mv,&mw);
							D3DXVec3Project(&pos1p,&pos1,&viewport,&mp,&mv,&mw);

							float rrr=~(posp-pos1p);

							DWORD red = 255-(int)(MONDE->Cars[nc-1]->State.s1*255.0f);
							DWORD grn = 255-(int)(MONDE->Cars[nc-1]->State.s1*511.0f);
							DWORD blu = 255-(int)(MONDE->Cars[nc-1]->State.s1*1023.0f);
							if( grn > 255 ) grn = 0;
							if( blu > 255 ) blu = 0;

							DWORD DeltaT;
							switch(MONDE->Cars[nc-1]->State.sub_state)
							{
							case 0:
								PlaySound(g_pExplodeSound);
								MONDE->Cars[nc-1]->State.LastT=timeGetTime();
								DeltaT=0;
								MONDE->Cars[nc-1]->State.sub_state=1;
								break;
							case 1:								
								DWORD T=timeGetTime();
								DeltaT=T-MONDE->Cars[nc-1]->State.LastT;
								MONDE->Cars[nc-1]->State.LastT=T;								
								break;
							}

							MONDE->Cars[nc-1]->State.s1+=DeltaT/2000.0f;
							if (MONDE->Cars[nc-1]->State.s1>1) 
							{
								MONDE->Cars[nc-1]->State.car_state=CAR_STATE_BLOCKED;
								MONDE->Cars[nc-1]->State.s1=0;
							}

							bgPLANE->MeshMaterials[0].Diffuse.r=red/255.0f;
							bgPLANE->MeshMaterials[0].Diffuse.b=blu/255.0f;
							bgPLANE->MeshMaterials[0].Diffuse.g=grn/255.0f;
							bgPLANE->MeshMaterials[0].Diffuse.a=1.0f;

							bgPLANE->MeshMaterials[0].Specular=bgPLANE->MeshMaterials[0].Diffuse;
							bgPLANE->MeshMaterials[0].Ambient=bgPLANE->MeshMaterials[0].Diffuse;
							bgPLANE->MeshMaterials[0].Emissive=bgPLANE->MeshMaterials[0].Diffuse;


							bgPLANE->MeshMaterials[0].Power=20.0f;							
							
							DWORD saves[50];							
							m_pd3dDeviceUtility->GetTextureStageState( 0, D3DTSS_COLORARG1, &saves[ 0] );
							m_pd3dDeviceUtility->GetTextureStageState( 0, D3DTSS_COLORARG2, &saves[ 1] );
							m_pd3dDeviceUtility->GetTextureStageState( 0, D3DTSS_COLOROP,   &saves[ 2] );
							m_pd3dDeviceUtility->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &saves[ 3] );
							m_pd3dDeviceUtility->GetTextureStageState( 0, D3DTSS_ALPHAOP,   &saves[ 4] );							
							m_pd3dDeviceUtility->GetTextureStageState( 0, D3DTSS_MINFILTER, &saves[ 5] );
							m_pd3dDeviceUtility->GetTextureStageState( 0, D3DTSS_MAGFILTER, &saves[ 6] );
							m_pd3dDeviceUtility->GetRenderState( D3DRS_ALPHABLENDENABLE,    &saves[ 7] );
							m_pd3dDeviceUtility->GetRenderState( D3DRS_DITHERENABLE,        &saves[ 8] );
							m_pd3dDeviceUtility->GetRenderState( D3DRS_ZENABLE,             &saves[ 9] );
							m_pd3dDeviceUtility->GetRenderState( D3DRS_LIGHTING,            &saves[10] );
							m_pd3dDeviceUtility->GetRenderState( D3DRS_AMBIENT,             &saves[11] );
							m_pd3dDeviceUtility->GetRenderState(D3DRS_SPECULARENABLE,       &saves[12] );
							m_pd3dDeviceUtility->GetRenderState(D3DRS_ZWRITEENABLE,         &saves[13] );


							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

							m_pd3dDeviceUtility->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_DITHERENABLE, FALSE );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_ZENABLE,      false );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_LIGHTING,     TRUE );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_AMBIENT, 0x33330000 );
							m_pd3dDeviceUtility->SetRenderState(D3DRS_SPECULARENABLE,false);
							m_pd3dDeviceUtility->SetRenderState(D3DRS_ZWRITEENABLE,false);

							
							this->DrawBillBoard(pos,5*MONDE->Meshes[i]->TransformedRadius,5*MONDE->Meshes[i]->TransformedRadius,bgPLANE->MeshTextures[0],
								                &bgPLANE->MeshMaterials[0]);


							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_SPECULAR  );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_DITHERENABLE, true );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_ZENABLE,      TRUE );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_LIGHTING,     TRUE );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_AMBIENT, 0x33333333 );
							m_pd3dDeviceUtility->SetRenderState(D3DRS_ZWRITEENABLE,true);
							
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLORARG1, saves[ 0] );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLORARG2, saves[ 1] );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLOROP,   saves[ 2] );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_ALPHAARG1, saves[ 3] );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_ALPHAOP,   saves[ 4] );							
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_MINFILTER, saves[ 5] );
							m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_MAGFILTER, saves[ 6] );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_ALPHABLENDENABLE,    saves[ 7] );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_DITHERENABLE,        saves[ 8] );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_ZENABLE,             saves[ 9] );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_LIGHTING,            saves[10] );
							m_pd3dDeviceUtility->SetRenderState( D3DRS_AMBIENT,             saves[11] );
							m_pd3dDeviceUtility->SetRenderState(D3DRS_SPECULARENABLE,       saves[12] );
							m_pd3dDeviceUtility->SetRenderState(D3DRS_ZWRITEENABLE,         saves[13] );


							break;
						}
						case CAR_STATE_WIN_ANIM:
							{
								switch(MONDE->Cars[nc-1]->State.sub_state)
								{
								case 0:
									MONDE->Cars[nc-1]->State.LastT=timeGetTime();
									MONDE->Cars[nc-1]->State.matrice=MONDE->Cars[nc-1]->Mesh->matrice;									
									MONDE->Cars[nc-1]->State.sub_state=1;
									break;
								case 1:
									break;
								}							
								
								D3DXMATRIX m1,m2,m3;
								DWORD T=timeGetTime();
								D3DXVECTOR3 ax=D3DVEC(0,1,0);
								D3DXMatrixRotationAxis(&m1,&ax,2*PI*(T-MONDE->Cars[nc-1]->State.LastT)/1000.0f);
								m2=m1*MONDE->Cars[nc-1]->State.matrice;
								
								float f=(float)(T-MONDE->Cars[nc-1]->State.LastT)/1000.0f;
								DWORD F=T-MONDE->Cars[nc-1]->State.LastT;
								if (F<=1000)
								{
									m2(3,1)+=4*(float)(MONDE->Cars[nc-1]->Mesh->TransformedRadius*sin(PI*f));
								}
								else if (F<=2000)
								{
									m2(3,1)+=(float)(MONDE->Cars[nc-1]->Mesh->TransformedRadius*fabs(sin(PI*f)));
								}
								else if (F<=3000)
								{
									m2(3,1)+=4*(float)(MONDE->Cars[nc-1]->Mesh->TransformedRadius*fabs(sin(PI*f)));
								}
								else if (F<=4000)
								{
									m2(3,1)+=(float)(MONDE->Cars[nc-1]->Mesh->TransformedRadius*fabs(sin(PI*f)));
								}
								else if (F<=5000)
								{
									m2(3,1)+=(float)(4*MONDE->Cars[nc-1]->Mesh->TransformedRadius*fabs(sin(PI*f)));
								}
								else if (F<=6000)
								{
									m2(3,1)+=(float)(MONDE->Cars[nc-1]->Mesh->TransformedRadius*fabs(sin(PI*f)));
								}
								else
								{
									MONDE->Cars[nc-1]->State.car_state=CAR_STATE_BLOCKED;
									MONDE->Cars[nc-1]->Mesh->matrice=MONDE->Cars[nc-1]->State.matrice;
									break;
								}
								MONDE->Cars[nc-1]->Mesh->matrice=m2;
								MONDE->Cars[nc-1]->Mesh->Render(m_pd3dDevice,m_pd3dDeviceUtility);
								break;
							}
						}
					}
				}
			}
		}




		//	dessin	du	lap
		if((this->app_substate!=APP_SUBSTATE_INTROANIM)&&(this->app_substate!=APP_SUBSTATE_OUTROANIM))
		{
			char*s=IntToStr(this->Greatest_lap+1);
			char*s3=Text_Concat(s,"/3");
			D3DVIEWPORT8	vp;
			this->m_pd3dDeviceUtility->GetViewport(&vp);
			SIZE	size;
			m_pFont->GetTextExtent(s3,&size);
			DWORD	x;
			x=vp.Width/2;
			x=x-size.cx/2;
			m_pFont->DrawText((float) x , (float)vp.Height-size.cy, D3DCOLOR_ARGB(255,10,10,0x80), s3 );
			free(s);
			free(s3);
		}

		if((this->app_substate==APP_SUBSTATE_OUTROANIM))
		{
			DWORD	indices[4];
			indices[0]=0;
			indices[1]=1;
			indices[2]=2;
			indices[3]=3;

			//	tri	par	selection
			int	index;
			DWORD vieg;
			int i;
			for(i=0;i<(int)(this->nCPU+this->nPlayers-1);i++)
			{
				index=i;
				vieg=MONDE->Cars[indices[i]]->vie;
				for(DWORD	j=i+1;j<this->nCPU+this->nPlayers;j++)
				{
					if(MONDE->Cars[indices[j]]->vie>vieg)
					{
						vieg=MONDE->Cars[indices[j]]->vie;
						index=j;
					}
				}

				if(i!=index)
				{
					DWORD	temp;
					temp=indices[i];
					indices[i]=indices[index];
					indices[index]=temp;
				}
			}
			

			D3DVIEWPORT8	vp;
			this->m_pd3dDeviceUtility->GetViewport(&vp);
				
			SIZE	size;
			int	x;
			m_pFont->GetTextExtent("Ranking",&size);
			x=vp.Width/2;
			x=x-size.cx/2;				
			m_pFont->DrawText((float)x ,0, D3DCOLOR_ARGB(255,10,10,0x80),	"Ranking" );
			

			for(i=0;i<(int)(this->nCPU+this->nPlayers);i++)
			{
				DWORD	color;
				char*s;
				char*s1;
				switch(indices[i])
				{
				case	0:
					s=IntToStr(i+1);
					s1=Text_Concat(s,". Red");
					color=0xFFFF0000;
					break;
				case	1:
					s=IntToStr(i+1);
					s1=Text_Concat(s,". Green");
					color=0xFF00FF00;
					break;
				case	2:
					s=IntToStr(i+1);
					s1=Text_Concat(s,". Blue");
					color=0xFF0000FF;
					break;
				case	3:
					s=IntToStr(i+1);
					s1=Text_Concat(s,". Yellow");
					color=0xFF00FFFF;
					break;
				}
				
				m_pRankingFont->GetTextExtent(s1,&size);
				x=vp.Width/2;
				x=x-size.cx/2;
				m_pRankingFont->DrawText((float)x ,(float)(vp.Height/2)+size.cy*(i-1)*0.8f, color, s1 );
				free(s);
				free(s1);
			}

		}
				


		m_pd3dDeviceUtility->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
			
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0x00303080, 1.0f, 0L );

		D3DXMATRIX mv,mp,mw,mw1;
		D3DXMatrixIdentity(&mw);
		m_pd3dDeviceUtility->GetTransform(D3DTS_VIEW, &mv);
		m_pd3dDeviceUtility->GetTransform(D3DTS_PROJECTION, &mp);
		D3DVIEWPORT8 viewport;
		m_pd3dDeviceUtility->GetViewport(&viewport);

		D3DXMatrixIdentity(&mw1);
		

		float dx=~D3DVECTOR(eye-lookat);

		D3DVECTOR OX,OZ,OY;
		D3DXMATRIX mm;

		D3DXMatrixInverse(&mm,NULL,&this->matView);

		OX=D3DVEC(mm(0,0),mm(0,1),mm(0,2));
		OY=D3DVEC(mm(1,0),mm(1,1),mm(1,2));
		OZ=D3DVEC(mm(2,0),mm(2,1),mm(2,2));

		OX=OX/~OX;
		OY=OY/~OY;
		OZ=OZ/~OZ;
		
		
		
		mw1(0,0)=0.8f*dx*OX.x/viewport.Width;
		mw1(0,1)=0.8f*dx*OX.y/viewport.Width;
		mw1(0,2)=0.8f*dx*OX.z/viewport.Width;
		
		mw1(1,0)=0.8f*dx*OY.x/viewport.Height;
		mw1(1,1)=0.8f*dx*OY.y/viewport.Height;
		mw1(1,2)=0.8f*dx*OY.z/viewport.Height;
		
		mw1(2,0)=0.8f*dx*OZ.x/viewport.Width;
		mw1(2,1)=0.8f*dx*OZ.y/viewport.Width;
		mw1(2,2)=0.8f*dx*OZ.z/viewport.Width;
			
		
		mw1(3,0)=lookat.x-0.8f*(OX.x+OY.x)*dx/2;
		mw1(3,1)=lookat.y-0.8f*(OX.y+OY.y)*dx/2;
		mw1(3,2)=lookat.z-0.8f*(OX.z+OY.z)*dx/2;
		
		
		
		if (this->app_state==APP_STATE_PLAYING)
		{
			for (DWORD i=0;i<this->nCPU+this->nPlayers;i++)
			{
				D3DXVECTOR3 V0,Dist;			
				D3DCOLOR RED=D3DCOLOR_ARGB(255,255,0,0);
				D3DCOLOR GRE=D3DCOLOR_ARGB(255,0,255,0);
				D3DCOLOR BLU=D3DCOLOR_ARGB(255,0,0,255);
				D3DCOLOR YEL=D3DCOLOR_ARGB(255,255,255,0);
				D3DCOLOR C;			
				switch(i)
				{
				case 0:
					V0=D3DXVECTOR3(float(int(viewport.X))+15,float(int(viewport.Y))+15,0.0f);
					Dist=D3DXVECTOR3(12,0,0);
					C=RED;
					break;
				case 1:
					V0=D3DXVECTOR3(float(int(viewport.X+viewport.Width)-15),float(int(viewport.Y))+15,0.0f);
					Dist=D3DXVECTOR3(-12,0,0);
					C=GRE;
					break;
				case 2:
					V0=D3DXVECTOR3(float(int(viewport.X))+15,float(int(viewport.Y+viewport.Height)-15),0.0f);
					Dist=D3DXVECTOR3(12,0,0);
					C=BLU;
					break;
				case 3:
					V0=D3DXVECTOR3(float(int(viewport.X+viewport.Width)-15),float(int(viewport.Y+viewport.Height)-15),0.0f);
					Dist=D3DXVECTOR3(-12,0,0);
					C=YEL;
					break;
				}
				
				D3DMATERIAL8 mat;
				mat.Diffuse.a=((C&0xFF000000)>>24)/255.0f;
				mat.Diffuse.r=((C&0x00FF0000)>>16)/255.0f;
				mat.Diffuse.g=((C&0x0000FF00)>>8)/255.0f;
				mat.Diffuse.b=((C&0x000000FF))/255.0f;
				mat.Ambient=mat.Diffuse;
				mat.Emissive=mat.Diffuse;
				mat.Specular.a=1.0f;
				mat.Specular.r=0.5f;
				mat.Specular.g=0.5f;
				mat.Specular.b=0.5f;
				mat.Power=20.0f;
		

				for (DWORD j=0;j<this->MONDE->Cars[i]->vie;j++)
				{					
					D3DXVECTOR3 V=V0+float(j)*Dist;				
					
					m_pd3dDevice->SetMaterial( &mat );				
					m_pd3dDevice->SetTexture ( 0, NULL );
					
					D3DXMATRIX Mat;
					
					D3DXMatrixTranslation(&Mat,V.x,V.y,V.z);
					Mat=Mat*mw1;			
					
					m_pd3dDeviceUtility->SetTransform(D3DTS_WORLD, &Mat);
					Sphere->mesh->DrawSubset(0);					
				}			
			}
		}
		

		// Render game menu
        if (this->app_state!=APP_STATE_PLAYING  )
		{
		
			m_pd3dDeviceUtility->SetRenderState(D3DRS_LIGHTING,false);
			if (HyperPlane)
			{
				D3DXMATRIX mv,mw;
				D3DXVECTOR3 lookat1=D3DXVECTOR3(0,0,0);
				up1=D3DVEC(0,1,0);
				eye=D3DVEC(0,0,-1);


				D3DXMatrixLookAtLH( &mv, &eye,
					&lookat1,
					&up1);
				m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW, &mv );
				D3DXMatrixInverse( &m_matBillboard, NULL, &mv);
				m_matBillboard._41 = 0.0f;
				m_matBillboard._42 = 0.0f;
				m_matBillboard._43 = 0.0f;
				
				D3DXMatrixScaling(&mw,2,2,2);
				D3DXMatrixTranslation(&HyperPlane->matrice,0,0,0.5f);				
				HyperPlane->matrice=HyperPlane->matrice*mw;
				

				LPD3DXMESH cloned,temp;
				DWORD T=timeGetTime();
				
				WayPoints[0]->Advance();
				WayPoints[1]->Advance();
				WayPoints[2]->Advance();
				WayPoints[3]->Advance();
				
				D3DXVECTOR3 center[4];
				WayPoints[0]->GetVector(&center[0]);
				WayPoints[1]->GetVector(&center[1]);
				WayPoints[2]->GetVector(&center[2]);
				WayPoints[3]->GetVector(&center[3]);

				float MINDIST=~(center[2]-center[1])/2;
				if (~(center[3]-center[1])/2<MINDIST) MINDIST=~(center[3]-center[1])/2;
				if (~(center[3]-center[2])/2<MINDIST) MINDIST=~(center[3]-center[2])/2;

				MINDIST=MINDIST*MINDIST;
				
				float poids[4];
				poids[0]=35;
				poids[1]=35;
				poids[2]=35;
				poids[3]=35;


				if (SUCCEEDED(this->HyperPlane->mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM ,D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 ,m_pd3dDevice,&cloned)))
				{
					BYTE *VB;
					cloned->LockVertexBuffer(D3DLOCK_DISCARD,&VB);

					for(DWORD i=0;i<cloned->GetNumVertices();i++)
					{
						D3DXVECTOR3 * pos=(D3DXVECTOR3*) &VB[i*32];
						D3DVECTOR pos0=*pos;
						
						float dist[4];
						D3DXVECTOR3 V[4];
						float force[4];
						
						float FFF=float((1-exp((fabs(pos->y)-0.5f)*10 ))*(1-exp((fabs(pos->x)-0.5f)*10 )));
						DWORD j;
						for (j=1;j<4;j++)
						{
							V[j]=center[j]-*pos;
							dist[j]=V[j]*V[j];
							float f=2*(1-dist[j]/(0.25f*0.25f));
							if (f<=0.000001f)
								force[j]=0;
							else
								force[j]=									
									float((1-exp(-poids[j]*f*f*f*f)));
						}

						
						for (j=1;j<4;j++)
						{	
							V[j].z=0.45f;
							if (!((pos0.x<=-0.49f)||(pos0.y<=-0.49f)
								||(pos0.x>=0.49f)||(pos0.y>=0.49f)
								))
								
							*pos=*pos+force[j]*V[j];
						;
						}
						
						//center[0]=D3DVEC(0,0,0);
						dist[0]=~(pos0-center[0]);
						pos->z=pos->z+0.1f*(float)cos(10*dist[0]-T/200.0f);
					}
					cloned->UnlockVertexBuffer();
					temp=HyperPlane->mesh;
					HyperPlane->mesh=cloned;
					HyperPlane->Render(m_pd3dDevice,m_pd3dDeviceUtility);
					HyperPlane->mesh=temp;
					cloned->Release();
				}

			}
			
            g_pCurrentMenu->Render( m_pd3dDevice, m_pd3dDeviceUtility, m_pFont);
			m_pd3dDeviceUtility->SetRenderState(D3DRS_LIGHTING,true);
		}



        // Show frame rate
        
		m_pStatsFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
    

		
		if((app_substate==APP_SUBSTATE_INTROANIM) && (app_state==APP_STATE_PLAYING))
		{
			// Save current matrices
			D3DXMATRIX matViewSaved, matProjSaved;
			m_pd3dDeviceUtility->GetTransform( D3DTS_VIEW,       &matViewSaved );
			m_pd3dDeviceUtility->GetTransform( D3DTS_PROJECTION, &matProjSaved );

			// Setup new view and proj matrices for head-on viewing
			D3DXMATRIX matView;
			D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3(0.0f,0.0f,-30.0f),
										  &D3DXVECTOR3(0.0f,0.0f,0.0f),
				 						  &D3DXVECTOR3(0.0f,1.0f,0.0f) );
			m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW,       &matView );
			D3DXMatrixInverse( &m_matBillboard, NULL, &matView );
			m_matBillboard._41 = 0.0f;
			m_matBillboard._42 = 0.0f;
			m_matBillboard._43 = 0.0f;

			// Establish colors for selected vs. normal menu items
			D3DMATERIAL8 mtrlSelected;
			D3DUtil_InitMaterial( mtrlSelected, 1.0f, 0.0f, 0.0f, 1.0f );

			m_pd3dDeviceUtility->SetRenderState( D3DRS_LIGHTING, TRUE );
			m_pd3dDeviceUtility->SetRenderState( D3DRS_AMBIENT,  0xffffffff );

			D3DXMATRIX matWorld1;
			D3DXMatrixScaling( &matWorld1, 0.7f, 0.7f, 0.7f );

			D3DXMATRIX matRotate;
			D3DXMatrixRotationY( &matRotate, (D3DX_PI/3)*sinf(timeGetTime()/200.0f) );
			D3DXMatrixMultiply( &matWorld1, &matWorld1, &matRotate );
			m_pd3dDevice->SetMaterial( &mtrlSelected );
			// Translate the menuitem into place
			matWorld1._42 = 1.0f;
			m_pd3dDeviceUtility->SetTransform( D3DTS_WORLD, &matWorld1 );

			// Render the menuitem's label
			m_pFont->Render3DText( "READY ?", 
								 D3DFONT_CENTERED|D3DFONT_TWOSIDED );

			// Restore matrices
			m_pd3dDeviceUtility->SetTransform( D3DTS_VIEW,       &matViewSaved );
			D3DXMatrixInverse( &m_matBillboard, NULL, &matViewSaved );
			m_matBillboard._41 = 0.0f;
			m_matBillboard._42 = 0.0f;
			m_matBillboard._43 = 0.0f;
			m_pd3dDeviceUtility->SetTransform( D3DTS_PROJECTION, &matProjSaved );
		}

        	 
        // End the scene.
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // Restore the fonts
	m_pFont->InitDeviceObjects( m_pd3dDevice, m_pd3dDeviceUtility );
	m_pRankingFont->InitDeviceObjects(m_pd3dDevice, m_pd3dDeviceUtility);
	m_pStatsFont->InitDeviceObjects( m_pd3dDevice, m_pd3dDeviceUtility );
	
	Sphere->LoadFormFile("Meshes/Sphere.x",this->m_pd3dDevice);
	Sphere->ComputeFaceInfo();

	bgPLANE->LoadFormFile("Meshes/explosion.x",this->m_pd3dDevice);
	bgPLANE->ComputeFaceInfo();


	MONDE->UnloadAll();
		
	if (this->app_state==APP_STATE_PLAYING)
	{
		MONDE->ReloadAll(m_pd3dDevice);
	}


	this->CreateHyperPlanes("Meshes/HPLANE.X");

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	this->CreateHyperPlanes("Meshes/HPLANE.X");

    // Restore the fonts
	
	if (this->app_state==APP_STATE_PLAYING)
	{
		MONDE->ReloadAll(m_pd3dDevice);
	}

	if (Sphere->mesh!=NULL)
	{
		Sphere->UnLoad();
	}

	Sphere->LoadFormFile("Meshes/Sphere.x",this->m_pd3dDevice);
	Sphere->ComputeFaceInfo();
	

	if (bgPLANE->mesh!=NULL)
	{
		bgPLANE->UnLoad();
	}

	bgPLANE->LoadFormFile("Meshes/explosion.x",this->m_pd3dDevice);
	bgPLANE->ComputeFaceInfo();

	for (DWORD i=0;(i<this->nPlayers+this->nCPU)&&(i<this->MONDE->NumCars)
		 ;i++)
	{
		this->MONDE->Cars[i]->SetClavier(this->Touches[i].UP,
										 this->Touches[i].DOWN,
										 this->Touches[i].LEFT,
										 this->Touches[i].RIGHT);
	}


	MONDE->ComputeAllFaceInfo();
	if (this->MONDE->NumCars>0)
	{
		MONDE->ComputeAllFaceInfo2(3*MONDE->Cars[0]->Mesh->TransformedRadius);
		MONDE->ComputeBaliseInfo(PI/4.0f,6*MONDE->Cars[0]->Mesh->TransformedRadius);
	}
	else
	{
		MONDE->ComputeBaliseInfo(PI/4.0f,1.0);
	}


	MONDE->CreateLights(m_pd3dDevice,m_pd3dDeviceUtility);

    m_pFont->RestoreDeviceObjects();
	m_pRankingFont->RestoreDeviceObjects();
    m_pStatsFont->RestoreDeviceObjects();

    // Restore the textures
    m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDeviceUtility->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDeviceUtility->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDeviceUtility->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
    m_pd3dDeviceUtility->SetRenderState( D3DRS_AMBIENT,  0x00808080 );

    // Set the transform matrices
    D3DXMATRIX  matProj;

    FLOAT fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );

    m_pd3dDeviceUtility->SetTransform( D3DTS_PROJECTION, &matProj );

	OnCreateDevice(m_hWnd);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    m_pFont->InvalidateDeviceObjects();
	m_pRankingFont->InvalidateDeviceObjects();
    m_pStatsFont->InvalidateDeviceObjects();

	if (Sphere->mesh!=NULL)
	{
		Sphere->UnLoad();
	}

	if (bgPLANE->mesh!=NULL)
	{
		bgPLANE->UnLoad();
	}

	if (this->HyperPlane)
	{
		this->HyperPlane->~MeshStruct();this->HyperPlane=NULL;
	}
	MONDE->UnloadAll();	

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    m_pFont->DeleteDeviceObjects();
	m_pRankingFont->DeleteDeviceObjects();
    m_pStatsFont->DeleteDeviceObjects();

	if (Sphere->mesh!=NULL)
	{
		Sphere->mesh->Release();
		Sphere->mesh=NULL;
		Sphere->loaded=false;
	}

	if (bgPLANE->mesh!=NULL)
	{
		bgPLANE->mesh->Release();
		bgPLANE->mesh=NULL;
		bgPLANE->loaded=false;
	}

	FreeDirectInput();
	g_pDI = NULL;
	g_pKeyboard = NULL;

	MONDE->UnloadAll();

	if (this->HyperPlane)
	{
		this->HyperPlane->~MeshStruct();this->HyperPlane=NULL;
	}


//	this->menu->~MENU();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    DestroySoundObjects();
	DeleteObject(credits_hbmp);

    SAFE_DELETE( m_pFont );
	SAFE_DELETE( m_pRankingFont );
    SAFE_DELETE( m_pStatsFont );
	
	this->MONDE->~MeshManager();
	this->Sphere->~MeshStruct();
	this->bgPLANE->~MeshStruct();

	FILE*f=fopen("keyboard.cfg","wb");
	if(f!=NULL)
	{
		fwrite(&this->Touches[0],sizeof(this->Touches),1,f);
		fclose(f);
	}

	this->computers[0]->~CCOMPUTER();
	this->computers[1]->~CCOMPUTER();
	this->computers[2]->~CCOMPUTER();

	for (DWORD k=0;k<4;k++)
	{
		this->WayPoints[k]->~CWAYPOINT();
	}


    SAFE_DELETE( g_pQuitMenu );
    SAFE_DELETE( g_pMainMenu );
	SAFE_DELETE( g_pStageMenu  );
	SAFE_DELETE( g_pQuitGameMenu );
	SAFE_DELETE( g_pIntelligenceMenu );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: OnCreateDevice()
// Desc: Setups a the keyboard device using the flags from the dialog.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OnCreateDevice( HWND hDlg )
{
    HRESULT hr;
    DWORD   dwCoopFlags;
#ifdef _WIN64
    HINSTANCE hInst = (HINSTANCE) GetWindowLongPtr( hDlg, GWLP_HINSTANCE );
#else
    HINSTANCE hInst = (HINSTANCE) GetWindowLong( hDlg, GWL_HINSTANCE );
#endif

    // Cleanup any previous call first
    KillTimer( hDlg, 0 );    
    FreeDirectInput();

        dwCoopFlags = DISCL_EXCLUSIVE;

        dwCoopFlags |= DISCL_FOREGROUND;

    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&g_pDI, NULL ) ) )
        return hr;
    
    // Obtain an interface to the system keyboard device.
    if( FAILED( hr = g_pDI->CreateDevice( GUID_SysKeyboard, &g_pKeyboard, NULL ) ) )
        return hr;
    
    if( FAILED( hr = g_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
        return hr;
    
    // DirectInput applications.
    hr = g_pKeyboard->SetCooperativeLevel( hDlg, dwCoopFlags );


    if( FAILED(hr) )
        return hr;

    // Acquire the newly created device
    g_pKeyboard->Acquire();

    SetTimer( hDlg, 0, 1000 / 30, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ReadImmediateData()
// Desc: Read the input device's state when in immediate mode and display it.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ReadImmediateData()
{
    HRESULT hr;
 
    if( NULL == g_pKeyboard ) 
        return S_OK;
    
    // Get the input's device state, and put the state in dims
    ZeroMemory( &diks, sizeof(diks) );
    hr = g_pKeyboard->GetDeviceState( sizeof(diks), &diks );
    if( FAILED(hr) ) 
    {
        hr = g_pKeyboard->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = g_pKeyboard->Acquire();

        return S_OK; 
    }
    
 
   
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FreeDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::FreeDirectInput()
{
    if( g_pKeyboard ) 
        g_pKeyboard->Unacquire();

    // Release any DirectInput objects.
    SAFE_RELEASE( g_pKeyboard );
    SAFE_RELEASE( g_pDI );
}

HWND hWndDlg_About;
int AboutDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg)
    {
        case WM_INITDIALOG:
           return TRUE;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
               case IDOK:EndDialog( hDlg, TRUE );
                    return TRUE;
            }
            break;

        case WM_DESTROY:
            hWndDlg_About = NULL;
            break;
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{	
	if( uMsg==WM_TIMER)
	{
		ReadImmediateData();

		if((diks[0x01] & 0x80)!=0x80)
			EscPressed=false;

		if(diks[0x01] & 0x80)
			if(this->app_state==APP_STATE_PLAYING)
				if(!EscPressed)
				{
					PlaySound( g_pSphereExplodeSound );
					EscPressed=true;
					g_pGlobalSaveMenu=g_pCurrentMenu;
					g_pCurrentMenu=g_pQuitGameMenu;
					this->app_state=APP_STATE_QUITGAMEMENU;
				}

		if(diks[0x3e] &0x80)
			if(diks[0x38] &0x80)
		        SendMessage( hWnd, WM_CLOSE, 0, 0 );

		if(diks[0x1c] & 0x80)
			if(diks[0x38] & 0x80)
				if( m_bActive && m_bReady)
                    {
						EnterPressed=true;
                        if( FAILED( ToggleFullscreen() ) )
                        {
                            DisplayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
                            return 0;
                        }
                    }
	}


	// WM_MOUSEMOVE
	if (uMsg==WM_MOUSEWHEEL)
	{
		if ((this->app_state!=APP_STATE_PLAYING)
			&&
			(!ReadingEntry)
			)

		{
			short i=(short)HIWORD(wParam);
			i=i/120;
			if (i<0)
			{
				if(this->g_pCurrentMenu->dwSelectedMenu<g_pCurrentMenu->dwNumChildren-1)
				{
					this->g_pCurrentMenu->dwSelectedMenu++;
					PlaySound( g_pSphereExplodeSound );
				}
			}
			else if (i>0)
			{
				
				if (this->g_pCurrentMenu->dwSelectedMenu>0)
				{
					PlaySound( g_pSphereExplodeSound );
					this->g_pCurrentMenu->dwSelectedMenu--;
				}
			}

			return 0;
		}
	}

	if (uMsg==WM_LBUTTONUP)
	{
		if (this->app_state!=APP_STATE_PLAYING)
		{
			diks[0x1c]=0x80;
			diks[0x1]=0;
			this->UpdateMenus();
			return 0;
		}
	}
	
	if ((uMsg==WM_RBUTTONUP)
		&&
		(!ReadingEntry)
		)
	{
		if (this->app_state!=APP_STATE_PLAYING)
		{
			diks[0x1]=0x80;
			diks[0x1c]=0;
			this->UpdateMenus();
			return 0;
		}
	}


    if( WM_COMMAND == uMsg )
    {
		switch( LOWORD(wParam) )
        {
 
			case ID_ABOUT_ABOUTMICROMACHINES3D:
			{
				HINSTANCE hInst;
				#ifdef _WIN64
				hInst = (HINSTANCE) GetWindowLongPtr( m_hWnd, GWLP_HINSTANCE );
				#else
				hInst = (HINSTANCE) GetWindowLong( m_hWnd, GWL_HINSTANCE );
				#endif
				DialogBox(hInst, (LPCTSTR)IDD_DIALOG1, hWnd, (DLGPROC)AboutDlgProc);
			}
        }
    }

    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}