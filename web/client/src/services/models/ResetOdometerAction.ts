/* istanbul ignore file */
/* tslint:disable */
/* eslint-disable */

import type { Action } from './Action';

export type ResetOdometerAction = (Action & {
    id?: ResetOdometerAction.id;
});

export namespace ResetOdometerAction {

    export enum id {
        RESET_ODOMETER = 'resetOdometer',
    }


}
