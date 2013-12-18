#ifndef __GLOBALS_H__
#define __GLOBALS_H__


#define nosave static
#define protected static


#define PROTO_DELIMITER "|" // 协议中字段之间的分隔符


// paths
#define CMD_USER_PATH       "/cmds/user/"
#define COMMON_SAVE_PATH    "/data/"
#define USER_SAVE_PATH      "/data/user/"


//User IDs
#define ROOT_UID        "Root"
#define BACKBONE_UID    "Backbone"

// singletons
#define CMD_D			"/singleton/cmdd"
#define LOGIN_D			"/singleton/logind"
#define BLESS_D         "/singleton/blessd"
#define FLOWER_D        "/singleton/flowerd"

// features
#define F_DBASE			"/feature/dbase"
#define F_SAVE          "/feature/save"

// objects
#define USER_OB			"/object/user"
#define BLESS_OB        "/object/bless"


#endif

