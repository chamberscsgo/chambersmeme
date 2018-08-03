#pragma once
struct SSettings
{
	struct {
		std::string base [ 4 ] = { "World", "View", "LBY", "Closest" };
		std::string aimbottype [ 4 ] = { "Off", "Nospread", "Spread" };
		std::string reals [ 6 ] = { "Off", "Static", "Jitter", "Spin", "180z", "Side" };
		std::string lagtype [ 4 ] = { "Static", "Break", "Switch", "Test" };
		std::string chams [ 3 ] = { "Off", "Vis", "Invis" };
		std::string fakes [ 6 ] = { "Off", "Static", "Jitter", "Spin", "180z", "Side" };
		std::string pitches [ 5 ] = { "Off", "Down", "Up", "Fake Up", "Fake Down" };
		std::string infos [ 6 ] = { "Off", "Box", "Info", "Both" };
		std::string resolvers [ 6 ] = { "Off", "Primary", "Secondary", "Tertiary" };
		std::string bones [ 4 ] = { "Off", "Entity", "Last Rec", "All Recs" };
	}opts;

	struct {
		bool bSendPacket;
	}nonmenu;
	struct {
		bool enabled;
		int real; //off, manual, manual jitter, back, 180z, freestanding
		int fake; //off, rmanual, rmanual jitter, back, 180z, freestanding
		int pitch; //off, down, up, fakeup, fakedown

		int fakeoff;
		int realoff;

		bool adaptive_lag;
		bool doFakelag;
		int choke = 13;
		int fakelagType;
	}aa;
	struct {
		bool enabled;
		bool headonly;
		bool animfix;
		int resolver;
	}aimbot;
	struct {
		bool glow;
		int esp;
		int skeleton;
	}visuals;
	struct {
		bool bhop;
		bool anti_ut;
		bool cstrafe;
		int btcode;
		bool autostrafe;
		int tpcode;
		bool thirdperson;
		int config;
	}misc;

}; extern SSettings g_Settings;