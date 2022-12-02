/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

import type { NetworkInterfaceBase } from './NetworkInterfaceBase';

export type NetworkInterfaceWifiBase = (NetworkInterfaceBase & {
    /**
     * Wifi SSID
     */
    readonly ssid: string;
    /**
     * Wifi Frequency
     */
    readonly frequency: number;
    /**
     * Wifi Channel
     */
    readonly channel: number;
    /**
     * Wifi Channel width
     */
    readonly channel_width: string;
});
