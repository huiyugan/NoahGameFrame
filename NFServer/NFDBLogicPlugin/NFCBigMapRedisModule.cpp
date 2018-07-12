/*
            This file is part of: 
                NoahFrame
            https://github.com/ketoo/NoahGameFrame

   Copyright 2009 - 2018 NoahFrame(NoahGameFrame)

   File creator: lvsheng.huang
   
   NoahFrame is open-source software and you can redistribute it and/or modify
   it under the terms of the License; besides, anyone who use this file/software must include this copyright announcement.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "NFCBigMapRedisModule.h"
#include "NFComm/NFMessageDefine/NFProtocolDefine.hpp"
#include "NFComm/NFPluginModule/NFINetModule.h"

NFCBigMapRedisModule::NFCBigMapRedisModule(NFIPluginManager * p)
{
	pPluginManager = p;
}

NFCBigMapRedisModule::~NFCBigMapRedisModule()
{

}

bool NFCBigMapRedisModule::Init()
{
	return true;
}

bool NFCBigMapRedisModule::Shut()
{

	return true;
}

bool NFCBigMapRedisModule::AfterInit()
{
	
	m_pNoSqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pLogicClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pGuildRedisModule = pPluginManager->FindModule<NFIGuildRedisModule>();

	return true;
}

bool NFCBigMapRedisModule::Execute()
{
	return true;
}

std::string NFCBigMapRedisModule::GetGridBaseKey()
{
	return "GridBaseKey";
}

std::string NFCBigMapRedisModule::GetGridLeaveMsgKey(const std::string&strGridID)
{
	return "GridLeaveMsgKey_" + strGridID;
}

std::string NFCBigMapRedisModule::GetGridWarHistoryKey(const std::string&strGridID)
{
	return "GridWarHistoryKey_" + strGridID;
}

std::string NFCBigMapRedisModule::GetGridStationHistoryKey(const std::string&strGridID)
{
	return "GridStationKey_" + strGridID;
}

bool NFCBigMapRedisModule::GetGridBaseInfo(const std::string&strGridID, NFMsg::BigMapGridBaseInfo& xBaseInfo)
{
	if (!m_pElementModule->ExistElement(strGridID))
	{
		return false;
	}
	
	NF_SHARE_PTR<NFIRedisClient> pNoSqlDriver = m_pNoSqlModule->GetDriverBySuit(strGridID);
	if (pNoSqlDriver)
	{
		std::string strKey = GetGridBaseKey();
		std::string strData;
		if (pNoSqlDriver->HGET(strKey, strGridID, strData))
		{
			if (xBaseInfo.ParseFromString(strData))
			{
				return true;
			}
		}
	}

	return false;
}

bool NFCBigMapRedisModule::GetGridBaseInfo(const std::vector<std::string>& strGridID, std::vector<NFMsg::BigMapGridBaseInfo>& xBaseInfoList)
{
	NF_SHARE_PTR<NFIRedisClient> pNoSqlDriver = m_pNoSqlModule->GetDriverBySuitConsistent();
	if (pNoSqlDriver)
	{
		std::vector<std::string> vValues;
		std::string strKey = GetGridBaseKey();
		std::string strData;
		if (pNoSqlDriver->HMGET(strKey, strGridID, vValues))
		{
			for (int i = 0; i < vValues.size(); ++i)
			{
				const std::string& strData = vValues[i];
				NFMsg::BigMapGridBaseInfo xBaseInfo;
				if (xBaseInfo.ParseFromString(strData))
				{
					xBaseInfoList.push_back(xBaseInfo);
				}
			}

			return true;
		}
	}

	return false;
}

bool NFCBigMapRedisModule::GetGridBaseInfo(std::vector<NFMsg::BigMapGridBaseInfo>& xBaseInfo)
{
 	NF_SHARE_PTR<NFIClass> xLogicClass = m_pLogicClassModule->GetElement(NFrame::Map::ThisName());
	const std::vector<std::string>& strIdList = xLogicClass->GetIDList();
	std::vector<std::string> vFields; 
	for (int i = 0; i < strIdList.size(); ++i)
	{
		const std::string& strId = strIdList[i];

		vFields.push_back(strId);
 	}

	return GetGridBaseInfo(vFields, xBaseInfo);
}

bool NFCBigMapRedisModule::GetGridLeaveMsgInfo(const std::string&strGridID, std::vector<NFMsg::BigMapLeaveMsg>& xLeaveMsgList)
{
	NF_SHARE_PTR<NFIRedisClient> pNoSqlDriver = m_pNoSqlModule->GetDriverBySuitConsistent();
	if (pNoSqlDriver)
	{
		std::string strKey = GetGridLeaveMsgKey(strGridID);
		std::vector<std::string> xValues;
		if (pNoSqlDriver->LRANGE(strKey, 0, 10, xValues))
		{
			for (int i = 0; i< xValues.size(); ++i)
			{
				NFMsg::BigMapLeaveMsg xLeaveMsgData;
				if (xLeaveMsgData.ParseFromString(xValues[i]))
				{
					xLeaveMsgList.push_back(xLeaveMsgData);
				}
			}

			return true;
		}
	}
	return false;
}

bool NFCBigMapRedisModule::GetGridWarHistoryInfo(const std::string&strGridID, std::vector<NFMsg::BigMapWarHistory>& xWarHistoryList)
{
	NF_SHARE_PTR<NFIRedisClient> pNoSqlDriver = m_pNoSqlModule->GetDriverBySuitConsistent();
	if (pNoSqlDriver)
	{
		std::string strKey = GetGridWarHistoryKey(strGridID);
		std::vector<std::string> xValues;
		if (pNoSqlDriver->LRANGE(strKey, 0, 10, xValues))
		{
			for (int i = 0; i< xValues.size(); ++i)
			{
				NFMsg::BigMapWarHistory xWarData;
				if (xWarData.ParseFromString(xValues[i]))
				{
					xWarHistoryList.push_back(xWarData);
				}
			}

			return true;
		}
	}

	return false;
}

bool NFCBigMapRedisModule::GetGridStationInfo(const std::string& strGridID, std::vector<NFMsg::GridGuildBaseInfo>& xWarHistoryList)
{
	/*
	if (!m_pElementModule->ExistElement(strGridID))
	{
		return false;
	}

	NF_SHARE_PTR<NFIRedisClient> pNoSqlDriver = m_pNoSqlModule->GetDriverBySuitConsistent();
	if (pNoSqlDriver)
	{
		std::string strKey = GetGridStationHistoryKey(strGridID);
		int nCount = 0;
		if (pNoSqlDriver->ZCard(strKey, nCount))
		{
			std::vector<std::pair<std::string, double> > memberScoreList;
			if (pNoSqlDriver->ZRevRange(strKey, 0, nCount, memberScoreList))
			{
				std::vector<std::pair<std::string, double> >::iterator it = memberScoreList.begin();
				for (it; it != memberScoreList.end(); it++)
				{
					std::string strGuildID = it->first;
					NFGUID xGuildID;
					if (xGuildID.FromString(strGuildID))
					{
						NF_SHARE_PTR<NFIPropertyManager> xPropertyManager = m_pGuildRedisModule->GetGuildCachePropertyInfo(xGuildID);
						if (xPropertyManager)
						{
							NFGUID xGuildID = xPropertyManager->GetPropertyObject(NFrame::Guild::GuildID());
							const int nLevel = xPropertyManager->GetPropertyInt(NFrame::Guild::GuildLevel());
							const int nCount = xPropertyManager->GetPropertyInt(NFrame::Guild::GuildMemeberCount());
							const int nResource = xPropertyManager->GetPropertyInt(NFrame::Guild::KingWarResource());
							const std::string& strIcon = xPropertyManager->GetPropertyString(NFrame::Guild::GuildIcon());

							NFMsg::GridGuildBaseInfo xGridGuildBaseInfo;
							xGridGuildBaseInfo.mutable_id()->CopyFrom(NFINetModule::NFToPB(xGuildID));
							xGridGuildBaseInfo.set_level(nLevel);
							xGridGuildBaseInfo.set_count(nCount);
							xGridGuildBaseInfo.set_resource(nResource);
							xGridGuildBaseInfo.set_icon(strIcon);

							xWarHistoryList.push_back(xGridGuildBaseInfo);
						}
					}
				}
			}
		}
	}
	*/

	return false;
}

