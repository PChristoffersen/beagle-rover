#ifndef _ROBOT_RC_MAPPING_H_
#define _ROBOT_RC_MAPPING_H_

#include <robotconfig.h>
#include "../types.h"

namespace Robot::RC::Mappings {

    class Mapping {
        public:
            Mapping() {}
            virtual ~Mapping() = default;

            virtual void set(const ChannelList &channels) = 0;
        private:
    };

}

#endif