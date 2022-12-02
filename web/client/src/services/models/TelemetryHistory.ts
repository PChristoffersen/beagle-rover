/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

import type { FloatArray } from './FloatArray';
import type { IntegerArray } from './IntegerArray';

export type TelemetryHistory = {
    /**
     * The time in ms of the lastes entry in the buffer
     */
    time: number;
    /**
     * The interval in ms between buffer entries
     */
    interval: number;
    /**
     * The position head of the buffer ring buffer
     */
    head: number;
    buffer: Array<(FloatArray | IntegerArray)>;
};
