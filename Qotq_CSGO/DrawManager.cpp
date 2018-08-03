#include "DrawManager.h"


void DrawManager::String ( int font , bool Center , int x , int y , Color c , const char *fmt , ... )
{
	int apple = 0;
	char Buffer [ 2048 ] = { '\0' };
	va_list Args;
	va_start ( Args , fmt );
	vsprintf_s ( Buffer , fmt , Args );
	va_end ( Args );
	size_t Size = strlen ( Buffer ) + 1;
	wchar_t* WideBuffer = new wchar_t [ Size ];
	mbstowcs_s ( 0 , WideBuffer , Size , Buffer , Size - 1 );
	int Width = 0 , Height = 0;
	if ( Center )
	{
		g_pVGuiSurface->GetTextSize ( font , WideBuffer , Width , Height );
	}
	g_pVGuiSurface->DrawSetTextColor ( c );
	g_pVGuiSurface->DrawSetTextFont ( font );
	g_pVGuiSurface->DrawSetTextPos ( x - ( Width / 2 ) , y );
	g_pVGuiSurface->DrawPrintText ( WideBuffer , wcslen ( WideBuffer ) );

	return;
}
void DrawManager::StringRight ( bool right , int font , int x , int y , Color c , const char *fmt , ... )
{
	int apple = 0;
	char Buffer [ 2048 ] = { '\0' };
	va_list Args;
	va_start ( Args , fmt );
	vsprintf_s ( Buffer , fmt , Args );
	va_end ( Args );
	size_t Size = strlen ( Buffer ) + 1;
	wchar_t* WideBuffer = new wchar_t [ Size ];
	mbstowcs_s ( 0 , WideBuffer , Size , Buffer , Size - 1 );
	int Width = 0 , Height = 0;
	if ( right )
	{
		g_pVGuiSurface->GetTextSize ( font , WideBuffer , Width , Height );
		x -= Width;
	}
	g_pVGuiSurface->DrawSetTextColor ( c );
	g_pVGuiSurface->DrawSetTextFont ( font );
	g_pVGuiSurface->DrawSetTextPos ( x , y );
	g_pVGuiSurface->DrawPrintText ( WideBuffer , wcslen ( WideBuffer ) );

	return;
}

void DrawManager::OutlinedRect ( int x , int y , int w , int h , Color &c ) {
	g_pVGuiSurface->DrawSetColor ( c );
	g_pVGuiSurface->DrawOutlinedRect ( x , y , x + w , y + h );
}
void DrawManager::FilledRect ( int x , int y , int w , int h , Color &c ) {
	g_pVGuiSurface->DrawSetColor ( c );
	g_pVGuiSurface->DrawFilledRect ( x , y , x + w , y + h );
}
void DrawManager::Line ( int x , int y , int x1 , int y2 , Color c ) {
	g_pVGuiSurface->DrawSetColor ( c );
	g_pVGuiSurface->DrawLine ( x , y , x1 , y2 );
}
void DrawManager::OutlinedCircle ( int x , int y , int r , int seg , Color c ) {
	g_pVGuiSurface->DrawSetColor ( c );
	g_pVGuiSurface->DrawOutlinedCircle ( x , y , r , seg );
}
RECT GetTextSize ( DWORD font , const char* text ) {
	size_t origsize = strlen ( text ) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring [ newsize ];
	mbstowcs_s ( &convertedChars , wcstring , origsize , text , _TRUNCATE );
	RECT rect; int x , y;
	g_pVGuiSurface->GetTextSize ( font , wcstring , x , y );
	rect.left = x; rect.bottom = y;
	rect.right = x;
	return rect;
}
