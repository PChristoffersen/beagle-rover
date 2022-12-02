import { darken, lighten, Theme } from '@mui/material';
import { styled } from '@mui/material/styles';


type VariableProgressColor = 'primary' | 'secondary' | 'error' | 'info' | 'success' | 'warning' | 'inherit';

interface VariableProgressProps {
    value: number;
    max?: number;
    min?: number;
    height?: number;
    transitionTime?: number;
    color?: VariableProgressColor;
}



const getColorShade = (theme: Theme, color: VariableProgressColor) => {
    if (color === 'inherit') {
      return 'currentColor';
    }
  
    return theme.palette.mode === 'light' ? lighten(theme.palette[color].main, 0.62) : darken(theme.palette[color].main, 0.5);
};

  
const Body = styled('span')(({ theme }) => ({
    position: 'relative',
    overflow: 'hidden',
    display: 'block',
    zIndex: 0,
    // Fix Safari's bug during composition of different paint.
    '@media print': {
      colorAdjust: 'exact'
    },
    backgroundColor: getColorShade(theme, "primary")
}));


const Bar = styled('span')(({ theme }) => ({
    position: 'absolute',
    top: 0,
    height: '100%',
    zIndex: 1,
    //transition: 'width 0.2s linear',
    backgroundColor: theme.palette.primary.main,
}));



export default function VariableProgress({ value, max=100, min=-100, height=4, transitionTime }: VariableProgressProps) {
    const range = (max-min);
    const center = (max+min)/2;

    const offset = center - value;

    const frac1 = offset<0 ? -offset/range : 0;
    const frac2 = offset>0 ? offset/range : 0;

    if (transitionTime !== undefined) {
        const time = transitionTime+"s";
        const delay1 = offset<0 ? time : '0s';
        const delay2 = offset>=0 ? time : '0s';

        return (
            <Body sx={{ height: height }}>
                <Bar sx={{ width: (100.0*frac1)+'%', left: '50%', transition: 'width '+time+' linear', transitionDelay: delay1 }}/>
                <Bar sx={{ width: (100.0*frac2)+'%', right: '50%', transition: 'width '+time+' linear', transitionDelay: delay2 }}/>
            </Body>
        )
    }
    else {
        return (
            <Body sx={{ height: height }}>
                <Bar sx={{ width: (100.0*frac1)+'%', left: '50%' }}/>
                <Bar sx={{ width: (100.0*frac2)+'%', right: '50%' }}/>
            </Body>
        )
    }


}