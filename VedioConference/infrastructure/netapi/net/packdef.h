#pragma once

// C标准库内存操作
#include<memory.h>

// ============= 协议通用配置 =============
// 网络缓冲区大小(字节)
#define _DEF_BUFFER         (4096)

// 内容区域大小(字节)
#define _DEF_CONTENT_SIZE	(1024)

// 字符串字段最大长度
#define _MAX_SIZE           (60)

// 文件路径最大长度(Windows标准)
#define _MAX_PATH           (260)

// ============= 服务器配置 =============
// 服务器控制信令端口
#define _DEF_PORT       8000

// 服务器IP地址(开发环境)
#define _DEF_SERVERIP   "192.168.61.92"

// ============= 协议包类型定义 =============
// 协议包基础偏移量
// 所有自定义协议包类型以此为基准，+0、+1、+2...
#define _DEF_PACK_BASE	(10000)

// 协议包总数量(用于包类型范围检查)
#define _DEF_PACK_COUNT (100)

// 注册请求/响应
#define _DEF_PACK_REGISTER_RQ	(_DEF_PACK_BASE + 0 )  // 注册请求
#define _DEF_PACK_REGISTER_RS	(_DEF_PACK_BASE + 1 )  // 注册响应

// 登录请求/响应
#define _DEF_PACK_LOGIN_RQ      (_DEF_PACK_BASE + 2 )  // 登录请求
#define _DEF_PACK_LOGIN_RS      (_DEF_PACK_BASE + 3 )  // 登录响应

// 会议房间请求/响应
#define _DEF_CREATEROOM_RQ      (_DEF_PACK_BASE + 4)  // 创建房间请求
#define _DEF_CREATEROOM_RS      (_DEF_PACK_BASE + 5)  // 创建房间响应
#define _DEF_JOINROOM_RQ        (_DEF_PACK_BASE + 6)  // 加入房间请求
#define _DEF_JOINROOM_RS        (_DEF_PACK_BASE + 7)  // 加入房间响应

// 会议成员管理
#define _DEF_PACK_ROOM_MEMBER   (_DEF_PACK_BASE + 8)  // 房间成员变动通知
#define _DEF_PACK_LEAVEROOM_RQ  (_DEF_PACK_BASE + 9)  // 离开房间请求

// 音视频数据传输
#define _DEF_PACK_AUDIO_FRAME   (_DEF_PACK_BASE + 10) // 音频帧数据
#define _DEF_PACK_VEDIO_FRAME   (_DEF_PACK_BASE + 11) // 视频帧数据

// 音视频通道注册
#define _DEF_PACK_AUDIO_REGISTER    (_DEF_PACK_BASE + 12) // 音频通道注册
#define _DEF_PACK_VIDEO_REGISTER    (_DEF_PACK_BASE + 13) // 视频通道注册

// ============= 协议结果码定义 =============
// 注册结果码
#define tel_is_exist		(0)  // 手机号已注册
#define register_success	(1)  // 注册成功
#define name_is_exist       (2)  // 昵称已被占用

// 登录结果码
#define user_not_exist		(0)  // 用户不存在
#define password_error		(1)  // 密码错误
#define login_success		(2)  // 登录成功

// 创建房间结果码
#define create_success      (0)  // 创建成功

// 加入房间结果码
#define room_isnot_exist    (0)  // 房间不存在
#define join_success        (1)  // 加入成功

// ============= 协议包类型别名 =============
// PackType: 所有协议包的第一个字段类型
typedef int PackType;

// ============= 协议结构体定义 =============

// STRU_REGISTER_RQ - 用户注册请求
// 发送方: 客户端
// 接收方: 服务器
// 说明: 用户注册新账号
typedef struct STRU_REGISTER_RQ
{
	STRU_REGISTER_RQ():type(_DEF_PACK_REGISTER_RQ)
	{
		memset( tel  , 0, sizeof(tel));
		memset( name  , 0, sizeof(name));
		memset( password , 0, sizeof(password) );
	}
	PackType type;           // 包类型: _DEF_PACK_REGISTER_RQ
	char tel[_MAX_SIZE];     // 手机号/账号
	char name[_MAX_SIZE];    // 用户昵称
	char password[_MAX_SIZE];// 密码(MD5加密后传输)

}STRU_REGISTER_RQ;

// STRU_REGISTER_RS - 用户注册响应
// 发送方: 服务器
// 接收方: 客户端
typedef struct STRU_REGISTER_RS
{
	STRU_REGISTER_RS(): type(_DEF_PACK_REGISTER_RS) , result(register_success) { }
	PackType type;           // 包类型: _DEF_PACK_REGISTER_RS
	int result;              // 结果码: tel_is_exist/register_success/name_is_exist

}STRU_REGISTER_RS;

// STRU_LOGIN_RQ - 用户登录请求
typedef struct STRU_LOGIN_RQ
{
	STRU_LOGIN_RQ():type(_DEF_PACK_LOGIN_RQ)
	{
		memset( tel , 0, sizeof(tel) );
		memset( password , 0, sizeof(password) );
	}
	PackType type;           // 包类型: _DEF_PACK_LOGIN_RQ
	char tel[_MAX_SIZE];     // 手机号/账号
	char password[_MAX_SIZE];// 密码(MD5加密后传输)

}STRU_LOGIN_RQ;

