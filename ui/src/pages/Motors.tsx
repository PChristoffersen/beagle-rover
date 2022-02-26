import { range } from 'lodash';
import { Grid } from '@mui/material';
import { DriveMode, useGetKinematicQuery } from '../services/kinematic';
import { InputSource, useGetInputQuery } from '../services/input';
import Page from '../components/Page';
import FullMotorCard from '../components/motor/FullMotorCard';



export default function Motors() {
    const { data: kinematic } = useGetKinematicQuery();
    const { data: input } = useGetInputQuery();
    const enabled = kinematic?.drive_mode === DriveMode.NONE && input?.kinematic_source === InputSource.WEB;

    const numMotors = 4;

    return (
        <Page>
            <Grid container spacing={{ xs: 1, sm: 2 }} sx={{ padding: { xs: 1, sm: 2 } }}>
                {range(numMotors).map((id) => (
                    <Grid key={id} item xs={12} sm="auto">
                        <FullMotorCard key={"motor-"+id} id={id} disabled={!enabled} />
                    </Grid>
                ))}
            </Grid>
        </Page>
    )
}