bool NFCBigMapRedisModule::SetGridBaseInfo(const std::string&strGridID, const NFMsg::BigMapGridBaseInfo& xBaseInfo)
{
	if (!m_pElementModule->ExistElement(strGridID))
	{
		return false;
	}

	NF_SHARE_PTR<NFIRedisClient> pNoSqlDriver = m_pNoSqlModule->GetDriverBySuitConsistent();
	if (pNoSqlDriver)
	{
		std::string strKey = GetGridBaseKey();
		std::string strData;
		if (xBaseInfo.SerializeToString(&strData))
		{
			return pNoSqlDriver->HSET(strKey, strGridID, strData);
		}
	}

	return false;
}

bool NFCBigMapRedisModule::AddGridLeaveMsgInfo(const std::string&strGridID, const NFMsg::BigMapLeaveMsg& xLeaveMsg)
{
	if (!m_pElementModule->ExistElement(strGridID))
	{
		return false;
	}

	
	NF_SHARE_PTR<NFIRedisClient> pNoSqlDriver = m_pNoSqlModule->GetDriverBySuitConsistent();
	if (pNoSqlDriver)
	{
		std::string strKey = GetGridLeaveMsgKey(strGridID);
		std::string strData;
		if (xLeaveMsg.SerializeToString(&strData))
		{
			return pNoSqlDriver->LPUSH(strKey, strData);
		}
	}

	return false;
}

