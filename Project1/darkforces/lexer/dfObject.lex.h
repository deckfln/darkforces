#include <string>
#include <map>

#include "../../gaEngine/Lexer.h"
#include "../../gaEngine/Parser.h"

enum {
	O_KW = 1024,
	O_LEVELNAME,
	O_PODS,
	O_POD,
	O_SPRS,
	O_SPR,
	O_FMES,	//1030
	O_FME,
	O_SOUNDS,
	O_OBJECTS,	// 1033
	O_CLASS,
	O_SPIRIT,
	O_DATA,
	O_X,
	O_Y,
	O_Z,
	O_PCH,	//1040
	O_YAW,
	O_ROL,
	O_DIFF,
	O_SEQ,		//1044
	O_SEQEND,	//1045
	O_LOGIC,	//1046
	O_EYE,
	O_TRUE,
	O_FALSE,
	O_TYPE,	//1050
	I_OFFICER,
	O_UPDATE,
	O_FLAGS,
	O_D_YAW,
	O_COMMANDO,	//1055
	O_PLANS,
	O_ITEM,
	O_SHIELD,
	O_ANIM,
	O_ENERGY, //1060
	O_TROOP,
	O_MOUSEBOT,
	O_SCENERY,
	O_HEIGHT,
	O_PAUSE,	//1065
	O_VUE,
	O_GOGGLES,
	O_RIFLE,
	O_LIFE,
	O_BATTERY, //1070
	O_SUPERCHARGE,
	O_MEDKIT,
	O_STORM1,
	O_INT_DROID,
	O_FRAME,	//1075
	O_SPRITE,
	O_SAFE,
	O_RADIUS,
	O_PLAYER,
	O_3D, //1080
	O_OFFICERR,
	O_KEY,
	O_REVIVE
};

std::map<std::string, uint32_t> g_keywords =
{
	{"O", O_KW},
	{"LEVELNAME", O_LEVELNAME},
	{"PODS", O_PODS},
	{"POD", O_POD},
	{"SPRS", O_SPRS},
	{"SPR", O_SPR},
	{"FMES", O_FMES},
	{"FME", O_FME},
	{"SOUNDS", O_SOUNDS},
	{"OBJECTS", O_OBJECTS},
	{"CLASS", O_CLASS},
	{"SPIRIT", O_SPIRIT},
	{"DATA", O_DATA},
	{"X", O_X},
	{"Y", O_Y},
	{"Z", O_Z},
	{"PCH", O_PCH},
	{"YAW", O_YAW},
	{"ROL", O_ROL},
	{"DIFF", O_DIFF},
	{"SEQ", O_SEQ},
	{"SEQEND", O_SEQEND},
	{"LOGIC", O_LOGIC},
	{"EYE", O_EYE},
	{"TRUE", O_TRUE},
	{"FALSE", O_FALSE},
	{"TYPE", O_TYPE},
	{"I_OFFICER", I_OFFICER},
	{"I_OFFICERR", O_OFFICERR},
	{"UPDATE", O_UPDATE},
	{"FLAGS", O_FLAGS},
	{"D_YAW", O_D_YAW},
	{"COMMANDO", O_COMMANDO},
	{"PLANS", O_PLANS},
	{"ITEM", O_ITEM},
	{"SHIELD", O_SHIELD},
	{"ANIM", O_ANIM},
	{"ENERGY", O_ENERGY},
	{"TROOP", O_TROOP},
	{"MOUSEBOT", O_MOUSEBOT},
	{"SCENERY", O_SCENERY},
	{"HEIGHT", O_HEIGHT},
	{"PAUSE", O_PAUSE},
	{"VUE", O_VUE},
	{"GOGGLES", O_GOGGLES},
	{"RIFLE", O_RIFLE},
	{"LIFE", O_LIFE},
	{"BATTERY", O_BATTERY},
	{"SUPERCHARGE", O_SUPERCHARGE},
	{"MEDKIT", O_MEDKIT},
	{"STORM1", O_STORM1},
	{"INT_DROID", O_INT_DROID},
	{"FRAME", O_FRAME},
	{"SPRITE", O_SPRITE},
	{"SAFE", O_SAFE},
	{"RADIUS", O_RADIUS},
	{"PLAYER", O_PLAYER },
	{"3D", O_3D},
	{"KEY", O_KEY},
	{"REVIVE", O_REVIVE}
};


