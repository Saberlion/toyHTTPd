//
// Created by arthur on 15-8-24.
//

#ifndef TOYHTTPD_CACHE_H
#define TOYHTTPD_CACHE_H

#include <unordered_map>
#include <list>
#include <chrono>

template<typename K, typename V>
class Cache
{
public:
    virtual V get(K key)=0;

    virtual void set(K key, V value)=0;

    virtual void flushData()=0;
};

template<typename K, typename V>
class LruCache:public Cache<K,V>
{
private:
    struct LruCacheNode
    {
        K key_;
        V value_;
        std::chrono::system_clock::time_point timeOutPoint_;

        LruCacheNode(K key, V value):key_(key),value_(value){}
        LruCacheNode(K key, V value, const int expiration)
        {
            LruCacheNode(key,value);
            std::chrono::duration <int > expiration_dur(expiration);//second_type
            timeOutPoint_ = std::chrono::system_clock::now() + expiration_dur;
        }

        bool isTimeOut()
        {
            return std::chrono::system_clock::now() > timeOutPoint_;
        }
    };

    size_t capacity_;
    std::list<LruCacheNode> cacheList_;
    typedef typename std::list<LruCacheNode>::iterator list_iterator;
    std::unordered_map<K, list_iterator> cacheMap_;


public:
    LruCache(size_t capacity):capacity_(capacity){}
    V get(K key) final
    {
        if(cacheMap_.find(key) == cacheMap_.end())
            return -1;
        if((cacheMap_[key])->isTimeOut())
            return -1;
        cacheList_.splice(cacheList_.begin(),cacheList_,cacheMap_[key]);
        cacheMap_[key] = cacheList_.begin();

        return cacheMap_[key]->value_;
    }

    void set(K key, V value) final
    {
        if(cacheMap_.find(key) == cacheMap_.end())
        {
            if(cacheList_.size() == capacity_)
            {
                auto it = prev(cacheList_.end());
                cacheMap_.erase((*it).key_);
                cacheList_.pop_back();

            }
            cacheList_.push_front(LruCacheNode(key,value));
            cacheMap_[key]= cacheList_.begin();
        }
    }
};



template <typename K, typename V>
class LfuCache:public Cache<K,V>
{
private:
    struct LfuCacheNode
    {
        K key_;
        V value_;
        std::chrono::system_clock::time_point timeOutPoint_;
        int usedTimes_;
        LfuCacheNode(K key, V value):key_(key),value_(value),usedTimes_(0){}
        LfuCacheNode(K key, V value, const int expiration)
        {
            LfuCacheNode(key,value);
            std::chrono::duration <int > expiration_dur(expiration);//second_type
            timeOutPoint_ = std::chrono::system_clock::now() + expiration_dur;
        }

        bool isTimeOut()
        {
            return std::chrono::system_clock::now() > timeOutPoint_;
        }
    };

    size_t capacity_;
    std::list<LfuCacheNode> cacheList_;
    typedef typename std::list<LfuCacheNode>::iterator list_iterator;
    std::unordered_map<K, list_iterator> cacheMap_;


public:
    LfuCache(size_t capacity):capacity_(capacity){}
    V get(K key) final
    {
        if(cacheMap_.find(key) == cacheMap_.end())
            return -1;

        if((cacheMap_[key])->isTimeOut())
            return -1;
        cacheMap_[key]->usedTimes_++;
        auto it =  (cacheMap_[key]);
        while(it != cacheList_.begin() && (*it).usedTimes_ < cacheMap_[key]->usedTimes_ )
        {
            it--;
        }
        cacheList_.splice(it,cacheList_,cacheMap_[key]);
        cacheMap_[key] = it;

        return cacheMap_[key]->value_;
    }

    void set(K key, V value) final
    {
        if(cacheMap_.find(key) == cacheMap_.end())
        {
            if(cacheList_.size() == capacity_)
            {
                auto it = prev(cacheList_.end());
                cacheMap_.erase((*it).key_);
                cacheList_.pop_back();

            }
            cacheList_.push_front(LfuCacheNode(key,value));
            cacheMap_[key]= cacheList_.begin();
        }
    }
};



#endif //TOYHTTPD_CACHE_H