bool NFCBigMapRedisModule::AddGridWarHistoryInfo(const std::string&strGridID, const NFMsg::BigMapWarHistory& xWarHistory)
{
	if (!m_pElementModule->ExistElement(strGridID))
	{
		return false;
	}

	
	NF_SHARE_PTR<NFIRedisClient> pNoSqlDriver = m_pNoSqlModule->GetDriverBySuitConsistent();
	if (pNoSqlDriver)
	{
		std::string strKey = GetGridWarHistoryKey(strGridID);
		std::string strData;
		if (xWarHistory.SerializeToString(&strData))
		{
			//
			return pNoSqlDriver->LPUSH(strKey, strData);
		}
	}

	return false;
}

bool NFCBigMapRedisModule::AddGridStationInfo(const std::string& strGridID, const NFGUID& self, const int nResource)
{
	if (!m_pElementModule->ExistElement(strGridID))
	{
		return false;
	}

	
	NF_SHARE_PTR<NFIRedisClient> pNoSqlDriver = m_pNoSqlModule->GetDriverBySuitConsistent();
	if (pNoSqlDriver)
	{
		std::string strKey = GetGridStationHistoryKey(strGridID);
		return pNoSqlDriver->ZADD(strKey, self.ToString(), nResource);
	}

	return false;
}

bool NFCBigMapRedisModule::RemoveGridStationInfo(const std::string& strGridID, const NFGUID& self, const int nResource)
{
	if (!m_pElementModule->ExistElement(strGridID))
	{
		return false;
	}

	
	NF_SHARE_PTR<NFIRedisClient> pNoSqlDriver = m_pNoSqlModule->GetDriverBySuitConsistent();
	if (pNoSqlDriver)
	{
		std::string strKey = GetGridStationHistoryKey(strGridID);
		return pNoSqlDriver->ZREM(strKey, self.ToString());
	}

	return false;
}
