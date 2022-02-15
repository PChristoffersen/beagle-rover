#ifndef _ROBOT_RC_MAPPING_TARANISX9D_H_
#define _ROBOT_RC_MAPPING_TARANISX9D_H_

#include "mapping.h"

namespace Robot::RC::Mappings {

    class TaranisX9D : public Mapping {
        public:
            TaranisX9D();

            void set(const ChannelList &channels) override;

        private:
    };

}

#endif
