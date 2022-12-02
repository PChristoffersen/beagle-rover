/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

import type { Servo } from './Servo';

/**
 * Motor properties
 */
export type Motor = {
    /**
     * Motor
     */
    readonly id: number;
    /**
     * Whether power to the motor is enabled
     */
    enabled: boolean;
    /**
     * Motor duty cycle
     */
    duty: number;
    /**
     * Target RPM of motor
     */
    target_rpm: number;
    /**
     * Motor RPM
     */
    readonly rpm: number;
    /**
     * Value of the encoder counter
     */
    readonly encoder: number;
    /**
     * Odometer value in mm
     */
    readonly odometer: number;
    servo?: Servo;
};
