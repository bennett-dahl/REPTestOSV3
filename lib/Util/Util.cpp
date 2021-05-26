#include <Util.h>

Util::Util(/* args */)
{
}

Util::~Util()
{
}

const char *Util::returnCharPFromBool(bool input)
{
    if (input)
    {
        const char *returnString = "True";

        return returnString;
    }
    const char *returnString = "False";
    return returnString;
    ;
}
