#ifndef _INCLUDE_FAKEQUERIES_NATIVE_H_
#define _INCLUDE_FAKEQUERIES_NATIVE_H_

#include "extension.h"
#include "challenge.h"

#define A2S_PLAYER_REQUEST_LEN 9

struct sockaddr;

struct PlayerInfo_t{
    uint8_t index;
    std::string name;
    uint32_t score;
    float playTime;
};

//Base response handle class
class CReturnHandle
{
public:
    CReturnHandle()
        :m_replyPacket(m_replyStore, 2048)
    {
    }
    
    virtual void BuildCommunicationFrame() = 0;
    
    virtual const char* GetCommunicationFramePtr() {return (const char *)m_replyPacket.GetData();}
    virtual int GetNumBytesWritten() {return m_replyPacket.GetNumBytesWritten();}
    
    virtual void SendTo(int s, int flags, sockaddr* from)
    {
        extern void* g_pSteamSocketMgr;
        extern int g_iSendToOffset;

        #ifdef _WIN32
        ((int(__thiscall*)(void*, int, const char*, int, int, sockaddr*))(*(void***)g_pSteamSocketMgr)[g_iSendToOffset])(g_pSteamSocketMgr, s, GetCommunicationFramePtr(), GetNumBytesWritten(), flags, from);
        #else
        ((int(__cdecl*)(void*, int, const char*, int, int, sockaddr*))(*(void***)g_pSteamSocketMgr)[g_iSendToOffset])(g_pSteamSocketMgr, s, GetCommunicationFramePtr(), GetNumBytesWritten(), flags, from);
        #endif
    }


protected:
    char m_replyStore[2048];
    bf_write m_replyPacket;
};

//A2S_PLAYER response
class CReturnA2sPlayer : public CReturnHandle
{
public:
    CReturnA2sPlayer()
        : m_bDefaultChallengeNumber(true), m_FakePlayerDisplayNum(64)
    {
    }
    
    void InitResourceEntity();  //Resource entity must be initialized in the main thread
    CBaseEntity* GetResourceEntity();
    
    virtual void BuildCommunicationFrame();
    bool RemoveFakePlayer(uint8_t index);
    void SetFakePlayerDisplayNum(uint8_t number){ m_FakePlayerDisplayNum = number; }
    
    void BuildChallengeResponse();
    bool IsValidRequest(char* requestBuf){ return g_ChallengeManager.IsValidChallengeRequest(requestBuf); }
    bool IsOfficialRequest(char* requestBuf);
    bool SetChallengeNumber(uint32_t number, bool bDefault);
    
    void InsertFakePlayer(uint8_t index, char* name, uint32_t score, float playTime)
    {
        PlayerInfo_t info{index, name, score, playTime};
        m_FakePlayers.push_back(std::move(info));
    }
    
    void ClearAllFakePlayer() { m_FakePlayers.clear();}
    
private:
    //Get real client status
    bool GetPlayerStatus(int iClientIndex, PlayerInfo_t& info);

private:
    CBaseHandle m_ResourceEntity;
    
    char m_RequestStrWithChallenge[10];
    uint32_t m_ChallengeNumber;
    bool m_bDefaultChallengeNumber;
    
    uint8_t m_FakePlayerDisplayNum;
    uint8_t m_TotalClientsCount;
    std::vector<PlayerInfo_t> m_FakePlayers;
};

//A2S_INFO respnse
class CReturnA2sInfo : public CReturnHandle
{
public:
    CReturnA2sInfo(){ ResetA2sInfo();}
    
    void ResetA2sInfo();
    bool ReadSteamINF();
    void GameRealDescription();
    void InitRealInformation();
    
    virtual void BuildCommunicationFrame();
    
    void SetPassWord(bool bHavePassword, bool bDefault = false);
    uint8_t GetPassWord();
    
    void SetProtocolVersion(uint8_t iVersion, bool bDefault = false);
    uint8_t GetProtocolVersion();
    
    void SetServerName(const char* sName, bool bDefault = false);
    const char* GetServerName();
    
    void SetMapName(const char* sName, bool bDefault = false);
    const char* GetMapName();
    
    void SetGameFolderName(const char* sName, bool bDefault = false);
    const char* GetGameFolderName();
    
    void SetGameDiscription(const char* sName, bool bDefault = false);
    const char* GetGameDiscription();
    
    void SetAppID(short iId, bool bDefault = false);
    short GetAppID();
    
    void SetNumClients(uint8_t iClientCount, bool bDefault = false);
    uint8_t GetNumClients();
    
    void SetMaxClients(uint8_t iMaxClients,bool bDefault = false);
    uint8_t GetMaxClients();
    
    void SetNumFakeClients(uint8_t iNumFakeClients, bool bDefault = false);
    uint8_t GetNumFakeClients();
    
    bool SetOS(uint8_t os, bool bDefault = false);
    uint8_t GetOS();
    
    void SetVacStatus(bool bVacStatus, bool bDefault = false);
    uint8_t GetVacStatus();
    
    void SetGameVersion(const char* sGameVersion, bool bDefault = false);
    const char* GetGameVersion();
    
    void SetServerTag(const char* sServerTag, bool bDefault = false);
    const char* GetServerTag();

private:
    
    //Real information
    char m_RealGameDir[256];
    char m_RealGameDesc[256];
    char m_RealGameVersion[256];
    int m_RealAppID;
    int m_RealPort;
    uint64_t m_RealSteamId;
    
    //Fake information set by user
    uint8_t m_iPassWord;
    bool m_bDefaultPassWord;
    
    uint8_t m_iProtocolVersion;
    bool m_bDefaultProtocolVersion;
    
    char m_ServerName[256];
    bool m_bDefaultServerName;
    
    char m_MapName[256];
    bool m_bDefaultMapName;
    
    char m_FolderName[256];
    bool m_bDefaultFolderName;
    
    char m_GameDescription[256];
    bool m_bDefaultGameDescription;
    
    short m_iAppID;
    bool m_bDefaultAppID;
    
    uint8_t m_iNumClients;
    bool m_bDefaultNumClients;
    
    uint8_t m_iMaxClients;
    bool m_bDefaultMaxClients;
    
    uint8_t m_iNumFakeClients;
    bool m_bDefaultNumFakeClients;
    
    uint8_t m_OS;
    bool m_bDefaultOS;
    
    uint8_t m_iVacStatus;
    bool m_bDefaultVacStatus;
    
    char m_GameVersion[64];
    bool m_bDefaultGameVersion;
    
    char m_ServerTag[256];
    bool m_bDefaultServerTag;
};

extern bool g_bSteamWorksAPIActivated;

#endif // _INCLUDE_FAKEQUERIES_NATIVE_H_