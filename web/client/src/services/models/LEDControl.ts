/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

import type { AnimationMode } from './AnimationMode';
import type { Color } from './Color';
import type { IndicatorMode } from './IndicatorMode';

export type LEDControl = {
    brightness: number;
    background: Color;
    animation: AnimationMode;
    indicators: IndicatorMode;
};
