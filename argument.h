#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <QtDebug>

template<typename AlphabetContainerType, typename ElementType, typename StringType, typename HashType>
class Argument
{
public:
    typedef AlphabetContainerType AlphabetType;

private:
    AlphabetType alphabet;
    StringType begin, end;
    StringType currentString;
public:
    typedef int DistanceType;
    typedef int PositionType;

    Argument(AlphabetType al, StringType be, StringType en) : alphabet(al), begin(be), end(en), currentString(be)
    {

    }

    Argument operator+(DistanceType distance)
    {
        Argument<AlphabetContainerType, ElementType, StringType, HashType> ret = *this;

        for(DistanceType i = 0; i < distance; ++i)
        {
            PositionType currentElement = ret.currentString.size() - 1;
            while(currentElement >= 0 &&
                  ret.currentString[currentElement] == alphabet[alphabet.size()-1])
            {
                ret.currentString[currentElement] = alphabet[0];
                --currentElement;
            }

            if(currentElement >= 0)
            {
                ret.currentString[currentElement] = alphabet[alphabet.indexOf(ret.currentString[currentElement]) + 1];
            }
            else {
                StringType newBeggining;
                newBeggining.push_back(alphabet[0]);
                ret.currentString = newBeggining.append(ret.currentString);
            }
        }

        return ret;
    }

    Argument operator++()
    {
        *this = *this + 1;
        return *this;
    }

    Argument operator++(DistanceType)
    {
        Argument<AlphabetContainerType, ElementType, StringType, HashType> ret = *this;
        ++*this;
        return ret;
    }

    StringType getString()
    {
        return currentString;
    }

    void setBegin(ElementType be)
    {
        begin = be;
    }

    StringType getBegin()
    {
        return begin;
    }

    void setEnd(ElementType en)
    {
        end = en;
    }

    StringType getEnd()
    {
        return end;
    }

    void setAsBegin()
    {
        currentString = begin;
    }
};

#endif // ARGUMENT_H
