#ifndef _SYMBOL_HPP_
#define _SYMBOL_HPP_

class Symbol
{
    public:
        enum Type{NSymbol,TSymbol,End};
        int index;
        Type type;
        Symbol():index(-1),type(End){};
        Symbol(int index,Type type) : index(index), type(type){};
        bool operator<(const Symbol& other) const
        {
            return this->index<other.index;
        }
        bool operator==(const Symbol &other) const
        {
            return this->index==other.index&&this->type==other.type;
        }
};

#endif