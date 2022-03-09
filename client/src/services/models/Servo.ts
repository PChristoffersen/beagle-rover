/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

/**
 * Servo properties
 */
export type Servo = {
    /**
     * Whether power to the servo is enabled
     */
    enabled: boolean;
    /**
     * The servo signal pulse width in micro seconds
     */
    pulse_us: number;
    /**
     * Angle of the servo
     */
    angle: number;
    /**
     * The minimum angle of the servo
     */
    readonly limit_min: number;
    /**
     * The maximum angle of the servo
     */
    readonly limit_max: number;
};
