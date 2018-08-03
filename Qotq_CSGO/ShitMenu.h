#pragma once
#include "SDK.h"

class ShitMenu
{
public:
	void DrawShit ( );
	void HandleInput ( );
private:
	int indent = 400;
	int cur;
	int autonum;

	void DoInt ( int &val , int incr , int min , int max , int pos );
	void DoBool ( bool &val , int pos );
	void DoConfig ( int pos );

	void DrawLine ( std::string str , bool value );
	void DrawLine ( std::string str , int value );
	void DrawLine ( std::string str , int value , std::string *opts , int numopts );

	void DrawConfig ( std::string str );
}; extern ShitMenu qualitymenu;