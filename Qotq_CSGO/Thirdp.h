#pragma once
#include "SDK.h"
class CThirdperson : public singleton<CThirdperson>
{
public:
	void			Run ( CViewSetup * setup );
	QAngle			m_angle = QAngle ( 0 , 0 , 0 );
private:
};