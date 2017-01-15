#ifndef META_H
#define META_H

template <class T>
constexpr inline int bitcount(T i)
{
    int count=0;
    for(count=0;i;i>>=1)
	count+=i&1;
    return count;
}

template <class T>
constexpr inline int find_lsb(T i)
{
    for(int count=0;i;count++, i>>=1)
	if(i&1)
	    return count;
    return -1;
}

#endif
