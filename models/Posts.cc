/**
 *
 *  Posts.cc
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#include "Posts.h"
#include <drogon/utils/Utilities.h>
#include <string>

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::sqlite3;

const std::string Posts::Cols::_post_id = "post_id";
const std::string Posts::Cols::_name = "name";
const std::string Posts::Cols::_body = "body";
const std::string Posts::Cols::_created = "created";
const std::string Posts::Cols::_updated = "updated";
const std::string Posts::primaryKeyName = "post_id";
const bool Posts::hasPrimaryKey = true;
const std::string Posts::tableName = "posts";

const std::vector<typename Posts::MetaData> Posts::metaData_={
{"post_id","std::string","text",0,0,1,1},
{"name","std::string","text",0,0,0,1},
{"body","std::string","text",0,0,0,1},
{"created","::trantor::Date","datetime",0,0,0,0},
{"updated","::trantor::Date","datetime",0,0,0,0}
};
const std::string &Posts::getColumnName(size_t index) noexcept(false)
{
    assert(index < metaData_.size());
    return metaData_[index].colName_;
}
Posts::Posts(const Row &r, const ssize_t indexOffset) noexcept
{
    if(indexOffset < 0)
    {
        if(!r["post_id"].isNull())
        {
            postId_=std::make_shared<std::string>(r["post_id"].as<std::string>());
        }
        if(!r["name"].isNull())
        {
            name_=std::make_shared<std::string>(r["name"].as<std::string>());
        }
        if(!r["body"].isNull())
        {
            body_=std::make_shared<std::string>(r["body"].as<std::string>());
        }
        if(!r["created"].isNull())
        {
            auto timeStr = r["created"].as<std::string>();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                created_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
        if(!r["updated"].isNull())
        {
            auto timeStr = r["updated"].as<std::string>();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                updated_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    else
    {
        size_t offset = (size_t)indexOffset;
        if(offset + 5 > r.size())
        {
            LOG_FATAL << "Invalid SQL result for this model";
            return;
        }
        size_t index;
        index = offset + 0;
        if(!r[index].isNull())
        {
            postId_=std::make_shared<std::string>(r[index].as<std::string>());
        }
        index = offset + 1;
        if(!r[index].isNull())
        {
            name_=std::make_shared<std::string>(r[index].as<std::string>());
        }
        index = offset + 2;
        if(!r[index].isNull())
        {
            body_=std::make_shared<std::string>(r[index].as<std::string>());
        }
        index = offset + 3;
        if(!r[index].isNull())
        {
            auto timeStr = r[index].as<std::string>();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                created_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
        index = offset + 4;
        if(!r[index].isNull())
        {
            auto timeStr = r[index].as<std::string>();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                updated_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }

}

Posts::Posts(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 5)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        dirtyFlag_[0] = true;
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            postId_=std::make_shared<std::string>(pJson[pMasqueradingVector[0]].asString());
        }
    }
    if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
    {
        dirtyFlag_[1] = true;
        if(!pJson[pMasqueradingVector[1]].isNull())
        {
            name_=std::make_shared<std::string>(pJson[pMasqueradingVector[1]].asString());
        }
    }
    if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
    {
        dirtyFlag_[2] = true;
        if(!pJson[pMasqueradingVector[2]].isNull())
        {
            body_=std::make_shared<std::string>(pJson[pMasqueradingVector[2]].asString());
        }
    }
    if(!pMasqueradingVector[3].empty() && pJson.isMember(pMasqueradingVector[3]))
    {
        dirtyFlag_[3] = true;
        if(!pJson[pMasqueradingVector[3]].isNull())
        {
            auto timeStr = pJson[pMasqueradingVector[3]].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                created_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    if(!pMasqueradingVector[4].empty() && pJson.isMember(pMasqueradingVector[4]))
    {
        dirtyFlag_[4] = true;
        if(!pJson[pMasqueradingVector[4]].isNull())
        {
            auto timeStr = pJson[pMasqueradingVector[4]].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                updated_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

Posts::Posts(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("post_id"))
    {
        dirtyFlag_[0]=true;
        if(!pJson["post_id"].isNull())
        {
            postId_=std::make_shared<std::string>(pJson["post_id"].asString());
        }
    }
    if(pJson.isMember("name"))
    {
        dirtyFlag_[1]=true;
        if(!pJson["name"].isNull())
        {
            name_=std::make_shared<std::string>(pJson["name"].asString());
        }
    }
    if(pJson.isMember("body"))
    {
        dirtyFlag_[2]=true;
        if(!pJson["body"].isNull())
        {
            body_=std::make_shared<std::string>(pJson["body"].asString());
        }
    }
    if(pJson.isMember("created"))
    {
        dirtyFlag_[3]=true;
        if(!pJson["created"].isNull())
        {
            auto timeStr = pJson["created"].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                created_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    if(pJson.isMember("updated"))
    {
        dirtyFlag_[4]=true;
        if(!pJson["updated"].isNull())
        {
            auto timeStr = pJson["updated"].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                updated_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

void Posts::updateByMasqueradedJson(const Json::Value &pJson,
                                            const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 5)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            postId_=std::make_shared<std::string>(pJson[pMasqueradingVector[0]].asString());
        }
    }
    if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
    {
        dirtyFlag_[1] = true;
        if(!pJson[pMasqueradingVector[1]].isNull())
        {
            name_=std::make_shared<std::string>(pJson[pMasqueradingVector[1]].asString());
        }
    }
    if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
    {
        dirtyFlag_[2] = true;
        if(!pJson[pMasqueradingVector[2]].isNull())
        {
            body_=std::make_shared<std::string>(pJson[pMasqueradingVector[2]].asString());
        }
    }
    if(!pMasqueradingVector[3].empty() && pJson.isMember(pMasqueradingVector[3]))
    {
        dirtyFlag_[3] = true;
        if(!pJson[pMasqueradingVector[3]].isNull())
        {
            auto timeStr = pJson[pMasqueradingVector[3]].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                created_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    if(!pMasqueradingVector[4].empty() && pJson.isMember(pMasqueradingVector[4]))
    {
        dirtyFlag_[4] = true;
        if(!pJson[pMasqueradingVector[4]].isNull())
        {
            auto timeStr = pJson[pMasqueradingVector[4]].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                updated_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

void Posts::updateByJson(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("post_id"))
    {
        if(!pJson["post_id"].isNull())
        {
            postId_=std::make_shared<std::string>(pJson["post_id"].asString());
        }
    }
    if(pJson.isMember("name"))
    {
        dirtyFlag_[1] = true;
        if(!pJson["name"].isNull())
        {
            name_=std::make_shared<std::string>(pJson["name"].asString());
        }
    }
    if(pJson.isMember("body"))
    {
        dirtyFlag_[2] = true;
        if(!pJson["body"].isNull())
        {
            body_=std::make_shared<std::string>(pJson["body"].asString());
        }
    }
    if(pJson.isMember("created"))
    {
        dirtyFlag_[3] = true;
        if(!pJson["created"].isNull())
        {
            auto timeStr = pJson["created"].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                created_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    if(pJson.isMember("updated"))
    {
        dirtyFlag_[4] = true;
        if(!pJson["updated"].isNull())
        {
            auto timeStr = pJson["updated"].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                updated_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

const std::string &Posts::getValueOfPostId() const noexcept
{
    const static std::string defaultValue = std::string();
    if(postId_)
        return *postId_;
    return defaultValue;
}
const std::shared_ptr<std::string> &Posts::getPostId() const noexcept
{
    return postId_;
}
void Posts::setPostId(const std::string &pPostId) noexcept
{
    postId_ = std::make_shared<std::string>(pPostId);
    dirtyFlag_[0] = true;
}
void Posts::setPostId(std::string &&pPostId) noexcept
{
    postId_ = std::make_shared<std::string>(std::move(pPostId));
    dirtyFlag_[0] = true;
}
const typename Posts::PrimaryKeyType & Posts::getPrimaryKey() const
{
    assert(postId_);
    return *postId_;
}

const std::string &Posts::getValueOfName() const noexcept
{
    const static std::string defaultValue = std::string();
    if(name_)
        return *name_;
    return defaultValue;
}
const std::shared_ptr<std::string> &Posts::getName() const noexcept
{
    return name_;
}
void Posts::setName(const std::string &pName) noexcept
{
    name_ = std::make_shared<std::string>(pName);
    dirtyFlag_[1] = true;
}
void Posts::setName(std::string &&pName) noexcept
{
    name_ = std::make_shared<std::string>(std::move(pName));
    dirtyFlag_[1] = true;
}

const std::string &Posts::getValueOfBody() const noexcept
{
    const static std::string defaultValue = std::string();
    if(body_)
        return *body_;
    return defaultValue;
}
const std::shared_ptr<std::string> &Posts::getBody() const noexcept
{
    return body_;
}
void Posts::setBody(const std::string &pBody) noexcept
{
    body_ = std::make_shared<std::string>(pBody);
    dirtyFlag_[2] = true;
}
void Posts::setBody(std::string &&pBody) noexcept
{
    body_ = std::make_shared<std::string>(std::move(pBody));
    dirtyFlag_[2] = true;
}

const ::trantor::Date &Posts::getValueOfCreated() const noexcept
{
    const static ::trantor::Date defaultValue = ::trantor::Date();
    if(created_)
        return *created_;
    return defaultValue;
}
const std::shared_ptr<::trantor::Date> &Posts::getCreated() const noexcept
{
    return created_;
}
void Posts::setCreated(const ::trantor::Date &pCreated) noexcept
{
    created_ = std::make_shared<::trantor::Date>(pCreated);
    dirtyFlag_[3] = true;
}
void Posts::setCreatedToNull() noexcept
{
    created_.reset();
    dirtyFlag_[3] = true;
}

const ::trantor::Date &Posts::getValueOfUpdated() const noexcept
{
    const static ::trantor::Date defaultValue = ::trantor::Date();
    if(updated_)
        return *updated_;
    return defaultValue;
}
const std::shared_ptr<::trantor::Date> &Posts::getUpdated() const noexcept
{
    return updated_;
}
void Posts::setUpdated(const ::trantor::Date &pUpdated) noexcept
{
    updated_ = std::make_shared<::trantor::Date>(pUpdated);
    dirtyFlag_[4] = true;
}
void Posts::setUpdatedToNull() noexcept
{
    updated_.reset();
    dirtyFlag_[4] = true;
}

void Posts::updateId(const uint64_t id)
{
}

const std::vector<std::string> &Posts::insertColumns() noexcept
{
    static const std::vector<std::string> inCols={
        "post_id",
        "name",
        "body",
        "created",
        "updated"
    };
    return inCols;
}

void Posts::outputArgs(drogon::orm::internal::SqlBinder &binder) const
{
    if(dirtyFlag_[0])
    {
        if(getPostId())
        {
            binder << getValueOfPostId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[1])
    {
        if(getName())
        {
            binder << getValueOfName();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[2])
    {
        if(getBody())
        {
            binder << getValueOfBody();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[3])
    {
        if(getCreated())
        {
            binder << getValueOfCreated();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[4])
    {
        if(getUpdated())
        {
            binder << getValueOfUpdated();
        }
        else
        {
            binder << nullptr;
        }
    }
}

const std::vector<std::string> Posts::updateColumns() const
{
    std::vector<std::string> ret;
    if(dirtyFlag_[0])
    {
        ret.push_back(getColumnName(0));
    }
    if(dirtyFlag_[1])
    {
        ret.push_back(getColumnName(1));
    }
    if(dirtyFlag_[2])
    {
        ret.push_back(getColumnName(2));
    }
    if(dirtyFlag_[3])
    {
        ret.push_back(getColumnName(3));
    }
    if(dirtyFlag_[4])
    {
        ret.push_back(getColumnName(4));
    }
    return ret;
}

void Posts::updateArgs(drogon::orm::internal::SqlBinder &binder) const
{
    if(dirtyFlag_[0])
    {
        if(getPostId())
        {
            binder << getValueOfPostId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[1])
    {
        if(getName())
        {
            binder << getValueOfName();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[2])
    {
        if(getBody())
        {
            binder << getValueOfBody();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[3])
    {
        if(getCreated())
        {
            binder << getValueOfCreated();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[4])
    {
        if(getUpdated())
        {
            binder << getValueOfUpdated();
        }
        else
        {
            binder << nullptr;
        }
    }
}
Json::Value Posts::toJson() const
{
    Json::Value ret;
    if(getPostId())
    {
        ret["post_id"]=getValueOfPostId();
    }
    else
    {
        ret["post_id"]=Json::Value();
    }
    if(getName())
    {
        ret["name"]=getValueOfName();
    }
    else
    {
        ret["name"]=Json::Value();
    }
    if(getBody())
    {
        ret["body"]=getValueOfBody();
    }
    else
    {
        ret["body"]=Json::Value();
    }
    if(getCreated())
    {
        ret["created"]=getCreated()->toDbStringLocal();
    }
    else
    {
        ret["created"]=Json::Value();
    }
    if(getUpdated())
    {
        ret["updated"]=getUpdated()->toDbStringLocal();
    }
    else
    {
        ret["updated"]=Json::Value();
    }
    return ret;
}

Json::Value Posts::toMasqueradedJson(
    const std::vector<std::string> &pMasqueradingVector) const
{
    Json::Value ret;
    if(pMasqueradingVector.size() == 5)
    {
        if(!pMasqueradingVector[0].empty())
        {
            if(getPostId())
            {
                ret[pMasqueradingVector[0]]=getValueOfPostId();
            }
            else
            {
                ret[pMasqueradingVector[0]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[1].empty())
        {
            if(getName())
            {
                ret[pMasqueradingVector[1]]=getValueOfName();
            }
            else
            {
                ret[pMasqueradingVector[1]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[2].empty())
        {
            if(getBody())
            {
                ret[pMasqueradingVector[2]]=getValueOfBody();
            }
            else
            {
                ret[pMasqueradingVector[2]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[3].empty())
        {
            if(getCreated())
            {
                ret[pMasqueradingVector[3]]=getCreated()->toDbStringLocal();
            }
            else
            {
                ret[pMasqueradingVector[3]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[4].empty())
        {
            if(getUpdated())
            {
                ret[pMasqueradingVector[4]]=getUpdated()->toDbStringLocal();
            }
            else
            {
                ret[pMasqueradingVector[4]]=Json::Value();
            }
        }
        return ret;
    }
    LOG_ERROR << "Masquerade failed";
    if(getPostId())
    {
        ret["post_id"]=getValueOfPostId();
    }
    else
    {
        ret["post_id"]=Json::Value();
    }
    if(getName())
    {
        ret["name"]=getValueOfName();
    }
    else
    {
        ret["name"]=Json::Value();
    }
    if(getBody())
    {
        ret["body"]=getValueOfBody();
    }
    else
    {
        ret["body"]=Json::Value();
    }
    if(getCreated())
    {
        ret["created"]=getCreated()->toDbStringLocal();
    }
    else
    {
        ret["created"]=Json::Value();
    }
    if(getUpdated())
    {
        ret["updated"]=getUpdated()->toDbStringLocal();
    }
    else
    {
        ret["updated"]=Json::Value();
    }
    return ret;
}

bool Posts::validateJsonForCreation(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("post_id"))
    {
        if(!validJsonOfField(0, "post_id", pJson["post_id"], err, true))
            return false;
    }
    else
    {
        err="The post_id column cannot be null";
        return false;
    }
    if(pJson.isMember("name"))
    {
        if(!validJsonOfField(1, "name", pJson["name"], err, true))
            return false;
    }
    else
    {
        err="The name column cannot be null";
        return false;
    }
    if(pJson.isMember("body"))
    {
        if(!validJsonOfField(2, "body", pJson["body"], err, true))
            return false;
    }
    else
    {
        err="The body column cannot be null";
        return false;
    }
    if(pJson.isMember("created"))
    {
        if(!validJsonOfField(3, "created", pJson["created"], err, true))
            return false;
    }
    if(pJson.isMember("updated"))
    {
        if(!validJsonOfField(4, "updated", pJson["updated"], err, true))
            return false;
    }
    return true;
}
bool Posts::validateMasqueradedJsonForCreation(const Json::Value &pJson,
                                               const std::vector<std::string> &pMasqueradingVector,
                                               std::string &err)
{
    if(pMasqueradingVector.size() != 5)
    {
        err = "Bad masquerading vector";
        return false;
    }
    try {
      if(!pMasqueradingVector[0].empty())
      {
          if(pJson.isMember(pMasqueradingVector[0]))
          {
              if(!validJsonOfField(0, pMasqueradingVector[0], pJson[pMasqueradingVector[0]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[0] + " column cannot be null";
            return false;
        }
      }
      if(!pMasqueradingVector[1].empty())
      {
          if(pJson.isMember(pMasqueradingVector[1]))
          {
              if(!validJsonOfField(1, pMasqueradingVector[1], pJson[pMasqueradingVector[1]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[1] + " column cannot be null";
            return false;
        }
      }
      if(!pMasqueradingVector[2].empty())
      {
          if(pJson.isMember(pMasqueradingVector[2]))
          {
              if(!validJsonOfField(2, pMasqueradingVector[2], pJson[pMasqueradingVector[2]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[2] + " column cannot be null";
            return false;
        }
      }
      if(!pMasqueradingVector[3].empty())
      {
          if(pJson.isMember(pMasqueradingVector[3]))
          {
              if(!validJsonOfField(3, pMasqueradingVector[3], pJson[pMasqueradingVector[3]], err, true))
                  return false;
          }
      }
      if(!pMasqueradingVector[4].empty())
      {
          if(pJson.isMember(pMasqueradingVector[4]))
          {
              if(!validJsonOfField(4, pMasqueradingVector[4], pJson[pMasqueradingVector[4]], err, true))
                  return false;
          }
      }
    }
    catch(const Json::LogicError &e)
    {
      err = e.what();
      return false;
    }
    return true;
}
bool Posts::validateJsonForUpdate(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("post_id"))
    {
        if(!validJsonOfField(0, "post_id", pJson["post_id"], err, false))
            return false;
    }
    else
    {
        err = "The value of primary key must be set in the json object for update";
        return false;
    }
    if(pJson.isMember("name"))
    {
        if(!validJsonOfField(1, "name", pJson["name"], err, false))
            return false;
    }
    if(pJson.isMember("body"))
    {
        if(!validJsonOfField(2, "body", pJson["body"], err, false))
            return false;
    }
    if(pJson.isMember("created"))
    {
        if(!validJsonOfField(3, "created", pJson["created"], err, false))
            return false;
    }
    if(pJson.isMember("updated"))
    {
        if(!validJsonOfField(4, "updated", pJson["updated"], err, false))
            return false;
    }
    return true;
}
bool Posts::validateMasqueradedJsonForUpdate(const Json::Value &pJson,
                                             const std::vector<std::string> &pMasqueradingVector,
                                             std::string &err)
{
    if(pMasqueradingVector.size() != 5)
    {
        err = "Bad masquerading vector";
        return false;
    }
    try {
      if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
      {
          if(!validJsonOfField(0, pMasqueradingVector[0], pJson[pMasqueradingVector[0]], err, false))
              return false;
      }
    else
    {
        err = "The value of primary key must be set in the json object for update";
        return false;
    }
      if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
      {
          if(!validJsonOfField(1, pMasqueradingVector[1], pJson[pMasqueradingVector[1]], err, false))
              return false;
      }
      if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
      {
          if(!validJsonOfField(2, pMasqueradingVector[2], pJson[pMasqueradingVector[2]], err, false))
              return false;
      }
      if(!pMasqueradingVector[3].empty() && pJson.isMember(pMasqueradingVector[3]))
      {
          if(!validJsonOfField(3, pMasqueradingVector[3], pJson[pMasqueradingVector[3]], err, false))
              return false;
      }
      if(!pMasqueradingVector[4].empty() && pJson.isMember(pMasqueradingVector[4]))
      {
          if(!validJsonOfField(4, pMasqueradingVector[4], pJson[pMasqueradingVector[4]], err, false))
              return false;
      }
    }
    catch(const Json::LogicError &e)
    {
      err = e.what();
      return false;
    }
    return true;
}
bool Posts::validJsonOfField(size_t index,
                             const std::string &fieldName,
                             const Json::Value &pJson,
                             std::string &err,
                             bool isForCreation)
{
    switch(index)
    {
        case 0:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isString())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 1:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isString())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 2:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isString())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 3:
            if(pJson.isNull())
            {
                return true;
            }
            if(!pJson.isString())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 4:
            if(pJson.isNull())
            {
                return true;
            }
            if(!pJson.isString())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        default:
            err="Internal error in the server";
            return false;
    }
    return true;
}
