/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

import type { NetworkInterfaceWifiBase } from './NetworkInterfaceWifiBase';

export type NetworkInterfaceWifi = (NetworkInterfaceWifiBase & {
    /**
     * Wireless signal strength in dBm
     */
    readonly signal_dbm: number;
    /**
     * Wireless signal strength in percent
     */
    readonly signal_rssi: number;
});
