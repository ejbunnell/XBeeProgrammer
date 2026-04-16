#pragma once

#include <string>

namespace Channel
{
    enum Selections
    {
        C,
        F,
        Null
    };

    inline std::string to_string(Channel::Selections channel)
    {
        switch (channel)
        {
        case Channel::Selections::C:
            return "C";
        case Channel::Selections::F:
            return "F";
        default:
            return "";
        }
    }
}

namespace Bandwidth
{
    enum Selections
    {
        B555,
        B3332,
        Null
    };

    inline std::string to_string(Bandwidth::Selections bandwidth)
    {
        switch (bandwidth)
        {
        case Bandwidth::Selections::B555:
            return "555";
        case Bandwidth::Selections::B3332:
            return "3332";
        default:
            return "";
        }
    }
}