// STRU_LOGIN_RS - 用户登录响应
typedef struct STRU_LOGIN_RS
{
	STRU_LOGIN_RS(): type(_DEF_PACK_LOGIN_RS) , result(login_success),userid(0)
	{
        memset(m_name,0,sizeof(m_name));
    }
	PackType type;           // 包类型: _DEF_PACK_LOGIN_RS
	int result;              // 结果码: user_not_exist/password_error/login_success
	int userid;              // 用户ID(登录成功时服务器分配)
    char m_name[_MAX_SIZE];  // 用户昵称(登录成功时返回)

}STRU_LOGIN_RS;

// STRU_CREATEROOM_RQ - 创建会议请求
typedef struct STRU_CREATEROOM_RQ{
    STRU_CREATEROOM_RQ():type(_DEF_CREATEROOM_RQ),m_userId(0){ }
    PackType type;           // 包类型: _DEF_CREATEROOM_RQ
    int m_userId;           // 创建者用户ID

}STRU_CREATEROOM_RQ;

// STRU_CREATEROOM_RS - 创建会议响应
typedef struct STRU_CREATEROOM_RS{
    STRU_CREATEROOM_RS():type(_DEF_CREATEROOM_RS),m_result(create_success),m_RoomId(0){ }
    PackType type;           // 包类型: _DEF_CREATEROOM_RS
    int m_result;            // 结果码: create_success
    int m_RoomId;            // 房间ID(创建成功时分配)

}STRU_CREATEROOM_RS;

// STRU_JOINROOM_RQ - 加入会议请求
typedef struct STRU_JOINROOM_RQ{
    STRU_JOINROOM_RQ():m_nType(_DEF_JOINROOM_RQ),m_UserId(0),m_RoomId(0){ }
    PackType m_nType;        // 包类型: _DEF_JOINROOM_RQ
    int m_UserId;            // 加入者用户ID
    int m_RoomId;            // 要加入的房间号

}STRU_JOINROOM_RQ;

// STRU_JOINROOM_RS - 加入会议响应
typedef struct STRU_JOINROOM_RS{
    STRU_JOINROOM_RS():m_nType(_DEF_JOINROOM_RS),m_lResult(0),m_roomId(0){ }
    PackType m_nType;        // 包类型: _DEF_JOINROOM_RS
    int m_lResult;           // 结果码: room_isnot_exist/join_success
    int m_roomId;            // 房间号

}STRU_JOINROOM_RS;

// STRU_ROOM_MEMBER_RQ - 房间成员变动通知
// 服务器主动推送给房间内所有成员
typedef struct STRU_ROOM_MEMBER_RQ{
    STRU_ROOM_MEMBER_RQ(){
        m_nType=_DEF_PACK_ROOM_MEMBER;
        m_userId=0;
        memset(m_userName,0,sizeof(m_userName));
    }
    PackType m_nType;        // 包类型: _DEF_PACK_ROOM_MEMBER
    int m_userId;            // 成员用户ID
    char m_userName[_MAX_SIZE]; // 成员昵称

}STRU_ROOM_MEMBER_RQ;

// STRU_LEAVEROOM_RQ - 离开会议请求
typedef struct STRU_LEAVEROOM_RQ{
    STRU_LEAVEROOM_RQ(){
        m_nType=_DEF_PACK_LEAVEROOM_RQ;
        m_nUserId=0;
        RoomId=0;
        memset(UserName,0,sizeof(UserName));
    }
    PackType m_nType;        // 包类型: _DEF_PACK_LEAVEROOM_RQ
    int m_nUserId;           // 离开者用户ID
    int RoomId;              // 所在房间号
    char UserName[_MAX_SIZE];// 离开者昵称

}STRU_LEAVEROOM_RQ;

// 音视频数据帧格式说明(通用)：
// 成员: type(4B) + userId(4B) + roomId(4B) + min(4B) + sec(4B) + msec(4B) + 数据
// 用于音视频同步和路由

// STRU_AUDIO_REGISTER - 音频通道注册
typedef struct STRU_AUDIO_REGISTER{
    STRU_AUDIO_REGISTER():m_nType(_DEF_PACK_AUDIO_REGISTER),userid(0){ }
    PackType m_nType;        // 包类型: _DEF_PACK_AUDIO_REGISTER
    int userid;              // 用户ID

}STRU_AUDIO_REGISTER;

// STRU_VIDEO_REGISTER - 视频通道注册
typedef struct STRU_VIDEO_REGISTER{
    STRU_VIDEO_REGISTER():m_nType(_DEF_PACK_VIDEO_REGISTER),userid(0){ }
    PackType m_nType;        // 包类型: _DEF_PACK_VIDEO_REGISTER
    int userid;              // 用户ID

}STRU_VIDEO_REGISTER;
