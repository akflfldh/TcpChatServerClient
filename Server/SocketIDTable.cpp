#include "SocketIDTable.h"

bool SocketIDTable::AddID(const UserID & id, const ClientInfo &  clientInfo)
{
    
    ClientInfo* pClientInfo = new ClientInfo;
    *pClientInfo = clientInfo;

    std::pair<std::unordered_map<UserID, ClientInfo*>::iterator,bool> result=mIDTable.insert({ id,pClientInfo });
    return result.second;
}

ClientInfo* SocketIDTable::GetClientInfo(const UserID& id) const
{
    std::unordered_map<UserID, ClientInfo*>::const_iterator it = mIDTable.find(id);
    if (it == mIDTable.cend())
    {
        return nullptr;
    }
   return  it->second;
}

bool SocketIDTable::DeleteID(const UserID& id)
{

  std::unordered_map<UserID,ClientInfo*>::iterator result=  mIDTable.find(id);
  if (result != mIDTable.end())
  {
      delete result->second;
      mIDTable.erase(result);
  }

  return true;
}

const std::unordered_map<UserID, ClientInfo*>& SocketIDTable::GetTable() const
{
    return mIDTable;
    // TODO: 여기에 return 문을 삽입합니다.
}
