//
// Created by Aleksandr on 31.03.2020.
//

#ifndef BIMAP_BIMAP_H
#define BIMAP_BIMAP_H
#include <iostream>
#include <iostream>
#include <map>
#include <set>
#include <functional>
#include <stdexcept>

using std::out_of_range;

namespace container {
    template<typename Tkey, typename Tval>
    class bimap final {
    private:
        using RetPair = std::pair<std::reference_wrapper<const Tkey>, std::reference_wrapper<const Tval>>;
        using Map = std::map<Tkey, Tval>;
        using Setpair = std::pair<std::reference_wrapper<const Tval>, typename Map::const_iterator>;

        struct LocalLess final {
            bool operator()(const Setpair &i1, const Setpair &i2) const {
                return (i1.first < i2.first);
            }
        };

        using Set = std::set<Setpair, LocalLess>;
        std::size_t bimapSize;
        Map firstMap;
        Set secondSet;
    public:
        bimap();

        bimap(const Tkey &key, const Tval &val);

        ~bimap();

        std::size_t size() const;

        bool isEmpty() const;

        bool checkKey(const Tkey &key) const;

        bool checkVal(const Tval &val) const;

        bool insert(const Tkey &key, const Tval &val);

        bool emplace(Tkey &&key, Tval &&val);

        void keyErase(const Tkey &key);

        void valErase(const Tval &val);

        RetPair keyGet(const Tkey &key) const;

        RetPair valGet(const Tval &val) const;
    };

    template<typename Tkey, typename Tval>
    bimap<Tkey, Tval>::bimap() {
        bimapSize = 0;
    }

    template<typename Tkey, typename Tval>
    bimap<Tkey, Tval>::bimap(const Tkey &key, const Tval &val) {
        std::pair<typename bimap::Map::const_iterator, bool> ret;
        ret = firstMap.insert(std::pair<Tkey, Tval>(key, val));
        std::reference_wrapper<const Tval> rwval(val);
        typename bimap::Setpair tempPair(rwval, ret.first);
        secondSet.insert(tempPair);
        bimapSize = 1;
    }

    template<typename Tkey, typename Tval>
    bimap<Tkey, Tval>::~bimap() = default;

    template<typename Tkey, typename Tval>
    std::size_t bimap<Tkey, Tval>::size() const {
        return bimapSize;
    }

    template<typename Tkey, typename Tval>
    bool bimap<Tkey, Tval>::isEmpty() const {
        return !bimapSize;
    }

    template<typename Tkey, typename Tval>
    bool bimap<Tkey, Tval>::checkKey(const Tkey &key) const {
        auto search = firstMap.find(key);
        return search != firstMap.end();
    }

    template<typename Tkey, typename Tval>
    bool bimap<Tkey, Tval>::checkVal(const Tval &val) const {

        for (auto it = secondSet.begin(); it != secondSet.end(); ++it) {
            if (it->first == val) {
                return true;
            }
        }
        return false;
    }

    template<typename Tkey, typename Tval>
    bool bimap<Tkey, Tval>::insert(const Tkey &key, const Tval &val) {
        std::pair<typename bimap::Map::const_iterator, bool> ret;
        ret = firstMap.insert(std::pair<Tkey, Tval>(key, val));
        if (!ret.second) {
            return false;
        }
        std::reference_wrapper<const Tval> rwval(ret.first->second);
        typename bimap::Setpair tempPair(rwval, ret.first);
        std::pair<typename bimap::Set::const_iterator, bool> ret2;
        ret2 = secondSet.insert(tempPair);
        if (!ret2.second) {
            firstMap.erase(ret.first);
            return false;
        }
        bimapSize += 1;
        return true;
    }

    template<typename Tkey, typename Tval>
    bool bimap<Tkey, Tval>::emplace(Tkey &&key, Tval &&val) {
        if (checkKey(key) == 1) {
            return false;
        }
        std::pair<typename bimap::Map::const_iterator, bool> ret;
        ret = firstMap.insert(std::pair<Tkey, Tval>(key, val));

        if (!ret.second || (checkVal(val) == 1)) {
            firstMap.erase(ret.first);
            return false;
        }
        std::reference_wrapper<const Tval> rwval(ret.first->second);
        typename bimap::Setpair tempPair(rwval, ret.first);
        std::pair<typename bimap::Set::const_iterator, bool> ret2;
        ret2 = secondSet.insert(tempPair);
        if (!ret2.second) {
            firstMap.erase(ret.first);
            return false;
        }

        bimapSize += 1;
        return true;
    }

    template<typename Tkey, typename Tval>
    void bimap<Tkey, Tval>::keyErase(const Tkey &key) {
        if (checkKey(key) == 1) {
            auto fmit = firstMap.find(key);
            Tval T = fmit->second;
            for (auto it = secondSet.begin(); it != secondSet.end(); ++it) {
                if (it->first == T) {
                    firstMap.erase(it->second);
                    it = secondSet.erase(it);
                }
            }
        }
        bimapSize -= 1;
    }

    template<typename Tkey, typename Tval>
    void bimap<Tkey, Tval>::valErase(const Tval &val) {
        if (checkVal(val) == 1) {
            for (auto it = secondSet.begin(); it != secondSet.end(); ++it) {
                if (it->first == val) {
                    firstMap.erase(it->second);
                    it = secondSet.erase(it);
                }
            }
        }
        bimapSize -= 1;
    }


    template<typename Tkey, typename Tval>
    typename bimap<Tkey, Tval>::RetPair bimap<Tkey, Tval>::keyGet(const Tkey &key) const {
        std::reference_wrapper<const Tkey> keyRw(secondSet.begin()->second->first);
        std::reference_wrapper<const Tval> valRw(secondSet.begin()->first);
        typename bimap::RetPair tempPair(keyRw, valRw);
        bool flag = false;
        auto it = firstMap.find(key);
        if (it != firstMap.end()) {
            keyRw = it->first;
            valRw = it->second;
            typename bimap::RetPair tempPair2(keyRw, valRw);
            tempPair = tempPair2;
            flag = true;
        }
        if (!flag) {
            throw out_of_range("error");
        } else {
            return tempPair;
        }
    }

    template<typename Tkey, typename Tval>
    typename bimap<Tkey, Tval>::RetPair bimap<Tkey, Tval>::valGet(const Tval &val) const {
        bool flag = false;
        std::reference_wrapper<const Tkey> keyRw(secondSet.begin()->second->first);
        std::reference_wrapper<const Tval> valRw(secondSet.begin()->first);
        typename bimap::RetPair tempPair(keyRw, valRw);
        for (auto it = secondSet.begin(); it != secondSet.end(); ++it) {
            if (it->first == val) {
                keyRw = it->second->first;
                valRw = it->first;
                typename bimap::RetPair tempPair2(keyRw, valRw);
                tempPair = tempPair2;
                flag = true;
            }
        }
        if (!flag) {
            throw out_of_range("error");
        } else {
            return tempPair;
        }
    }
}

#endif //BIMAP_BIMAP_H