enum {
	E_MAGIC = 4096,
	E_LEVELNAME,
	E_PODS,				
	E_PODSEQ,	// 4099
	E_POD,		// 4100
	E_SPRS,		// 4101
	E_SPRSEQ,
	E_SPR,
	E_FMES,		//4104
	E_FMESEQ,	//4105
	E_FME,
	E_SOUNDS,	//4107
	E_OBJECTS,	//4108
	E_CLASS,		//4109
	E_CLASSTYPE,	//4110
	E_DATA,
	E_X,
	E_Y,
	E_Z,
	E_PCH,			//4115
	E_YAW,
	E_ROL,
	E_DIFF,
	E_CLASSHEADER,
	E_CLASSBODY,	//4120
	E_COMPONENT,	//4121
	E_CLASSFULL,	//4122
	E_LOGIC,		//4123
	E_LOGICTYPE,	//4124
	E_ITEM,
	E_EYE,			//4126
	E_TRUEFALSE,	//4127
	E_TYPE,
	E_TYPETYPE,		
	E_FLAGS,		//4130
	E_D_YAW,
	E_RADIUS,
	E_HEIGHT,
	E_PAUSE,
	E_VUE,			//4135
	E_ITEMTYPE,
	E_SHIELD,
	E_ENERGY,
	E_STRING,
	E_CLASSSEQ,		//4140
	E_CLASSATTR,	//4141
	E_POSITION,
	E_ROTATION
};

