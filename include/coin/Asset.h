/* -*-c++-*- libcoin - Copyright (C) 2012 Michael Gronager
 *
 * libcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * libcoin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libcoin.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _BITCOIN_ASSET_H_
#define _BITCOIN_ASSET_H_

#include <coin/Export.h>
#include <coin/Transaction.h>

//typedef std::pair<uint256, unsigned int> Coin;
typedef std::set<Coin> Coins;

class AssetSyncronizer
{
public:
    typedef std::set<Coin> Coins;
    virtual void getCreditCoins(uint160 btc, Coins& coins) = 0;
    virtual void getDebitCoins(uint160 btc, Coins& coins) = 0;
    virtual void getCoins(uint160 btc, Coins& coins) = 0;
    
    virtual void getTransaction(const Coin& coin, Transaction&) = 0;
};

class Asset;

class COIN_EXPORT Asset : public KeyStore
{
public:
    //    typedef Coin Coin;
    typedef std::map<uint256, Transaction> TxCache;
    typedef std::set<uint256> TxSet;
    typedef std::map<uint160, CKey> KeyMap;
private:
    KeyMap _keymap;
    TxCache _tx_cache;
    Coins _coins;
    
public:
    Asset() {}
    void addAddress(uint160 hash160) { _keymap[hash160]; }
    bool addKey(const CKey& key) { _keymap[toPubKeyHash(key.GetPubKey())] = key; return true; }
    
    std::set<uint160> getAddresses();
    
    virtual bool AddKey(const CKey& key);
    
    virtual bool HaveKey(const ChainAddress &address) const;
    
    virtual bool GetKey(const ChainAddress &address, CKey& keyOut) const;
    
    // we might need to override these as well... Depending how well we want to support having only keys with pub/160 part
    //    virtual bool GetPubKey(const ChainAddress &address, std::vector<unsigned char>& vchPubKeyOut) const
    //    virtual std::vector<unsigned char> GenerateNewKey();
    
    const Transaction& getTx(uint256 hash) const;
    
    uint160 getAddress(const Output& out) const;
    
    void remote_sync();

    void syncronize(AssetSyncronizer& sync, bool all_transactions = false);
    
    const Coins& getCoins()
    {
        return _coins;
    }
    
    bool isSpendable(Coin coin) const;
    const int64 value(Coin coin) const;
    
    struct CompValue {
        const Asset& _asset;
        CompValue(const Asset& asset) : _asset(asset) {}
        bool operator() (Coin a, Coin b)
        {
            return (_asset.value(a) < _asset.value(b));
        }
    };
    
    int64 balance();    
    int64 spendable_balance();
    
    typedef std::pair<uint160, int64> Payment;
    Transaction generateTx(std::set<Payment> payments, uint160 changeaddr = 0);
    
    Transaction generateTx(uint160 to, int64 amount, uint160 change = 0);
};

#endif