#include "Items.hpp"
#include <iostream>
Item::Item(const int &left,const vector<Symbol> &right, const int &prodIndex, const int &pos, const int &forward)
{
    this->left=left;
    this->right=right;
    this->pos=pos;
    this->forward=forward;
    this->prodIndex=prodIndex;
}

bool Item::operator<(const Item &other) const
{
    if(this->pos!=other.pos||this->left!=other.left||this->right.size()!=other.right.size()
        ||this->pos!=other.pos||this->right!=other.right)
    {
        return true;
    }
    return false;
}

bool Item::operator==(const Item &other) const
{
    if(this->pos!=other.pos||this->left!=other.left||this->forward!=other.forward
        ||this->right!=other.right)
    {
        return false;
    }
    return true;
}

Items::Items()
{

}

Items::Items(ProductionPool *PPool)
{
    this->PPool=PPool;
}

Items::Items(Item &Item,ProductionPool *PPool)
{
    this->PPool=PPool;
    this->Its.push_back(Item);
}

Items::Items(Production &prod,ProductionPool *PPool,const int &forward)
{
    for(int i=0;i<prod.RightsSize();i++)
    {
        Item It=Item(prod.getLeft(),prod.getRight(i).second,prod.getRight(i).first,0,forward);
        this->Its.push_back(It);
    }
    this->PPool=PPool;
}

int Items::size()
{
    return this->Its.size();
}

void Items::closure()
{
    int cnt=0;
    int size=this->Its.size();
    set<int> tempForwards;
    while(cnt<size)
    {
        Item curItem=this->Its[cnt];
        tempForwards.clear();
        if(curItem.pos<curItem.right.size()&&curItem.right[curItem.pos].type==Symbol::NSymbol)
        {
            Symbol curSymbol=curItem.right[curItem.pos];
            int next=curItem.pos+1;
            // find Forwards of curSymbol
            // if nextSymbol exist && is NSymbol
            while(next<curItem.right.size()&&curItem.right[next].type==Symbol::NSymbol)
            {
                Symbol nextSymbol=curItem.right[next];
                set<int> nextSymbolFirstSet=this->PPool->getFirst(nextSymbol.index);
                tempForwards.insert(nextSymbolFirstSet.begin(),nextSymbolFirstSet.end());
                // if nextSymbol's First set do not contain epsilon
                if(!this->PPool->hasEpsilonFirsts[nextSymbol.index])
                {
                    break;
                }
                // else continue
                next++;
            }
            // if nextSymbol exist && is TSymbol
            if(next<curItem.right.size()&&curItem.right[next].type==Symbol::TSymbol)
            {
                tempForwards.insert(curItem.right[next].index);
            }
            else if(next==curItem.right.size())
            {
                tempForwards.insert(curItem.forward);
            }
            // find all productions of curSymbol
            Production curProd=this->PPool->find(curSymbol.index);
            // create Item and insert them into Items
            for(int i=0;i<curProd.RightsSize();i++)
            {
                for(auto forward:tempForwards)
                {
                    Item newItem=Item(curSymbol.index,curProd.getRight(i).second,curProd.getRight(i).first,0,forward);
                    if(!this->ItemExisted(newItem))
                    {
                        this->Its.push_back(newItem);
                        size++;
                    }
                }
            }
        }
        cnt++;
    }
}

void Items::showItems()
{
    for(auto It:this->Its)
    {
        printf("%d->",It.left);
        for(int i=0;i<It.right.size();i++)
        {
            if(i==It.pos)
            {
                printf(".");
            }
            printf("%d ",It.right[i].index);
        }
        printf("\t");
        printf("%d",It.forward);
        printf("\n");
    }
}

Items::~Items()
{
    
}

int Items::go(const Symbol nextSymbol,Items &nextItems)
{
    if(this->Its.size()==0)
    {
        return 0;
    }
    int flag=0;
    // cout<<"Item size "<<this->Its.size()<<endl;
    for(auto item:this->Its)
    {
        // cout<<"go "<<Item.left<<endl;
        if(item.pos<item.right.size()&&item.right[item.pos]==nextSymbol)
        {
            // cout<<"go "<<Item.left<<endl;
            Item nextItem=Item(item.left,item.right,item.prodIndex,item.pos+1,item.forward);
            nextItems.Its.push_back(nextItem);
            flag=1;
        }
    }
    return flag;
}

bool Items::ItemExisted(const Item &Item)
{
    for(auto each:this->Its)
    {
        if(each==Item)
        {
            return true;
        }
    }
    return false;
}

bool Items::operator==(const Items &Items) const
{
    if(this->Its.size()!=Items.Its.size())
    {
        return false;
    }
    for(int i=0;i<this->Its.size();i++)
    {
        if(!(this->Its[i]==Items.Its[i]))
        {
            return false;
        }
    }
    return true;
}