std::vector <struct GameEngine::ParserRule> g_dfObjectParse = {
{E_MAGIC,		GameEngine::PARSER_AND, {O_KW, GameEngine::DIGIT}},
{E_LEVELNAME,	GameEngine::PARSER_AND, {O_LEVELNAME, GameEngine::STRING}},
{E_PODS,		GameEngine::PARSER_AND, {O_PODS, GameEngine::DIGIT, E_PODSEQ}},
{E_PODSEQ,		GameEngine::PARSER_LIST,{E_POD}},
{E_POD,			GameEngine::PARSER_AND, {O_POD, GameEngine::COLON, GameEngine::STRING}},
{E_SPRS,		GameEngine::PARSER_AND, {O_SPRS, GameEngine::DIGIT, E_SPRSEQ}},
{E_SPRSEQ,		GameEngine::PARSER_LIST,{E_SPR}},
{E_SPR,			GameEngine::PARSER_AND, {O_SPR, GameEngine::COLON, GameEngine::STRING}},
{E_FMES,		GameEngine::PARSER_AND, {O_FMES, GameEngine::DIGIT, E_FMESEQ}},
{E_FMESEQ,		GameEngine::PARSER_LIST,{E_FME}},
{E_FME,			GameEngine::PARSER_AND, {O_FME, GameEngine::COLON, GameEngine::STRING}},
{E_SOUNDS,		GameEngine::PARSER_AND, {O_SOUNDS, GameEngine::DIGIT}},
{E_OBJECTS,		GameEngine::PARSER_AND, {O_OBJECTS, GameEngine::DIGIT, E_CLASSSEQ}},
{E_CLASS,		GameEngine::PARSER_AND, {O_CLASS, GameEngine::COLON, E_CLASSTYPE}},
{E_CLASSTYPE,	GameEngine::PARSER_OR,	{O_SPIRIT, O_SPRITE, O_FRAME, O_3D }},
{E_DATA,		GameEngine::PARSER_AND, {O_DATA, GameEngine::COLON, GameEngine::DIGIT}},
{E_X,			GameEngine::PARSER_AND, {O_X, GameEngine::COLON, GameEngine::DIGIT}},
{E_Y,			GameEngine::PARSER_AND, {O_Y, GameEngine::COLON, GameEngine::DIGIT}},
{E_Z,			GameEngine::PARSER_AND, {O_Z, GameEngine::COLON, GameEngine::DIGIT}},
{E_YAW,			GameEngine::PARSER_AND, {O_YAW, GameEngine::COLON, GameEngine::DIGIT}},
{E_ROL,			GameEngine::PARSER_AND, {O_ROL, GameEngine::COLON, GameEngine::DIGIT}},
{E_PCH,			GameEngine::PARSER_AND, {O_PCH, GameEngine::COLON, GameEngine::DIGIT}},
{E_DIFF,		GameEngine::PARSER_AND, {O_DIFF, GameEngine::COLON, GameEngine::DIGIT}},
{E_POSITION,	GameEngine::PARSER_AND, {E_X, E_Y, E_Z}},
{E_ROTATION,	GameEngine::PARSER_AND, {E_PCH, E_YAW, E_ROL}},
{E_CLASSATTR,	GameEngine::PARSER_AND, {E_DATA, E_POSITION, E_ROTATION, E_DIFF}},
{E_CLASSBODY,	GameEngine::PARSER_LIST,{E_COMPONENT}},
{E_COMPONENT,	GameEngine::PARSER_OR,	{E_LOGIC, E_EYE, E_TYPE, E_FLAGS, E_D_YAW, E_RADIUS, E_HEIGHT, E_PAUSE, E_VUE}},
{E_LOGIC,		GameEngine::PARSER_AND, {O_LOGIC, GameEngine::COLON, E_LOGICTYPE}},
{E_LOGICTYPE,	GameEngine::PARSER_OR,	{O_PLAYER, O_UPDATE, O_PLANS, E_ITEM, O_ANIM, O_SCENERY, O_BATTERY, O_STORM1, I_OFFICER, O_OFFICERR, O_MOUSEBOT, O_INT_DROID, O_MEDKIT, O_COMMANDO, O_SHIELD, O_TROOP, O_SUPERCHARGE, O_LIFE, O_KEY, O_GOGGLES, O_RIFLE, O_REVIVE}},
{E_ITEM,		GameEngine::PARSER_OR,	{E_SHIELD, E_ENERGY}},
{E_SHIELD,		GameEngine::PARSER_AND,	{O_ITEM, O_SHIELD}},
{E_ENERGY,		GameEngine::PARSER_AND,	{O_ITEM, O_ENERGY}},
{E_EYE,			GameEngine::PARSER_AND, {O_EYE, GameEngine::COLON, E_TRUEFALSE}},
{E_TRUEFALSE,	GameEngine::PARSER_OR,	{O_TRUE, O_FALSE}},
{E_TYPE,		GameEngine::PARSER_AND, {O_TYPE, GameEngine::COLON, E_LOGICTYPE}},
{E_FLAGS,		GameEngine::PARSER_AND, {O_FLAGS, GameEngine::COLON, GameEngine::DIGIT}},
{E_D_YAW,		GameEngine::PARSER_AND, {O_D_YAW, GameEngine::COLON, GameEngine::DIGIT}},
{E_RADIUS,		GameEngine::PARSER_AND, {O_RADIUS, GameEngine::COLON, GameEngine::DIGIT}},
{E_HEIGHT,		GameEngine::PARSER_AND, {O_HEIGHT, GameEngine::COLON, GameEngine::DIGIT}},
{E_PAUSE,		GameEngine::PARSER_AND, {O_PAUSE, GameEngine::COLON, E_TRUEFALSE}},
{E_STRING,		GameEngine::PARSER_AND, {GameEngine::QUOTE, GameEngine::STRING, GameEngine::QUOTE}},
{E_VUE,			GameEngine::PARSER_AND, {O_VUE, GameEngine::COLON, GameEngine::STRING, E_STRING}},
{E_CLASSFULL,	GameEngine::PARSER_AND, {E_CLASS, E_CLASSATTR, O_SEQ, E_CLASSBODY, O_SEQEND}},
{E_CLASSFULL,	GameEngine::PARSER_AND, {O_CLASS, GameEngine::COLON, O_SAFE, E_CLASSATTR}},
{E_CLASSSEQ,	GameEngine::PARSER_LIST,{E_CLASSFULL}},
};
