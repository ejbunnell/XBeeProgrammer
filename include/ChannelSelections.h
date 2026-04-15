#pragma once

enum ChannelSelections {
    C,
    F,
    Null
};

std::string to_string(ChannelSelections channel)
{
    switch (channel)
    {
    case ChannelSelections::C:
        return "C";
    case ChannelSelections::F:
        return "F";
    default:
        return "";
    }
}

enum BandwidthSelections {
    B555,
    B3332,
    BNull
};

std::string to_string(BandwidthSelections bandwidth)
{
    switch (bandwidth)
    {
    case BandwidthSelections::B555:
        return "555";
    case BandwidthSelections::B3332:
        return "3332";
    default:
        return "";
    }
}