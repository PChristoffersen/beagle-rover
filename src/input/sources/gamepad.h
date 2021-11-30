#ifndef _INPUT_GAMEPADSOURCE_H_
#define _INPUT_GAMEPADSOURCE_H_

#include <memory>
#include <mutex>

#include "abstractinputsource.h"

namespace Robot::Input {

    class GamepadSource : public AbstractSource<GamepadSource> {
        public:
            explicit GamepadSource(const std::shared_ptr<::Robot::Context> &context, const Signals &signals);
            GamepadSource(const GamepadSource&) = delete; // No copy constructor
            GamepadSource(GamepadSource&&) = delete; // No move constructor
            virtual ~GamepadSource();

            void init();
            void cleanup();

            void setEnabled(bool enabled);

        private:
            bool m_initialized;
            bool m_enabled;

    };

};


#endif
