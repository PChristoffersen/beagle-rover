/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

import type { NetworkInterfaceType } from './NetworkInterfaceType';

export type NetworkInterfaceBase = {
    /**
     * Interface name
     */
    readonly name: string;
    readonly type: NetworkInterfaceType;
    /**
     * Whether the interface is currently active
     */
    readonly active: boolean;
    /**
     * MAC Address of the interface
     */
    readonly mac: string;
    /**
     * List of IpV4 addresses of the interface
     */
    addresses: Array<string>;
};
