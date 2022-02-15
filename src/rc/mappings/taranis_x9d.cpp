#include "taranis_x9d.h"

#include <chrono>
#include <boost/log/trivial.hpp>

using namespace std::literals;

namespace Robot::RC::Mappings {

TaranisX9D::TaranisX9D() 
{
}



void TaranisX9D::set(const ChannelList &channels) {

    // Sticks
    auto stickL_x { channels[0] };
    auto stickL_y { channels[1] };
    auto stickR_x { channels[2] };
    auto stickR_y { channels[3] };

    // Analogue dials
    auto s1 { channels[4] };
    auto s2 { channels[5] };
    auto sliderL { channels[6] };
    auto sliderR { channels[7] };

    // Buttons
    auto sa { channels[8].asButton(3) };
    auto sb { channels[8].asButton(3) };
    auto sc { channels[8].asButton(3) };
    auto sd { channels[8].asButton(3) };
    auto se { channels[8].asButton(3) };
    auto sf { channels[8].asButton(3) };
    auto sg { channels[8].asButton(3) };
    auto sh { channels[8].asButton(3) };
    auto si { channels[8].asButton(3) };

    static std::chrono::high_resolution_clock::time_point last_update;
    auto time { std::chrono::high_resolution_clock::now() };
        
    if ((time-last_update) > 200ms) {

        BOOST_LOG_TRIVIAL(info) << "Taranis "
            << "lstick=(" << stickL_x.asFloat() << "," << stickL_y.asFloat() << ") "
            << "rstick=(" << stickR_x.asFloat() << "," << stickR_y.asFloat() << ") "
            << "sa=" << sa << " "
            << "sb=" << sb << " "
            << "sc=" << sc << " "
            << "sd=" << sd << " "
            << "se=" << se << " "
            << "sf=" << sf << " "
            << "sg=" << sg << " "
            << "sh=" << sh << " "
            << "si=" << si << " "
            ;
        last_update = time;
    }
}


}
