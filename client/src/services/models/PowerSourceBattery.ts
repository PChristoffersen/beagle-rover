/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

import type { PowerSource } from './PowerSource';

export type PowerSourceBattery = (PowerSource & {
    readonly charging: boolean;
    readonly on_battery: boolean;
    readonly jack_voltage: number;
    readonly percent: number;
    readonly voltage: number;
});
