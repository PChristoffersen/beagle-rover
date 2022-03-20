/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

import type { NetworkInterfaceEthernet } from './NetworkInterfaceEthernet';
import type { NetworkInterfaceUSB } from './NetworkInterfaceUSB';
import type { NetworkInterfaceWifi } from './NetworkInterfaceWifi';
import type { NetworkInterfaceWifiAP } from './NetworkInterfaceWifiAP';

export type Network = Record<string, (NetworkInterfaceEthernet | NetworkInterfaceWifi | NetworkInterfaceWifiAP | NetworkInterfaceUSB)>;