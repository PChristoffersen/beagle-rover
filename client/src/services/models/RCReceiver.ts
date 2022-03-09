/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

export type RCReceiver = {
    /**
     * Whether power to the receiver is enabled
     */
    enabled: boolean;
    /**
     * True if the receiver has a connection to the transmitter
     */
    readonly connected: boolean;
    /**
     * Signal strength
     */
    readonly rssi: number;
};
