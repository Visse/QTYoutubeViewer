#ifndef KEYLIST_H
#define KEYLIST_H

#include <QList>
#include <QPair>

#include <stdexcept>

/********** Class KeyList ************
  * This class is basicly just a wrapper around QList
  * but it uses keys and values just likes maps do
  * it does NOT sort the items!
*/

#define __THROW_OUT_OF_RANGE__ throw( std::out_of_range(QString("%1:%2:%3").arg(__FUNCTION__,__FILE__,QString::number(__LINE__)).toStdString()) )

template< class KeyType, class ValueType >
class KeyList {
    typedef QPair< KeyType, ValueType > ItemType;
    typedef QList< ItemType > List;
public:
    /// I realy don't understand why i could not only typedef here, but the compiler said to me what i should use 'typename' instead...
    /// Well it said that 'QList<QPair<T1, T2> >' is a dependent scope, no idea that it's mean
    /// Okey had to change it from 'typename' to a simple class the compiler complain that it where not a nametype, the compiler just complains...
    typedef typename List::Iterator Iterator;
    typedef typename List::ConstIterator ConstIterator;


    KeyList()
        :mItems() {}
    virtual ~KeyList() {}


    ValueType& value( const KeyType& Key ) throw( std::out_of_range ) {
        Iterator Iter = _FindKey(Key);
        if( Iter == mItems.end() )
            __THROW_OUT_OF_RANGE__;
        return Iter->second;
    }

    ValueType& value( const KeyType& Key, ValueType& Default ) {
        Iterator Iter = _FindKey(Key);
        if( Iter == mItems.end() )
            return Default;
        return Iter->second;
    }


    uint count( const KeyType& Key ) const {
        uint Count = 0;
        for( ConstIterator iter=begin(); iter != end(); ++iter ) {
            if( iter->first == Key )
                ++Count;
        }
        return Count;
    }
    uint countValues( const  ValueType& Value ) const {
        uint Count = 0;
        for( ConstIterator iter=begin(); iter != end(); ++iter ) {
            if( iter->second == Value )
                ++Count;
        }
        return Count;

    }

    bool countains( const KeyType& Key ) const {
        return _FindKey(Key) != mItems.end();
    }
    bool countainsValue( const ValueType& Value ) const {
        return _FindValue(Value) != mItems.end();
    }


    ValueType& takeFirst() throw(std::out_of_range) {
        if( mItems.isEmpty() )
            __THROW_OUT_OF_RANGE__;
        return mItems.takeFirst().second;
    }

    QPair<KeyType,ValueType> takeFirstPair() throw(std::out_of_range) {
        if( mItems.isEmpty() )
            __THROW_OUT_OF_RANGE__;
        return mItems.takeFirst();
    }

    ValueType& at( uint i ) throw(std::out_of_range) {
        if( i < mItems.size() )
            return mItems.at(i);
        __THROW_OUT_OF_RANGE__;
    }

    Iterator insert( const KeyType& Key, const ValueType& Value ) {
        mItems.push_back(ItemType(Key,Value));
        return mItems.end()-1;
    }

    QList<ValueType> values( const KeyType& Key ) {
        QList<ValueType> Items;
        foreach( const ItemType& Item, mItems ) {
            if( Item.first == Key ) {
                Items.push_back(Item.second);
            }
        }
        return Items;
    }

    Iterator begin() {
        return mItems.begin();
    }

    ConstIterator begin() const {
        return mItems.begin();
    }

    ConstIterator end() const {
        return mItems.end();
    }

    uint size() const {
        return mItems.size();
    }

    void clear() {
        mItems.clear();
    }

    bool isEmpty() const {
        return mItems.isEmpty();
    }

    ValueType& first() throw( std::out_of_range ) {
        if( mItems.isEmpty() )
            __THROW_OUT_OF_RANGE__;
        return mItems.first().second;
    }


    /// const versions...
    const KeyType& key( const ValueType& Value ) const throw( std::out_of_range ) {
        ConstIterator iter = _FindValue(Value);
        if( iter == mItems.end() )
            __THROW_OUT_OF_RANGE__;
        return iter->first;
    }

    const KeyType& key( const ValueType& Value, const KeyType& Default ) const{
        ConstIterator iter = _FindValue(Value);
        if( iter == mItems.end() )
            return Default;
        return iter->first;
    }

    const ValueType& value( const KeyType& Key ) const throw( std::out_of_range ) {
        Iterator Iter = _FindKey(Key);
        if( Iter == mItems.end() )
            __THROW_OUT_OF_RANGE__;
        return Iter->second;
    }

    const ValueType& value( const KeyType& Key, const ValueType& Default ) const {
        ConstIterator Iter = _FindKey(Key);
        if( Iter == mItems.end() )
            return Default;
        return Iter->second;
    }

    const ValueType& at( uint i) const throw( std::out_of_range ) {
        if( i < mItems.size() )
            return mItems.at(i);
        __THROW_OUT_OF_RANGE__;
    }

protected:
    Iterator _FindKey( const KeyType& Key ) {
        for( Iterator iter = mItems.begin(); iter != mItems.end(); ++iter ) {
            if( iter->first == Key )
                return iter;
        }
        return mItems.end();
    }
    ConstIterator _FindKey( const KeyType& Key ) const {
        for( ConstIterator iter = mItems.begin(); iter != mItems.end(); ++iter ) {
            if( iter->first == Key )
                return iter;
        }
        return mItems.end();
    }

    Iterator _FindValue( const ValueType& Value ) {
        for( Iterator iter = mItems.begin(); iter != mItems.end(); ++iter ) {
            if( iter->second == Value )
                return iter;
        }
        return mItems.end();
    }
    ConstIterator _FindValue( const ValueType& Value ) const {
        for( ConstIterator iter = mItems.begin(); iter != mItems.end(); ++iter ) {
            if( iter->second == Value )
                return iter;
        }
        return mItems.end();
    }

    List mItems;
};

#undef __THROW_OUT_OF_RANGE__

#endif // KEYLIST_H
