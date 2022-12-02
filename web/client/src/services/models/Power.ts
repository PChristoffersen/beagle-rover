/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

import type { PowerSourceBattery } from './PowerSourceBattery';
import type { PowerSourceUnknown } from './PowerSourceUnknown';

export type Power = {
    /**
     * System power source
     */
    system: (PowerSourceUnknown | PowerSourceBattery);
};
